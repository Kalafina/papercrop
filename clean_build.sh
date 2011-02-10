
cd libharu-2.0.8
chmod 700 ./configure
./configure
make
cd ..
cd build_linux
rm CMakeCache.txt
cmake ..
make clean
make
cp paperCrop ..
