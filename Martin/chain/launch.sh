#!/bin/sh


BUILDROOT="buildroot-2011.08"

sudo apt-get install build-essential

wget "http://buildroot.uclibc.org/downloads/$BUILDROOT.tar.gz" -O $BUILDROOT.tar.gz
tar -xf $BUILDROOT.tar.gz

cp buildroot.config $BUILDROOT/.config
cp -r dl $BUILDROOT/
cp -r minigui $BUILDROOT/package/
patch $BUILDROOT/package/Config.in package-config.diff

cd $BUILDROOT

ln -s /usr/bin/minigui fs/skeleton/etc/init.d/S99minigui

make

exit 0
cp package/Config.in buildroot-2011.08/package/Config.in
cp -r home buildroot-2011.08/package/customize/source/

echo "===== Lancement de la construction du buildroot ====="
cd buildroot-2011.08
make
cp buildroot-2011.08/output/images/uImage .
cp buildroot-2011.08/output/images/u-boot.bin .

if_error () {
	if [ $? -ne 0 ]; then
		echo "Error: $1"
		exit 1
	fi;
}
