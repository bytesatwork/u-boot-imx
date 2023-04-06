# Copyright (C) 2021 bytes at work AG All Rights Reserved.
#
# Based on: imx8mm_evk/Makefile
# Copyright 2018 NXP
#
# SPDX-License-Identifier:      GPL-2.0+
#

obj-y += bytedevkit.o

ifdef CONFIG_SPL_BUILD
obj-y += spl.o
obj-$(CONFIG_IMX8M_LPDDR4) += lpddr4_timing_512_1024_1536_2048.o
obj-$(CONFIG_IMX8M_LPDDR4) += lpddr4_timing_3072_4096.o
endif

obj-y += ../baw_config/
