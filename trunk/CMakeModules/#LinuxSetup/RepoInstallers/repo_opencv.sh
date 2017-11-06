mkdir ~/programming/OpenSource/opencv
mkdir ~/programming/OpenSource/opencv/source
mkdir ~/programming/OpenSource/opencv/source_contrib
git clone https://github.com/opencv/opencv ~/programming/OpenSource/opencv/source
git clone https://github.com/opencv/opencv_contrib ~/programming/OpenSource/opencv/source_contrib

cd ~/programming/OpenSource/opencv/_out

cmake -G "Unix Makefiles" -DOPENCV_EXTRA_MODULES_PATH=~/programming/OpenSource/opencv/source_contrib/modules -DCMAKE_INSTALL_PREFIX:PATH="~/programming/OpenSource/opencv/_install" "~/programming/OpenSource/opencv/source" 
make
make install

rm -rf ~/programming/Work/SAVA/SourceCode/trunk/include/opencv/
rm -rf ~/programming/Work/SAVA/SourceCode/trunk/lib/linux64/release/opencv/
rm -rf ~/programming/Work/SAVA/SourceCode/trunk/lib/linux64/debug/opencv/

ln -s ~/programming/OpenSource/opencv/_install/include/ ~/programming/Work/SAVA/SourceCode/trunk/include/opencv/
ln -s ~/programming/OpenSource/opencv/_install/lib/ ~/programming/Work/SAVA/SourceCode/trunk/lib/linux64/release/opencv/

