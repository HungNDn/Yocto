SUMMARY = "Firmware for ath6k Wi-Fi and wireless regulatory database"
DESCRIPTION = "Install ath6k firmware and regulatory.db for wireless operation"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${THISDIR}/files/COPYING.MIT;md5=838c366f69b72c5df05c96dff79b35f2"

SRC_URI = "\
    git://git.kernel.org/pub/scm/linux/kernel/git/firmware/linux-firmware.git;branch=main;protocol=https;name=firmware;subdir=linux-firmware \
    git://git.kernel.org/pub/scm/linux/kernel/git/sforshee/wireless-regdb.git;branch=master;protocol=https;name=regdb;subdir=wireless-regdb \
"

SRCREV_FORMAT = "firmware_regdb"
SRCREV_firmware = "919daee84a86592c8fe6710e16a788dfb706962a"
SRCREV_regdb    = "37dcea0e6e5effb4228fe385e906edba3cbee389"

S = "${WORKDIR}/linux-firmware"
REGDB_S = "${WORKDIR}/wireless-regdb"

inherit allarch

do_install() {
    install -d ${D}${nonarch_base_libdir}/firmware/ath6k/AR6003/hw2.1.1/
    install -m 0644 ${S}/ath6k/AR6003/hw2.1.1/* ${D}${nonarch_base_libdir}/firmware/ath6k/AR6003/hw2.1.1/

    if [ -f ${D}${nonarch_base_libdir}/firmware/ath6k/AR6003/hw2.1.1/bdata.SD32.bin ]; then
        mv ${D}${nonarch_base_libdir}/firmware/ath6k/AR6003/hw2.1.1/bdata.SD32.bin ${D}${nonarch_base_libdir}/firmware/ath6k/AR6003/hw2.1.1/bdata.bin
    fi

    install -m 0644 ${REGDB_S}/regulatory.db ${D}${nonarch_base_libdir}/firmware/
    install -m 0644 ${REGDB_S}/regulatory.db.p7s ${D}${nonarch_base_libdir}/firmware/
}

FILES:${PN} += "${nonarch_base_libdir}/firmware/"

