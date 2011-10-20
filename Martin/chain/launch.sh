#!/bin/sh


BUILDROOT="buildroot-2011.08"

sudo apt-get install build-essential bison flex gettext unzip

wget "http://buildroot.uclibc.org/downloads/$BUILDROOT.tar.gz" -O $BUILDROOT.tar.gz
tar -xf $BUILDROOT.tar.gz

cp buildroot.config $BUILDROOT/.config

mkdir dl
ln -s ../dl $BUILDROOT/dl
cp minigui-0.1.tar.gz dl/
cp -r minigui $BUILDROOT/package/
patch $BUILDROOT/package/Config.in package-config.diff
patch $BUILDROOT/boot/uboot/uboot.mk uboot.mk.diff

cd $BUILDROOT

ln -s /usr/bin/minigui fs/skeleton/etc/init.d/S99minigui

make

exit 0
cp output/images/uImage .
cp output/images/u-boot.bin .

exit 0
cp -r home buildroot-2011.08/package/customize/source/
