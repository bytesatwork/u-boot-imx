#!/bin/bash

# interactive
docker run --rm -it -v "$(pwd)":/workdir/u-boot-imx ghcr.io/bytesatwork/uboot-imx-container:lf-5.10.72_2.2.0
