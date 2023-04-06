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
obj-y += baw_config/baw_config.o
# baw_config_get is mandatory for lpddr4 timing setup
obj-y += baw_config/baw_config_get.o
obj-$(CONFIG_IMX8M_LPDDR4) += lpddr4_timing_512_1024_1536_2048.o
obj-$(CONFIG_IMX8M_LPDDR4) += lpddr4_timing_3072_4096.o
else
ifneq ($(or $(CONFIG_CMD_BAW_CONFIG), $(CONFIG_BAW_CONFIG_BUILTIN), $(CONFIG_BAW_CONFIG_EEPROM)),)
obj-y += baw_config/baw_config.o
endif
endif

obj-$(CONFIG_CMD_BAW_CONFIG) += baw_config/cmd_baw_config.o
obj-$(CONFIG_BAW_CONFIG_EEPROM) += baw_config/baw_config_eeprom.o
obj-$(CONFIG_BAW_CONFIG_BUILTIN) += baw_config/baw_config_builtin.o
