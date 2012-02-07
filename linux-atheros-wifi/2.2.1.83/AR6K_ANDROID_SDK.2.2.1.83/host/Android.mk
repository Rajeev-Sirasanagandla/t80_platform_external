#Android makefile to build kernel as a part of Android Build

ifneq ($(TARGET_SIMULATOR),true)
ifeq ($(BOARD_WLAN_CHIP_AR6102), true)
LOCAL_PATH := $(call my-dir)
ATH_SRC := $(LOCAL_PATH)

ATH_LINUXPATH := kernel_imx
#export  ATH_SRC_BASE=$(ATH_SRC)
#export  ATH_BUILD_TYPE=FSL_ARM_NATIVEMMC
#export  ATH_BUS_TYPE=sdio
#export  ATH_OS_SUB_TYPE=linux_2_6
#export  ATH_LINUXPATH=kernel_imx
#export  ATH_ARCH_CPU_TYPE=arm
#export  ATH_BUS_SUBTYPE=linux_sdio
export  ATH_ANDROID_FW_PATH=yes
#export  ATH_EEPROM_FILE_USED=no
#export  ATH_SOFTMAC_USED=no

ATH_ANDROID_ROOT:= $(CURDIR)
ATH_CROSS_COMPILE_TYPE:=$(ATH_ANDROID_ROOT)/prebuilt/linux-x86/toolchain/arm-eabi-4.4.0/bin/arm-eabi-
export  ATH_CROSS_COMPILE_TYPE

ATH_HIF_TYPE:=sdio
#ATH_SRC_BASE:= .

mod_file := $(TARGET_OUT)/lib/modules/ar6102.ko
$(mod_file) :  $(ACP)
	cd $(ATH_SRC) && ./comp.sh 2 ${PWD}/$(ATH_LINUXPATH)
	cd $(ATH_SRC) && ./comp.sh 1 ${PWD}/$(ATH_LINUXPATH)
	mkdir -p $(TARGET_OUT)/lib/modules
#	$(MAKE) ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabi- -C $(ATH_LINUXPATH) ATH_HIF_TYPE=$(ATH_HIF_TYPE) SUBDIRS=${PWD}/$(ATH_SRC)/os/linux modules
	$(ACP) $(ATH_SRC)/os/linux/ar6000.ko $(TARGET_OUT)/lib/modules/ar6102.ko

ALL_PREBUILT+=$(mod_file)
endif
endif
