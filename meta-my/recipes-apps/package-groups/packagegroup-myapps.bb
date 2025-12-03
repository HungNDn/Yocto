
SUMMARY = "Custom package group for my applications"
DESCRIPTION = "This package group includes custom and useful packages"
LICENSE = "MIT"

PACKAGE_ARCH = "${MACHINE_ARCH}"

inherit packagegroup

RDEPENDS:${PN} = " example my-pcf857x simple-char simple-app nano htop openssh i2c-tools"
