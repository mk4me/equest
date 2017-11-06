mkdir ~/programming/OpenSource/boost
#mkdir ~/programming/OpenSource/boost/source

#kompilacja z repo wymaga wiecej krokow
#git clone https://github.com/boostorg/boost.git ~/programming/OpenSource/boost/source

#zadowolimy sie tym:
BOOST_VER="boost_1_62_0"
BOOST_VER2="1.62.0"
   
cd ~/programming/OpenSource/boost
if [ ! -f ./$BOOST_VER.tar.gz ]; then
    wget http://downloads.sourceforge.net/project/boost/boost/$BOOST_VER2/$BOOST_VER.tar.gz
fi
#

tar -xzf $BOOST_VER.tar.gz
mv $BOOST_VER source
cd ~/programming/OpenSource/boost/source
sh ./bootstrap.sh
./b2 install --prefix=../_out
cd ~/programming/OpenSource/boost/_out/bin
./b2

rm -rf ~/programming/Work/SAVA/SourceCode/trunk/include/boost/boost
rm -rf ~/programming/Work/SAVA/SourceCode/trunk/lib/linux64/release/boost/
rm -rf ~/programming/Work/SAVA/SourceCode/trunk/lib/linux64/debug/boost/

ln -s ~/programming/OpenSource/boost/_out/include/boost ~/programming/Work/SAVA/SourceCode/trunk/include/boost/boost
ln -s ~/programming/OpenSource/boost/_out/lib/ ~/programming/Work/SAVA/SourceCode/trunk/lib/linux64/release/boost



