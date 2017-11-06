# przygotowanie do szukania
FIND_INIT(SQLITE sqlite3)

# szukanie
FIND_SHARED(SQLITE "<lib,?>sqlite3" "<lib,?>sqlite3")

# skopiowanie
FIND_FINISH(SQLITE)
