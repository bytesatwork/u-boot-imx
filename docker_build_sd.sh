#!/bin/bash

# SD
docker run --rm -v "$(pwd)":/workdir/u-boot-imx ghcr.io/bytesatwork/uboot-imx-container:lf-5.10.72_2.2.0 /workdir/build.sh -t sd -c bytedevkit_defconfig
