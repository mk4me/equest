#Skrypt wczytuje pierwsz¹ za³adowan¹ tablicê wektorów
import plugin_python
import sys
sys.path += ["C:\\Users\\Wojciech\\Anaconda\\Lib\\site-packages"]

import matplotlib.pyplot as plt

#listowanie za³adowanych danych
v = mde.listLoadedVectors()
if len(v)>1:
	#pobieramy pierwsz¹ tablicê
	vc = mde.getVectorChannel(v[0].first, v[0].second)
	data = vc.getData();
	#wype³nianie wykresu danymi
	plt.plot([v.x() for v in data])
	plt.ylabel('some numbers')
	#rysowanie wykresu
	plt.show()
else:
	print "no data loaded"






