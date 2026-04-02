# Copyright (C) 2025 bytesatwork AG - https://www.bytesatwork.io
#
# Based on board/freescale/imx93_evk/Makefile
# Copyright 2022 NXP
#
# SPDX-License-Identifier:      GPL-2.0+
#

obj-y += imx93_bytedevkit.o
obj-y += ../../freescale/common/

ifdef CONFIG_SPL_BUILD
obj-y += spl.o
obj-$(CONFIG_IMX93_BYTEDEVKIT_LPDDR4X) += lpddr4x_timing.o lpddr4x_timing_1866mts.o
obj-$(CONFIG_IMX93_BYTEDEVKIT_LPDDR4X_2GB) += lpddr4x_timing_2gb.o
obj-$(CONFIG_IMX93_BYTEDEVKIT_LPDDR4) += lpddr4_timing_2400mts.o
endif

obj-y += ../baw_config/
