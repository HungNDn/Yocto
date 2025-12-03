SUMMARY = "apps recipe"
DESCRIPTION = "This is a recipe of meta-my layer which contain my source"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${THISDIR}/files/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

SRC_URI = "file://simple-app.c"
FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

S = "${WORKDIR}"

python do_display_banner() {
    bb.plain("***********************************************")
    bb.plain("*                                             *")
    bb.plain("*  Apps recipe created by bitbake-layers   *")
    bb.plain("*                                             *")
    bb.plain("***********************************************")
}

addtask display_banner before do_compile after do_unpack

do_compile() {
    ${CC} ${CFLAGS} ${LDFLAGS} simple-app.c -o simple-app
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 simple-app ${D}${bindir}/simple-app
}
