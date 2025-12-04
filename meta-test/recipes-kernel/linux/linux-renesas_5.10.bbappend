################################################################################
# File: linux-renesas_5.10.bbappend
# Mục đích:
# - Thêm common FTRACE config
# - Mode-specific patches/config
# - Task riêng để log thông tin mode khi cần
################################################################################
 
################################################################################
# 0️⃣ FILESEXTRAPATHS & MODE TEST
################################################################################
 
FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
 
MODE ?= "DEFAULT"
 
################################################################################
# 2️⃣ Mode-specific patches & config
################################################################################
 
python () {
    mode = d.getVar("MODE", True) or "DEFAULT"
 
    # Thêm vào overrides để tự động apply mode-specific bbappend hoặc flags
    if mode != "DEFAULT":
        d.appendVar("OVERRIDES", ":%s" % mode)
 
    # Khởi tạo danh sách
    mode_patches = []
    mode_frags = []
 
    # Gán patch và config theo mode
    if mode == "VIN_TEST":
        mode_frags   = ["file://vin/vin.cfg"]
        mode_patches = ["file://vin/0001-CONFIG_VIDEO_RCAR_VIN_DEBUG.patch"]
    elif mode == "IDLE_TEST":
        mode_frags = ["file://idle/idle.cfg"]
 
    elif mode == "USB2H_TEST":
        mode_patches = ["file://usb2h/0001-usb3f-g_zero.patch"]
        mode_frags = ["file://usb2h/usb2h.cfg"]
    elif mode == "FREQ_TEST":
        mode_frags = ["file://freq/freq.cfg"]
    elif mode == "GPIO_TEST":
        mode_frags = ["file://gpio/gpio.cfg"]
 
    elif mode == "SD_TEST":
        mode_frags = ["file://sd/sd.cfg"]
 
    elif mode == "RUNTIME_TEST":
        mode_patches = ["file://runtime/0001-pm-runtime-normal-test.patch"]
 
    elif mode == "UIO_TEST":
        mode_frags = ["file://uio/uio.cfg"]
 
    elif mode == "SCIF_DMA_TEST":
        mode_frags = ["file://scif/scif-dma.cfg"]
 
    elif mode == "SCIF_PIO_TEST":
        mode_frags = ["file://scif/scif-pio.cfg"]
 
    elif mode == "SCIF_DMA_DEBUG_TEST":
        mode_frags = ["file://scif/scif-dma-debug.cfg"]
 
    elif mode == "SCIF_PIO_DEBUG_TEST":
        mode_frags = ["file://scif/scif-pio-debug.cfg"]
 
    elif mode == "USB3F_TEST":
        mode_frags = ["file://usb3f/usb3f.cfg"]
        mode_patches = ["file://usb3f/0001-usb3f-g_zero.patch"]
    elif mode == "USB3H_TEST":
        mode_frags = ["file://usb3h/usb3h.cfg"]
 
    elif mode == "DEFAULT":
        bb.warn("MODE is set to DEFAULT or unrecognized. No patch/config will be applied.")
 
    else:
        bb.warn("MODE '%s' is not recognized. No patch/config will be applied." % mode)
 
    
    mode_frags += ["file://common/memleak.cfg"]
    mode_frags += ["file://common/deadlock.cfg"]
    mode_frags += ["file://common/ftrace.cfg"]
    # Append SRC_URI nếu có
    all_uri = " ".join(mode_patches + mode_frags)
    if all_uri:
        d.appendVar("SRC_URI", " %s" % all_uri)
 
    # Append config fragments nếu có
    for frag in mode_frags:
        d.appendVar("KERNEL_CONFIG_FRAGMENTS", " %s" % frag)
 
    ################# Log block ##################
    msg = "\n" + "*"*100 + "\n\n"
    msg += "MODE: %s\n\n" % mode
    msg += "-"*100 + "\n"
    msg += "SRC_URI patches applied:\n"
    for p in mode_patches:
        msg += "  - %s\n" % p
 
    msg += "\nKERNEL_CONFIG_FRAGMENTS applied:\n"
    for f in mode_frags:
        msg += "  - %s\n" % f
 
    msg += "-"*100 + "\n\n"
    msg += "*"*100 + "\n\n"
 
    bb.note(msg)
    ################# Log block ##################
}