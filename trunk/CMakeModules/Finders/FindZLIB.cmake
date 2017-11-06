# przygotowanie do szukania
FIND_INIT(ZLIB zlib)
# szukanie
FIND_SHARED(ZLIB "z<lib,?>" "zlib<lib,?>")
# skopiowanie
FIND_FINISH(ZLIB)

