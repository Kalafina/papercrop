
cd libharu-2.0.8
chmod 700 ./configure
./configure
make -j 4
cd ..

cd poppler
./autogen.sh
cd ..

mkdir -p build_linux
cd build_linux
cmake ..
make -j 4
cp paperCrop ..

