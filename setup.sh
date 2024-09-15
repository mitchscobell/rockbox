mkdir build_dir -p
cd build-dir
../tools/configure
make clean
make
mkdir -p simdisk/.rockbox/rocks/apps/
mkdir -p simdisk/.rockbox/backdrops/
cp ../backdrops/* simdisk/.rockbox/backdrops/ -r
mkdir -p simdisk/.rockbox/icons/
cp ../icons/* simdisk/.rockbox/icons/ -r
cp -r apps/plugins/* simdisk/.rockbox/rocks/apps/
