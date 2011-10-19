cp buildroot-config buildroot-2011.08/.config
cp package/ipshow buildroot-2011.08/package/ -r
cat package/Config.in >> buildroot-2011.08/package/Config.in
mkdir buildroot-2011.08/dl
cp ipshow-1.0.tar.gz buildroot-2011.08/dl/
