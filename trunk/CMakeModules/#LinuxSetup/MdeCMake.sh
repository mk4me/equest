mkdir -p ~/programming/Work/MDE/_out/edrutils
chmod +x ./edrutils_install.sh
cp ./edrutils_install.sh ~/programming/Work/MDE/_out/edrutils/edrutils_install.sh
cd ~/programming/Work/MDE/_out/edrutils
cmake -G "Eclipse CDT4 - Unix Makefiles" -DCMAKE_INSTALL_PREFIX:PATH="~/programming/Work/MDE/MDE/trunk/" -DCONFIG_CREATE_INSTALLATION:BOOL=ON -DCONFIG_CREATE_INSTALLERS:BOOL=ON -DCONFIG_GENERATE_CODEBLOCKS_STARTER:BOOL=ON -DCONFIG_GENERATE_FINDERS:BOOL=ON -DCONFIG_GENERATE_STARTER_SCRIPTS:BOOL=ON -DCONFIG_USE_OLD_ABI:BOOL=ON "~/programming/Work/MDE/MDE/trunk/src/edrutils/"
make
./edrutils_install.sh

mkdir -p ~/programming/Work/MDE/_out/edr
cd ~/programming/Work/MDE/_out/edr
cmake -G "Eclipse CDT4 - Unix Makefiles" -DCONFIG_GENERATE_CODEBLOCKS_STARTER:BOOL=ON -DCONFIG_GENERATE_STARTER_SCRIPTS:BOOL=ON -DCONFIG_USE_OLD_ABI:BOOL=ON "~/programming/Work/MDE/MDE/trunk/src/edr/"
./MDE_make.sh
