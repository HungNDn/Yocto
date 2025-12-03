
# Yocto Setup for Raspberry Pi3-64
**Date:** 23/07/2025

---

## 1. Create a Working Directory

```bash
mkdir yocto
cd yocto
mkdir sources
cd sources
```

---

## 2. Getting the Meta Layers

**Poky** is the Yocto reference distribution. It contains all the basic meta layers:

```bash
git clone git://git.yoctoproject.org/poky -b dunfell
```

**meta-raspberrypi** is the BSP layer required to support Raspberry Pi boards:

```bash
git clone git://git.yoctoproject.org/meta-raspberrypi -b dunfell
```

**meta-openembedded** includes additional necessary layers such as meta-oe, meta-networking, etc.:

```bash
git clone https://git.openembedded.org/meta-openembedded -b dunfell
```

---

## 3. Setup Build Environment

- Initialize the build environment using `oe-init-build-env` from the `poky` directory:

```bash
cd yocto
source ./sources/poky/oe-init-build-env
```

---

## 4. Configuration

### local.conf

- Used for user configuration. Set the target machine:

```bash
nano build/conf/local.conf
```

- Add or modify the line:

```conf
MACHINE ?= "raspberrypi3-64"
```

---

### bblayers.conf

- Defines paths to the meta layers in use:

```bash
nano build/conf/bblayers.conf
```

- Edit as follows:

```conf
POKY_BBLAYERS_CONF_VERSION = "2"

BBPATH = "${TOPDIR}"
BBFILES ?= ""

BBLAYERS ?= " \
  ${TOPDIR}/../sources/poky/meta \
  ${TOPDIR}/../sources/poky/meta-poky \
  ${TOPDIR}/../sources/poky/meta-yocto-bsp \
  ${TOPDIR}/../sources/meta-raspberrypi \
  ${TOPDIR}/../sources/meta-openembedded/meta-oe \
  ${TOPDIR}/../sources/meta-openembedded/meta-multimedia \
  ${TOPDIR}/../sources/meta-openembedded/meta-networking \
  ${TOPDIR}/../sources/meta-openembedded/meta-python \
"
```

---

## 5. Build the Image

- Build a minimal bootable image for Raspberry Pi:

```bash
bitbake core-image-minimal
```

- Generated images will be located at:

```
build/tmp/deploy/images/raspberrypi3-64/
```
