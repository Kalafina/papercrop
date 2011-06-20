sudo apt-get install subversion
echo 'svn checking out....(wait)'
svn checkout http://papercrop.googlecode.com/svn/trunk/ paperCrop_source
cd paperCrop_source
echo 'svn update .'
svn update .
sudo apt-get install libdevil-dev libfontconfig-dev libfltk1.1-dev libdevil-dev libfreeimage-dev liblua5.1-dev libpng12-dev libboost1.42-dev lua5.1 cmake g++ libxml2-dev libogre-dev libgsl0-dev libluabind-dev
echo 'start building paperCrop'
sh clean_build.sh
cd ..
echo "cd paperCrop_source" > paperCropLauncher
echo "./paperCrop" >> paperCropLauncher
chmod 700 paperCropLauncher
./paperCropLauncher
