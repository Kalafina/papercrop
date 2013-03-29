all: release

# to compile and install paperCrop to ~/bin/
# make install_dependencies_ubuntu11.10; make; make console; make install
SOURCE_DIR :=~/bin/papercrop/paperCrop_source
.PHONY: release debug clean console
release:
	cd libharu-2.0.8;chmod 700 ./configure;./configure; make
	mkdir -p build_linux;cd build_linux;cmake ..;make;cp paperCrop ..

debug:
	cd libharu-2.0.8;chmod 700 ./configure;./configure; make
	cd build_linux_debug: cmake -D "CMAKE_BUILD_TYPE=Debug" ..; make; cp paperCrop ..

console: 
	cd libharu-2.0.8;chmod 700 ./configure;./configure; make
	mkdir -p build_linux_console;cd build_linux_console;cmake -D "CONSOLE=True" ..;make;cp paperCrop ../paperCrop_console

clean:
	rm libharu-2.0.8/libhpdf.a
	rm -rf build_linux
	rm -rf build_linux_debug
	rm -rf build_linux_console


install_dependencies_ubuntu11.10:
	sudo apt-get install libpoppler-dev libdevil-dev libfontconfig-dev libfltk1.1-dev libdevil-dev libfreeimage-dev liblua5.1-dev libpng12-dev lua5.1 cmake g++ libxml2-dev libgsl0-dev 
	sudo apt-get install openjdk-7-jre

install_dependencies_fedora:
	sudo yum install DevIL-devel fltk-devel fltk-static fontconfig-devel freeimage-devel lua-devel libpng-devel libpng10-devel boost-devel libxml2-devel ogre-devel gsl-devel luabind-devel fltk-fluid fltk lcms-devel lcms-libs poppler-devel

install: 
	if test -d $(SOURCE_DIR); then echo "Test passed"; else mkdir -p ~/bin/papercrop; fi
	cp -rf . $(SOURCE_DIR)
	cp paperCrop ~/bin/papercrop/paperCrop
	cp paperCrop $(SOURCE_DIR)
	cp paperCrop.py ~/bin/paperCrop
	cp paperCrop_auto.py ~/bin/paperCropAuto
	chmod 700 ~/bin/paperCrop
	chmod 700 ~/bin/paperCropAuto
	if test -f paperCrop_console; then cp paperCrop_console ~/bin/papercrop/paperCrop_console; fi
