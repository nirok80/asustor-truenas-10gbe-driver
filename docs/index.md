---
title: "Fixing ASUSTOR Flashstor 10GbE on TrueNAS SCALE 25.10"
description: "How to get the AMD XGMAC 10GbE NIC working on ASUSTOR Flashstor Gen2 with TrueNAS SCALE 25.10.1 (kernel 6.12)"
---

# Fixing ASUSTOR Flashstor 10GbE on TrueNAS SCALE 25.10

If you've got an **ASUSTOR Flashstor Gen2** (FS6812X or FS6806X) and tried to install TrueNAS SCALE 25.10.1, you probably discovered that the 10GbE NIC doesn't work. No network means a useless NAS.

This guide explains the problem and provides a working solution.

## The Hardware

The ASUSTOR Flashstor Gen2 is an impressive all-NVMe NAS:
- 12-bay (FS6812X) or 6-bay (FS6806X) M.2 NVMe slots
- AMD Ryzen Embedded V3C14 CPU
- Integrated AMD XGMAC 10GbE NIC (`[1022:1458]`)

The problem? That AMD 10GbE NIC has no out-of-box driver support in TrueNAS.

## The Problem

Someone at [mihnea.net](https://mihnea.net/asustor-flashstor-fs6812xfs6806x-experimental-truenas-support/) created a patched `amd-xgbe` driver that worked great with **TrueNAS 24.10.x** (kernel 6.6.44).

But when you upgrade to **TrueNAS SCALE 25.10.1** (kernel 6.12), the driver won't compile:

```
xgbe-ethtool.c: error: incompatible type for argument 2 of 'xgbe_get_rxfh'
xgbe-ethtool.c: error: incompatible type for argument 2 of 'xgbe_set_rxfh'
xgbe-ethtool.c: error: incompatible type for argument 2 of 'xgbe_get_ts_info'
xgbe-pci.c: error: 'PCI_IRQ_LEGACY' undeclared
```

## The Solution

The kernel APIs changed between 6.6 and 6.12. I wrote a patch that fixes all 4 breaking changes:

1. **RSS hash functions** — Changed from separate parameters to a single `ethtool_rxfh_param` struct
2. **Timestamp info struct** — `ethtool_ts_info` renamed to `kernel_ethtool_ts_info`
3. **PCI IRQ flag** — `PCI_IRQ_LEGACY` renamed to `PCI_IRQ_INTX`

## Installation

### Step 1: Enable Development Tools

SSH into your TrueNAS box and run:

```bash
sudo install-dev-tools
```

### Step 2: Download and Extract

```bash
# Download the pre-patched driver
wget https://github.com/nirok80/asustor-truenas-10gbe-driver/releases/download/v1.0.0/truenas-amd-xgbe-asustor-6.12-fixed.tar.bz2

# Extract
tar xvf truenas-amd-xgbe-asustor-6.12-fixed.tar.bz2
cd truenas-amd-xgbe-asustor-6.6.44
```

### Step 3: Compile and Install

```bash
make
sudo make install
sudo modprobe amd-xgbe
```

### Step 4: Verify

```bash
ip link show
```

You should see your 10GbE interface (typically `enp2s0f0`).

### Step 5: Make Persistent

Without this step, the driver disappears after reboot:

```bash
sudo mount -o remount,rw 'boot-pool/ROOT/25.10.1/'
sudo update-initramfs -u -k $(uname -r)
sudo mount -o remount,ro 'boot-pool/ROOT/25.10.1/'
```

## Caveats

1. **TrueNAS updates overwrite kernel modules** — Recompile after every update
2. **Unsupported configuration** — Using `install-dev-tools` voids your TrueNAS support
3. **Future kernels** — Kernel 6.15+ will need more patches

## Resources

- [GitHub Repository](https://github.com/nirok80/asustor-truenas-10gbe-driver) — Source code and patch
- [Original mihnea.net driver](https://mihnea.net/asustor-flashstor-fs6812xfs6806x-experimental-truenas-support/) — Where the original 6.6 patch came from
- [phillarson-xyz/amd-xgbe-patched-proxmox](https://github.com/phillarson-xyz/amd-xgbe-patched-proxmox) — Similar patches for Proxmox

## Contributing

Found a bug or have improvements? [Open an issue or PR](https://github.com/nirok80/asustor-truenas-10gbe-driver/issues).
