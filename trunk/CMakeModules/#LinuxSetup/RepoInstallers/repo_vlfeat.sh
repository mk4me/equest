mkdir ~/programming/OpenSource/vlfeat
mkdir ~/programming/OpenSource/vlfeat/source
git clone git://github.com/vlfeat/vlfeat.git ~/programming/OpenSource/vlfeat/source
cd ~/programming/OpenSource/vlfeat/source
make

rm -rf ~/programming/Work/SAVA/SourceCode/trunk/include/vlfeat/vl/
rm -rf ~/programming/Work/SAVA/SourceCode/trunk/lib/linux64/release/vlfeat/
rm -rf ~/programming/Work/SAVA/SourceCode/trunk/lib/linux64/debug/vlfeat/


mkdir ~/programming/Work/SAVA/SourceCode/trunk/include/vlfeat/
mkdir ~/programming/Work/SAVA/SourceCode/trunk/lib/linux64/release/vlfeat/
ln -s ~/programming/OpenSource/vlfeat/source/vl ~/programming/Work/SAVA/SourceCode/trunk/include/vlfeat/vl
ln -s ~/programming/OpenSource/vlfeat/source/bin/glnxa64/libvl.so ~/programming/Work/SAVA/SourceCode/trunk/lib/linux64/release/vlfeat/libvl.so
