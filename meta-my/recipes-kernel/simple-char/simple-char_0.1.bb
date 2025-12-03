SUMMARY = "kernel recipe"
DESCRIPTION = "Simple Character Driver"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${THISDIR}/files/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

SRC_URI = "file://simple_char_drv.c \
           file://Makefile"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

S = "${WORKDIR}"

inherit module

DEPENDS += "virtual/kernel"

EXTRA_OEMAKE = "KERNEL_SRC=${STAGING_KERNEL_DIR}"

do_compile() {
    oe_runmake
}

do_install() {
    install -d ${D}/lib/modules/${KERNEL_VERSION}/extra
    install -m 0644 simple_char_drv.ko ${D}/lib/modules/${KERNEL_VERSION}/extra
}

