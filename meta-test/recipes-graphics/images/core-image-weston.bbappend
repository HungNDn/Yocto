IMAGE_INSTALL:append = " \
    util-linux \
    e2fsprogs \
    dosfstools \
    iperf3 \
    stress-ng \
    ethtool \
    net-tools \
    lrzsz \
    inetutils \
    inetutils-dbg \
    inetutils-dev \
    inetutils-doc \
    inetutils-ftp \
    inetutils-ftpd \
    inetutils-hostname \
    inetutils-ifconfig \
    inetutils-inetd \
    inetutils-logger \
    inetutils-ping6 \
    inetutils-src \
    inetutils-syslogd \
    inetutils-telnet \
    inetutils-telnetd \
    inetutils-telnetd-dbg \
    inetutils-tftp \
    inetutils-tftpd \
    inetutils-tftpd-dbg \
    inetutils-traceroute \
    trace-cmd \
    alsa-utils \
    alsa-lib \
    make \
    perf \
    opkg \
    linuxptp \
"

python __anonymous() {
    import bb
    bb.warn("core-image-weston.bbappend from meta-test has been applied!")
}