COMPATIBLE_MACHINE:rasp3-64-custom = "rasp3-64-custom"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI += "file://0001-dts-Add-PCF8574-device-to-I2C1-on-Raspberry-Pi-3B.patch \
            file://0002-dts-Add-PCF8574-device-to-I2C1-on-Raspberry-Pi-3B.patch \
            file://my_pcf857x.cfg"


KERNEL_CONFIG_FRAGMENTS += "file://my_pcf857x.cfg"
