// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2021 bytes at work AG - https://www.bytesatwork.io
 *
 * Based on mx8mm_evk/imx8mm_evk.c:
 * Copyright 2018-2019 NXP
 */

#include <common.h>
#include <malloc.h>
#include <errno.h>
#include <asm/io.h>
#include <miiphy.h>
#include <netdev.h>
#include <asm/mach-imx/iomux-v3.h>
#include <asm-generic/gpio.h>
#include <fsl_esdhc.h>
#include <mmc.h>
#include <asm/arch/imx8mm_pins.h>
#include <asm/arch/sys_proto.h>
#include <asm/mach-imx/gpio.h>
#include <asm/mach-imx/mxc_i2c.h>
#include <asm/arch/clock.h>
#include <spl.h>
#include <asm/mach-imx/dma.h>
#include <power/pmic.h>
#include <power/bd71837.h>
#include "../common/tcpc.h"
#include <usb.h>
#include <asm/mach-imx/video.h>
#include <power/pca9450.h>

DECLARE_GLOBAL_DATA_PTR;

#define UART_PAD_CTRL	(PAD_CTL_DSE6 | PAD_CTL_FSEL1)
#define WDOG_PAD_CTRL	(PAD_CTL_DSE6 | PAD_CTL_ODE | PAD_CTL_PUE | PAD_CTL_PE)

static iomux_v3_cfg_t const uart_pads[] = {
	IMX8MM_PAD_UART2_RXD_UART2_RX | MUX_PAD_CTRL(UART_PAD_CTRL),
	IMX8MM_PAD_UART2_TXD_UART2_TX | MUX_PAD_CTRL(UART_PAD_CTRL),
};

static iomux_v3_cfg_t const wdog_pads[] = {
	IMX8MM_PAD_GPIO1_IO02_WDOG1_WDOG_B  | MUX_PAD_CTRL(WDOG_PAD_CTRL),
};

#ifdef CONFIG_FSL_FSPI
int board_qspi_init(void)
{
	init_clk_fspi(0);

	return 0;
}
#endif

int board_early_init_f(void)
{
	struct wdog_regs *wdog = (struct wdog_regs *)WDOG1_BASE_ADDR;

	imx_iomux_v3_setup_multiple_pads(wdog_pads, ARRAY_SIZE(wdog_pads));

	set_wdog_reset(wdog);

	imx_iomux_v3_setup_multiple_pads(uart_pads, ARRAY_SIZE(uart_pads));

	init_uart_clk(1);

	return 0;
}

#ifdef CONFIG_FEC_MXC
#define FEC_RST_PAD IMX_GPIO_NR(5, 27)
static iomux_v3_cfg_t const fec1_rst_pads[] = {
	IMX8MM_PAD_UART3_TXD_GPIO5_IO27 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

static void setup_iomux_fec(void)
{
	imx_iomux_v3_setup_multiple_pads(fec1_rst_pads,
					 ARRAY_SIZE(fec1_rst_pads));

	gpio_request(FEC_RST_PAD, "fec1_rst");
	gpio_direction_output(FEC_RST_PAD, 0);
	udelay(500);
	gpio_direction_output(FEC_RST_PAD, 1);
}

static int setup_fec(void)
{
	struct iomuxc_gpr_base_regs *gpr =
		(struct iomuxc_gpr_base_regs *)IOMUXC_GPR_BASE_ADDR;

	setup_iomux_fec();

	/* Use 125M anatop REF_CLK1 for ENET1, not from external */
	clrsetbits_le32(&gpr->gpr[1],
			IOMUXC_GPR_GPR1_GPR_ENET1_TX_CLK_SEL_MASK, 0);
	return set_clk_enet(ENET_125MHZ);
}

int board_phy_config(struct phy_device *phydev)
{
	/* enable rgmii rxc skew and phy mode select to RGMII copper */
	phy_write(phydev, MDIO_DEVAD_NONE, 0x1d, 0x1f);
	phy_write(phydev, MDIO_DEVAD_NONE, 0x1e, 0x8);

	phy_write(phydev, MDIO_DEVAD_NONE, 0x1d, 0x00);
	phy_write(phydev, MDIO_DEVAD_NONE, 0x1e, 0x82ee);
	phy_write(phydev, MDIO_DEVAD_NONE, 0x1d, 0x05);
	phy_write(phydev, MDIO_DEVAD_NONE, 0x1e, 0x100);

	if (phydev->drv->config)
		phydev->drv->config(phydev);
	return 0;
}
#endif

int board_init(void)
{

#ifdef CONFIG_FEC_MXC
	setup_fec();
#endif

#ifdef CONFIG_FSL_FSPI
	board_qspi_init();
#endif

	return 0;
}

static int pmic_i2c_reg_write(struct udevice *dev, uint addr, uint8_t data)
{
	return dm_i2c_write(dev, addr, &data, sizeof(data));
}

static void pmic_i2c_init(void)
{
	struct udevice *bus, *p;
	int i2c_bus = 1;
	int ret;

	ret = uclass_get_device_by_seq(UCLASS_I2C, i2c_bus, &bus);
	if (ret) {
		printf("%s: No bus %d\n", __func__, i2c_bus);
		return;
	}

	ret = dm_i2c_probe(bus, 0x25, 0, &p);
	if (ret) {
		printf("%s: Can't find device id=0x%x, on bus %d\n",
			__func__, 0x25, i2c_bus);
		return;
	}

	/* BUCKxOUT_DVS0/1 control BUCK123 output */
	pmic_i2c_reg_write(p, PCA9450_BUCK123_DVS, 0x29);

	/* Buck 1 DVS control through PMIC_STBY_REQ */
	pmic_i2c_reg_write(p, PCA9450_BUCK1CTRL, 0x59);

	/* decrease RESET key long push time from the default 10s to 10ms */
	/* Ton_Deb of PCA9450 is 20ms and don't change */

	/* increase VDD_SOC to typical value 0.85v before first DRAM access */
	/* pmic_reg_write(p, PCA9450_BUCK1OUT_DVS0, 0x14); */
	pmic_i2c_reg_write(p, PCA9450_BUCK1OUT_DVS0, 0x14);
	pmic_i2c_reg_write(p, PCA9450_BUCK1OUT_DVS1, 0x10);

	/* increase VDD_DRAM to 0.975v for 3Ghz DDR -> 0.95V instead of 0.975V, */
	/* because PCA9450 Buck3 can set 0.95V */
	/* Also, set B3_ENMODE=2 (ON by PMIC_ON_REQ=H & PMIC_STBY_REQ=L) */
	pmic_i2c_reg_write(p, PCA9450_BUCK3OUT_DVS0, 0x14);
	pmic_i2c_reg_write(p, PCA9450_BUCK3CTRL, 0x4A);

	/* set VDD_SNVS_0V8 from default 0.85V */
	pmic_i2c_reg_write(p, PCA9450_LDO2CTRL, 0xC0);

#ifndef CONFIG_IMX8M_LPDDR4
	/* increase NVCC_DRAM_1V2 to 1.2v for DDR4 */
	pmic_i2c_reg_write(p, PCA9450_BUCK6OUT, 0x18);
#endif

	/* set WDOG_B_CFG to 10b=Cold Reset, except LDO1/2 */
	pmic_i2c_reg_write(p, PCA9450_RESET_CTRL, 0xA1);
}

int board_late_init(void)
{
	pmic_i2c_init();
#ifdef CONFIG_ENV_IS_IN_MMC
	board_late_mmc_env_init();
#endif

	return 0;
}
