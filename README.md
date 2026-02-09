# AMD XGBE 10GbE Driver for ASUSTOR Flashstor + TrueNAS SCALE

Patched AMD XGBE driver for **ASUSTOR Flashstor Gen2 (FS6812X/FS6806X)** NAS devices running **TrueNAS SCALE 25.10.1+** (kernel 6.12).

## The Problem

The ASUSTOR Flashstor Gen2 has an AMD Ryzen Embedded CPU with an integrated **AMD XGMAC 10GbE NIC** (`[1022:1458]`). This NIC has no driver support out-of-the-box in TrueNAS SCALE — meaning no network on your NAS.

The original patched driver from [mihnea.net](https://mihnea.net/asustor-flashstor-fs6812xfs6806x-experimental-truenas-support/) worked with TrueNAS 24.10.x (kernel 6.6.44), but fails to compile on TrueNAS SCALE 25.10.1+ due to kernel API changes in kernel 6.12.

## The Solution

This repo provides a patched driver that compiles on kernel 6.12. The patch addresses 4 breaking API changes:

| API Change | Old (6.6) | New (6.12) |
|------------|-----------|------------|
| RSS hash getter | `xgbe_get_rxfh(netdev, indir, key, hfunc)` | `xgbe_get_rxfh(netdev, ethtool_rxfh_param)` |
| RSS hash setter | `xgbe_set_rxfh(netdev, indir, key, hfunc)` | `xgbe_set_rxfh(netdev, ethtool_rxfh_param, extack)` |
| Timestamp info | `ethtool_ts_info` | `kernel_ethtool_ts_info` |
| PCI IRQ flag | `PCI_IRQ_LEGACY` | `PCI_IRQ_INTX` |

## Quick Start

### Prerequisites

- TrueNAS SCALE 25.10.1 installed (you'll need temporary network access via USB Ethernet or similar)
- SSH access to TrueNAS

### Installation

```bash
# Enable development tools
sudo install-dev-tools

# Download the pre-patched driver
wget https://github.com/nirok80/asustor-truenas-10gbe-driver/releases/download/v1.0.0/truenas-amd-xgbe-asustor-6.12-fixed.tar.bz2

# Extract
tar xvf truenas-amd-xgbe-asustor-6.12-fixed.tar.bz2
cd truenas-amd-xgbe-asustor-6.6.44

# Compile and install
make
sudo make install

# Load the module
sudo modprobe amd-xgbe

# Verify it works
ip link show  # Should show enp2s0f0 or similar
```

### Make Persistent (Survives Reboot)

```bash
# Make root filesystem writable
sudo mount -o remount,rw 'boot-pool/ROOT/25.10.1/'

# Update initramfs to include the driver
sudo update-initramfs -u -k $(uname -r)

# Make root filesystem read-only again
sudo mount -o remount,ro 'boot-pool/ROOT/25.10.1/'
```

## Manual Patching (Alternative)

If you prefer to patch the original driver yourself:

```bash
# Download original driver from mihnea.net
wget https://mihnea.net/truenas-amd-xgbe-asustor-6.6.44.tar.bz2
tar xvf truenas-amd-xgbe-asustor-6.6.44.tar.bz2
cd truenas-amd-xgbe-asustor-6.6.44

# Download and apply the patch
wget https://raw.githubusercontent.com/nirok80/asustor-truenas-10gbe-driver/main/kernel-6.12-fix.patch
patch -p1 < kernel-6.12-fix.patch

# Compile and install
make
sudo make install
```

## Important Notes

1. **TrueNAS updates overwrite kernel modules** — You must recompile and reinstall the driver after every TrueNAS update.

2. **Unsupported configuration** — Using `install-dev-tools` and custom kernel modules makes your system unsupported by official TrueNAS channels.

3. **Future kernel updates** — Kernel 6.15+ will require additional patches (`timer_container_of` replaces `from_timer`, `timer_delete_sync` replaces `del_timer_sync`).

## Hardware Info

| Component | Details |
|-----------|---------|
| NAS | ASUSTOR Flashstor Gen2 FS6812X (12-bay) / FS6806X (6-bay) |
| CPU | AMD Ryzen Embedded V3C14 |
| NIC | AMD XGMAC 10GbE Controller `[1022:1458]` |
| OS | TrueNAS SCALE 25.10.1 (kernel 6.12) |

## Files

| File | Description |
|------|-------------|
| `driver-source/` | Patched driver source code (ready to compile) |
| `kernel-6.12-fix.patch` | The patch file for kernel 6.12 compatibility |
| `truenas-amd-xgbe-asustor-6.12-fixed.tar.bz2` | Pre-patched driver tarball |

## Credits

- **Original driver patch:** [mihnea.net](https://mihnea.net/asustor-flashstor-fs6812xfs6806x-experimental-truenas-support/)
- **Proxmox patches reference:** [phillarson-xyz/amd-xgbe-patched-proxmox](https://github.com/phillarson-xyz/amd-xgbe-patched-proxmox)
- **AMD upstream driver:** Linux kernel `drivers/net/ethernet/amd/xgbe/`

## License

The AMD XGBE driver is licensed under GPL-2.0, as per the Linux kernel.

## Contributing

Found a bug or have improvements? PRs welcome!
