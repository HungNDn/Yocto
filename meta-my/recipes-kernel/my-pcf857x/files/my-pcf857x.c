#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/gpio/driver.h>
#include <linux/interrupt.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/mutex.h>
#include <linux/version.h>

#define DRV_NAME "my_pcf857x"

struct my_pcf857x {
	struct gpio_chip chip;
	struct i2c_client *client;
	struct mutex lock;
	u8 out_state;
	int irq;
};

static int my_pcf857x_read(struct i2c_client *client)
{
	s32 val = i2c_smbus_read_byte(client);
	if (val < 0)
		dev_err(&client->dev, "i2c read failed\n");
	return val;
}

static int my_pcf857x_write(struct i2c_client *client, u8 value)
{
	return i2c_smbus_write_byte(client, value);
}

static int my_gpio_get(struct gpio_chip *chip, unsigned int offset)
{
	struct my_pcf857x *dev = gpiochip_get_data(chip);
	int val;

	mutex_lock(&dev->lock);
	val = my_pcf857x_read(dev->client);
	mutex_unlock(&dev->lock);

	return !!(val & BIT(offset));
}

static void my_gpio_set(struct gpio_chip *chip, unsigned int offset, int value)
{
	struct my_pcf857x *dev = gpiochip_get_data(chip);

	mutex_lock(&dev->lock);
	if (value)
		dev->out_state |= BIT(offset);
	else
		dev->out_state &= ~BIT(offset);

	my_pcf857x_write(dev->client, dev->out_state);
	mutex_unlock(&dev->lock);
}

static int my_gpio_direction_input(struct gpio_chip *chip, unsigned int offset)
{
	// PCF857x: Để ngõ vào = ghi 1 vào bit đó
	struct my_pcf857x *dev = gpiochip_get_data(chip);

	mutex_lock(&dev->lock);
	dev->out_state |= BIT(offset); // ghi 1 để set thành input
	my_pcf857x_write(dev->client, dev->out_state);
	mutex_unlock(&dev->lock);

	return 0;
}

static int my_gpio_direction_output(struct gpio_chip *chip, unsigned int offset, int value)
{
	my_gpio_set(chip, offset, value);
	return 0;
}

static irqreturn_t my_irq_handler(int irq, void *dev_id)
{
	struct my_pcf857x *dev = dev_id;
	int val = my_pcf857x_read(dev->client);
	dev_dbg(&dev->client->dev, "IRQ: GPIO state = 0x%02x\n", val);

	return IRQ_HANDLED;
}

static int my_pcf857x_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct my_pcf857x *gpio;
	int ret;

	gpio = devm_kzalloc(&client->dev, sizeof(*gpio), GFP_KERNEL);
	if (!gpio)
		return -ENOMEM;

	gpio->client = client;
	mutex_init(&gpio->lock);
	gpio->out_state = 0xFF; // Default HIGH trên tất cả chân

    /*
        gpio->chip.label = dev_name(&client->dev);  => cat /sys/class/gpio/gpiochipx/label = 1-0020(địa chỉ reg i2c trong device tree)
        gpio->chip.label = "pcf857x" => cat /sys/class/gpio/gpiochipx/label = pcf857x là đúng của mình
    */
   
	gpio->chip.label = dev_name(&client->dev); 
	gpio->chip.parent = &client->dev;
	gpio->chip.owner = THIS_MODULE;
	gpio->chip.base = -1;
	gpio->chip.ngpio = 8;
	gpio->chip.can_sleep = true;

	gpio->chip.get = my_gpio_get;
	gpio->chip.set = my_gpio_set;
	gpio->chip.direction_input = my_gpio_direction_input;
	gpio->chip.direction_output = my_gpio_direction_output;

	ret = gpiochip_add_data(&gpio->chip, gpio);
	if (ret) {
		dev_err(&client->dev, "failed to register gpiochip: %d\n", ret);
		return ret;
	}

	/* Gắn IRQ nếu có khai báo trong Device Tree */
	gpio->irq = of_irq_get(client->dev.of_node, 0);
	if (gpio->irq > 0) {
		ret = devm_request_threaded_irq(&client->dev, gpio->irq,
						NULL, my_irq_handler,
						IRQF_ONESHOT | IRQF_TRIGGER_LOW,
						dev_name(&client->dev), gpio);
		if (ret)
			dev_warn(&client->dev, "Failed to request IRQ: %d\n", ret);
		else
			dev_info(&client->dev, "IRQ %d registered\n", gpio->irq);
	} else {
		dev_info(&client->dev, "No IRQ configured\n");
	}

	i2c_set_clientdata(client, gpio);
	dev_info(&client->dev, "PCF857x GPIO driver registered\n");

	return 0;
}

static int my_pcf857x_remove(struct i2c_client *client)
{
	struct my_pcf857x *gpio = i2c_get_clientdata(client);
	gpiochip_remove(&gpio->chip);
	return 0;
}

/* 
"my_pcf857x" là tên thiết bị (device name) để kernel match với driver thông qua i2c_device_id.
Khi thiết bị /dev/i2c-1, địa chỉ 0x20, được probe, 
kernel sẽ tìm driver nào có tên khớp với node compatible = "my_pcf857x" trong device tree hoặc file modules.alias.
ls -l /sys/bus/i2c/devices/1-0020/driver => ... -> ../../../../bus/i2c/drivers/my_pcf857x
basename $(readlink /sys/bus/i2c/devices/1-0020/driver) = my_pcf857x
*/

static const struct i2c_device_id my_pcf857x_id[] = {
	{ "my_pcf857x", 0 }, 
	{ }
};
MODULE_DEVICE_TABLE(i2c, my_pcf857x_id);

static const struct of_device_id my_pcf857x_of_match[] = {
	{ .compatible = "pcf8574" }, // device tree match
	{ }
};
MODULE_DEVICE_TABLE(of, my_pcf857x_of_match);

static struct i2c_driver my_pcf857x_driver = {
	.driver = {
		.name = DRV_NAME,
		.of_match_table = my_pcf857x_of_match,
	},
	.probe = my_pcf857x_probe,
	.remove = my_pcf857x_remove,
	.id_table = my_pcf857x_id,
};

module_i2c_driver(my_pcf857x_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Manifest");
MODULE_DESCRIPTION("Custom PCF857x GPIO Driver with Device Tree support");
