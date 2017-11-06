localStorageChangerd.exe -h
pause

echo ---------------------
localStorageChangerd.exe --storage "%USERPROFILE%/AppData/Roaming/PJWSTK/MDE/db/localStorage.db" -p "P,j.W/s<T>k2:0\"1;2" --list
pause

echo ---------------------
localStorageChangerd.exe --storage "%USERPROFILE%/AppData/Roaming/PJWSTK/MDE/db/localStorage.db" -p "P,j.W/s<T>k2:0\"1;2" --extract "2011-02-24-B0001-S01-T01.56339527.avi" -e "2011-02-24-B0001-S01-T01.amc"
pause

echo ---------------------
localStorageChangerd.exe --storage "%USERPROFILE%/AppData/Roaming/PJWSTK/MDE/db/localStorage.db" -p "P,j.W/s<T>k2:0\"1;2" --update "2011-02-24-B0001-S01-T01.amc|2011-02-24-B0001-S01-T01.zmieniony"
pause