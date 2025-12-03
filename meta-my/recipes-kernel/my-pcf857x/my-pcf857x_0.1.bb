SUMMARY = "kernel recipe"
DESCRIPTION = "PCF857x Driver"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${THISDIR}/files/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

inherit module

SRC_URI = "file://my-pcf857x.c \
           file://Makefile \
           file://Kconfig"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

S = "${WORKDIR}"

DEPENDS += "virtual/kernel"

EXTRA_OEMAKE = "KERNEL_SRC=${STAGING_KERNEL_DIR}"

do_compile() {
    oe_runmake
}

do_install() {
    install -d ${D}/lib/modules/${KERNEL_VERSION}/extra
    install -m 0644 my-pcf857x.ko ${D}/lib/modules/${KERNEL_VERSION}/extra
}

