# przygotowanie do szukania
FIND_INIT(TBB tbb)

# szukanie
FIND_SHARED(TBB_CORE "<lib,?>tbb<_debug,?>" "<lib,?>tbb<_debug,?>")
FIND_SHARED(TBB_MALLOC "<lib,?>tbbmalloc<_debug,?>" "<lib,?>tbbmalloc<_debug,?>")
FIND_SHARED(TBB_MALLOC_PROXY "<lib,?>tbbmalloc_proxy<_debug,?>" "<lib,?>tbbmalloc_proxy<_debug,?>")
# koniec
FIND_FINISH(TBB)