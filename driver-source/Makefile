# SPDX-License-Identifier: GPL-2.0-only

TARGET         ?= $(shell uname -r)
KERNEL_MODULES := /lib/modules/$(TARGET)
KERNEL_BUILD   := $(KERNEL_MODULES)/build
SYSTEM_MAP     := /boot/System.map-$(TARGET)
DRIVER         := amd-xgbe
DRIVER_VERSION := 6.6.44

amd-xgbe_DEST_DIR      = $(KERNEL_MODULES)/kernel/drivers/net/ethernet/amd/xgbe

obj-m  := $(patsubst %,%.o,$(DRIVER))
obj-ko := $(patsubst %,%.ko,$(DRIVER))

amd-xgbe-objs := xgbe-main.o xgbe-drv.o xgbe-dev.o \
		 xgbe-desc.o xgbe-ethtool.o xgbe-mdio.o \
		 xgbe-ptp.o \
		 xgbe-i2c.o xgbe-phy-v1.o xgbe-phy-v2.o \
		 xgbe-platform.o

amd-xgbe-$(CONFIG_PCI) += xgbe-pci.o
amd-xgbe-$(CONFIG_AMD_XGBE_DCB) += xgbe-dcb.o
amd-xgbe-$(CONFIG_DEBUG_FS) += xgbe-debugfs.o


all: modules

modules:
	@$(MAKE) -C $(KERNEL_BUILD) M=$(CURDIR) modules

install: modules_modules

modules_modules:
	$(foreach mod,$(DRIVER),/usr/bin/install -m 644 -D $(mod).ko $($(mod)_DEST_DIR)/$(mod).ko;)
	depmod -a -F $(SYSTEM_MAP) $(TARGET)

clean:
	$(MAKE) -C $(KERNEL_BUILD) M=$(CURDIR) clean

.PHONY: all modules install modules_install clean
