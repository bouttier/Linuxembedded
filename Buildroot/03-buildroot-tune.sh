echo "===== Configuration de buildroot ====="
cp buildroot-config buildroot-2011.08/.config
cp package/ipshow buildroot-2011.08/package/ -r
cp package/Config.in buildroot-2011.08/package/Config.in
mkdir buildroot-2011.08/dl
cp ipshow-1.0.tar.gz buildroot-2011.08/dl/
cp -r home buildroot-2011.08/package/customize/source/
cp S50ipshow buildroot-2011.08/fs/skeleton/etc/init.d/
