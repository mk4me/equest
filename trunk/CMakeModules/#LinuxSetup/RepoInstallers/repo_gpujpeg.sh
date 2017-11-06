mkdir ~/programming/OpenSource/gpujpeg
mkdir ~/programming/OpenSource/gpujpeg/source
mkdir ~/programming/OpenSource/gpujpeg/_out
git clone https://github.com/hoopoe/gpujpeg.git ~/programming/OpenSource/gpujpeg/source
cd ~/programming/OpenSource/gpujpeg/_out

cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX:PATH="~/programming/OpenSource/gpujpeg/_install" "~/programming/OpenSource/gpujpeg/source"
make
make install

rm -rf ~/programming/Work/SAVA/SourceCode/trunk/include/libgpujpeg/libgpujpeg/
rm -rf ~/programming/Work/SAVA/SourceCode/trunk/lib/linux64/release/libgpujpeg/
rm -rf ~/programming/Work/SAVA/SourceCode/trunk/lib/linux64/debug/libgpujpeg/


mkdir ~/programming/Work/SAVA/SourceCode/trunk/include/libgpujpeg/
ln -s ~/programming/OpenSource/gpujpeg/_install/libgpujpeg/ ~/programming/Work/SAVA/SourceCode/trunk/include/libgpujpeg/libgpujpeg/
ln -s ~/programming/OpenSource/gpujpeg/_install/lib/ ~/programming/Work/SAVA/SourceCode/trunk/lib/linux64/release/libgpujpeg
