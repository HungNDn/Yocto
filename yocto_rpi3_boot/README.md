
# Boot Process Rasp3-64
**Date:** 24/07/2025

## 1. Preparing

- Image, `*.dtb` (Ex: `bcm2710-rpi-3-b-plus.dtb`)
- `*.rootfs.tar.bz2` (Ex: `core-image-minimal-{BOARD}.rootfs.tar.bz2`)

---

## 2. Choose a Machine with HubControl Installed

Example: `192.168.2.32`

---

## 3. Access the Machine

**From Host:**  
Login via SSH:

```bash
ssh 192.168.2.32
```

---

## 4. Choose Device Rasp3 and Prepare for Boot

### Prepare Rootfs:

```bash
mkdir /home/nfs/rpi3/{MAC address}/
# Example:
mkdir /home/nfs/rpi3/01-b8-27-eb-4b-a1-91

mkdir /home/nfs/rpi3/{MAC address}/user_name
# Example:
mkdir /home/nfs/rpi3/01-b8-27-eb-4b-a1-91/hung.nghiem-dinh
```

Put `*.rootfs.tar.bz2` to the above folder and extract:

```bash
tar -jxvf core-image-minimal-{BOARD}.rootfs.tar.bz2
```

---

### Prepare Image and DTB:

```bash
mkdir /home/tftpboot/rpi3/{MAC address}/
# Example:
mkdir /home/tftpboot/rpi3/01-b8-27-eb-4b-a1-91/

mkdir /home/tftpboot/rpi3/{MAC address}/user_name
# Example:
mkdir /home/tftpboot/rpi3/01-b8-27-eb-4b-a1-91/hung.nghiem-dinh
```

Put Image and DTB files to this folder.

---

### Configure PXE:

Edit PXE config:

```bash
nano /home/tftpboot/pxelinux.cfg/01-b8-27-eb-4b-a1-91_rpi
```

Insert the following content:

```
LABEL hung.nghiem-dinh
      MENU hung.nghiem-dinh
      LINUX rpi3/01-b8-27-eb-4b-a1-91/hung.nghiem-dinh/Image
      FDT rpi3/01-b8-27-eb-4b-a1-91/hung.nghiem-dinh/bcm2710-rpi-3-b-plus.dtb
      APPEND ${cbootargs} ${bootargs} 8250.nr_uarts=1 console=ttyS0,115200 rw ip=dhcp root=/dev/nfs nfsroot=192.168.10.232:/home/nfs/rpi3/01-b8-27-eb-4b-a1-91/hung.nghiem-dinh,nfsvers=3,tcp
```

---

## 5. Boot Process

### Terminal 1: Serial Console

```bash
minicom -b 115200 -D /dev/serial/by-path/pci-0000:00:14.0-usb-0:2.1.2:1.0-port0
```

### Terminal 2: HubControl

Run:

```bash
hubcontrol
```

hubcontrol => choose device => choose reset 

Select device with corresponding MAC address:

```
0.  Hub01-RaspberryPi3B-01   0x23    RaspberryPi3B   B8:27:EB:01:E2:F8  AVAILABLE  
1.  Hub02-RaspberryPi3B-01   0x25    RaspberryPi3B   B8:27:EB:4B:A1:91  AVAILABLE  
2.  Hub02-RaspberryPi3B-02   0x25    RaspberryPi3B   B8:27:EB:C5:EC:6D  AVAILABLE  
...
8.  Quit Hub Control
```

Available actions after selecting a device:

```
0. Power On Board
1. Power Off Board
2. Reset
3. Get Power
4. Get Output
5. Set Pinout High
6. Set Pinout Low
7. Quit
```

**Boot will proceed automatically and be visible in Terminal 1.**

---
