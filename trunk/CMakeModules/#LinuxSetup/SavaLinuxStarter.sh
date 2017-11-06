# Skrypt instaluje wszystkie potrzebne rzeczy do skompilowania i uruchomienia sava-linux na Linuxie
# należy skopiować skrypty SavaLinuxStarter.sh i StarterApps.sh do folderu domowego, nadac prawa do uruchomienia i uruchomić
# chmod +x SavaLinuxStarter.sh
# ./SavaLinuxStarter
# Skrypt zapyta o hasło roota w celu instalacji aplikacji
#
#	Wykonywane operacje:
#	1. Pobranie i zainstalowanie potrzebnych aplikacji (StarterApps.sh)
#	2. Utworzenie struktury katalogów
#	3. Ściągnięcie źródeł z svn
#	4. Ściągnięcie najnowszych bibliotek
#	5. Rozpakowanie ich i utworzenie dowiązań symbolicznych w trunk-u SAVA
# Instalacja przebiega bez ingerencji uzytkownika, nalezy tylko wypelnic zmienne nizej

assembla_user=""
assembla_pass=""


ftp_adress="83.230.112.91/lib/SAVA/"
ftp_user=""
ftp_pass=""

sudo sh ./StarterApps.sh

mkdir ~/programming
mkdir ~/programming/Work
mkdir ~/programming/Work/SAVA
mkdir ~/programming/Work/SAVA/_out
mkdir ~/programming/Work/SAVA/SourceCode
mkdir ~/programming/Work/SAVA/Libs

if [ -z "$assembla_user"] || [ -z "$assembla_pass" ]; then 
svn checkout https://subversion.assembla.com/svn/sava-linux/ ~/programming/Work/SAVA/SourceCode 
else  
svn checkout --non-interactive --no-auth-cache --username $assembla_user --password $assembla_pass https://subversion.assembla.com/svn/sava-linux/ ~/programming/Work/SAVA/SourceCode 
fi

file=`ncftpls -m ftp://$ftp_user:$ftp_pass@$ftp_adress  | grep linux64 | sort -k1 | awk '{ print $2 }' | tail -n1`
echo "~/programming/Work/MDE/Libs/$file"
ncftpget ftp://$ftp_user:$ftp_pass@$ftp_adress$file 
mv $file ~/programming/Work/SAVA/Libs/$file
folder="${file%.*}"
7z x ~/programming/Work/SAVA/Libs/$file -o./programming/Work/SAVA/Libs/$folder/

ln -s ~/programming/Work/SAVA/Libs/$folder/include ~/programming/Work/SAVA/SourceCode/trunk/include
ln -s ~/programming/Work/SAVA/Libs/$folder/lib     ~/programming/Work/SAVA/SourceCode/trunk/lib

sh ./RepoInstallers/install.sh


#sh ./MdeCMake.sh
