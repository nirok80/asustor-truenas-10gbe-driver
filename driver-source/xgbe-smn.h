// SPDX-License-Identifier: GPL-2.0-only
/*
 * AMD 10Gb Ethernet driver
 *
 * Copyright (c) 2024, Advanced Micro Devices, Inc.
 * All Rights Reserved.
 *
 * Author: Raju Rangoju <Raju.Rangoju@amd.com>
 */

#ifndef __SMN_H__
#define __SMN_H__

#ifdef CONFIG_AMD_NB

#include <asm/amd_nb.h>

#else

static inline int amd_smn_write(u16 node, u32 address, u32 value)
{
	return -ENODEV;
}

static inline int amd_smn_read(u16 node, u32 address, u32 *value)
{
	return -ENODEV;
}

#endif
#endif
