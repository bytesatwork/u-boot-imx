// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2024 bytesatwork AG - https://www.bytesatwork.io
 */

#include <command.h>
#include <init.h>

static int do_cpuinfo(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	return print_cpuinfo();
}

U_BOOT_CMD(cpuinfo, 1, 1, do_cpuinfo, "show cpu info", "");
