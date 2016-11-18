#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = /home/jin/ti/tirex-content/tirtos_msp43x_2_20_00_06/packages;/home/jin/ti/tirex-content/tirtos_msp43x_2_20_00_06/products/bios_6_46_00_23/packages;/home/jin/ti/tirex-content/tirtos_msp43x_2_20_00_06/products/tidrivers_msp43x_2_20_00_08/packages;/home/jin/ti/tirex-content/tirtos_msp43x_2_20_00_06/products/uia_2_00_06_52/packages;/home/jin/ti/ccsv6/ccs_base
override XDCROOT = /home/jin/ti/xdctools_3_32_01_22_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /home/jin/ti/tirex-content/tirtos_msp43x_2_20_00_06/packages;/home/jin/ti/tirex-content/tirtos_msp43x_2_20_00_06/products/bios_6_46_00_23/packages;/home/jin/ti/tirex-content/tirtos_msp43x_2_20_00_06/products/tidrivers_msp43x_2_20_00_08/packages;/home/jin/ti/tirex-content/tirtos_msp43x_2_20_00_06/products/uia_2_00_06_52/packages;/home/jin/ti/ccsv6/ccs_base;/home/jin/ti/xdctools_3_32_01_22_core/packages;..
HOSTOS = Linux
endif
