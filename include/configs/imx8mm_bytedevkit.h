/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2025 bytesatwork AG - https://www.bytesatwork.io
 *
 * Based on imx8mm_evk.h:
 * Copyright 2018 NXP
 */

#ifndef __IMX8MM_BYTEDEVKIT_H
#define __IMX8MM_BYTEDEVKIT_H

#include <linux/sizes.h>
#include <asm/arch/imx-regs.h>
#include "imx_env.h"

#define BAW_CONFIG_BUILTIN_PCB		M6_PCB_REV_1_0
#define BAW_CONFIG_BUILTIN_RAM		M6_RAM_MT53E128M32D2DS_053
#define BAW_CONFIG_BUILTIN_FLASH	EMMC_16GB
#define BAW_CONFIG_RAM_SIZE_PTR_ADDR	0x0091ffe0

#define CFG_SYS_UBOOT_BASE	\
	(QSPI0_AMBA_BASE + CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR * 512)

#ifdef CONFIG_SPL_BUILD
/* malloc f used before GD_FLG_FULL_MALLOC_INIT set */
#define CFG_MALLOC_F_ADDR		0x930000

#endif

/* Initial environment variables */
#define CFG_EXTRA_ENV_SETTINGS \
	"image=boot/Image\0" \
	"console=ttymxc0,115200 earlycon=ec_imx6q,0x30890000,115200\0" \
	"fdt_addr=0x43000000\0" \
	"fdt_file=boot/imx8mm-bytedevkit.dtb\0" \
	"initrd_addr=0x43800000\0" \
	"mmcroot=/dev/mmcblk1p1 rootwait rw\0" \
	"emmcroot=/dev/mmcblk0p1 rootwait rw\0" \
	"bootargs=setenv bootargs console=${console} root=${fsroot} quiet " \
		"${kernelargs} ${bootargs_append}\0" \
	"mmcargs=setenv mmcdev 1; setenv fsroot ${mmcroot}; run bootargs;\0" \
	"emmcargs=setenv mmcdev 2; setenv fsroot ${emmcroot}; run bootargs;\0" \
	"loadimage=load mmc ${mmcdev} ${loadaddr} ${image}\0" \
	"loadfdt=echo Searching for Device Tree ...; " \
		"setenv fdt_file ${fdt_path}${fdt_file}; " \
		"if load mmc ${mmcdev} ${fdt_addr} ${fdt_file}; then " \
			"echo Loading ${fdt_file}; " \
		"fi;\0" \
	"update-spi=if load mmc 1:1 ${loadaddr} flash.bin; then; " \
		"sf probe; sf erase 0 0x200000; " \
		"sf write ${loadaddr} 0 ${filesize}; fi\0" \
	"genericboot=" \
		"if run loadimage; then " \
			"if run loadfdt; then " \
				"booti ${loadaddr} - ${fdt_addr}; " \
			"else; " \
				"echo ERROR: Could not load the DT; " \
			"fi; " \
		"else; " \
			"echo ERROR: Could not load ${image}; " \
		"fi;\0" \
	"emmcboot=echo Booting from eMMC ...; " \
		"run emmcargs; " \
		"run genericboot;\0" \
	"mmcboot=echo Booting from MMC ...; " \
		"run mmcargs; " \
		"if mmc rescan; then " \
			"run genericboot;" \
		"else; " \
			"echo No MMC card detected!; " \
		"fi;\0" \
	"bsp_bootcmd=echo Running BSP bootcmd ...; " \
		"if mmc dev 1; then " \
			"run mmcboot;" \
		"else; " \
			"run emmcboot;" \
		"fi; "

/* Link Definitions */
#define CFG_SYS_INIT_RAM_ADDR		0x40000000
#define CFG_SYS_INIT_RAM_SIZE		0x80000

#define CFG_SYS_SDRAM_BASE		0x40000000
#define PHYS_SDRAM			0x40000000
#define PHYS_SDRAM_SIZE			0x20000000 /* 512 MB DDR */

#define CFG_MXC_UART_BASE		UART2_BASE_ADDR

/* USDHC */
#define CFG_SYS_FSL_USDHC_NUM	2
#define CFG_SYS_FSL_ESDHC_ADDR	0

#define SPL_I2C_SPEED			100000

#endif
