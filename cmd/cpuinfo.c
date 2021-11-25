/*
 * Copyright (C) 2018 bytes at work AG - https://www.bytesatwork.io
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <command.h>

static int do_cpuinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	return print_cpuinfo();
}

U_BOOT_CMD(
	cpuinfo, 2, 1, do_cpuinfo,
	"show cpu info",
	""
);
