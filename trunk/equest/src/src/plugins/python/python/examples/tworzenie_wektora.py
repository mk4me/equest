#skrypt tworzy dane wektorowe kompatybilne z aplikacj¹ i wrzuca je do drzewa analiz
import plugin_python

#tworzymy tablicê wektorów
v = v3vector()

'''wype³niamy wektor danymi
  2000 próbek
  x - funkcja liniowa
  y - funkcja kwadratowa
  z - funkcja trygonometryczna'''
for x in range(1,2000):
	a = Vec3(x*0.01, x*x*0.001, 3*sin(x/200.0))
	v.append(a)

#tworzymy DataChannel...
vc = mde.createVectorChannel()
#... wype³niamy danymi ...
vc.setData(v)
#... ustalamy próbkowanie ...
vc.setFrequency(100)
#... i nazwê
vc.setName("Test1")
# utworzenie wpisu w drzewie analiz
mde.addVectorChannel(vc)


