#skrypt tworzy dane wektorowe kompatybilne z aplikacją i wrzuca je do drzewa analiz

# wczytujemy pluginy (drugi parametr oznacza, ze z obiektow mozna korzystac bez koniecznosci 
# stosowania namespace'a pluginu (np. v3vector zamiast plugin_mdePython.v3vector)
mdeimport('plugin_mdePython', 1)
mdeimport('plugin_communicationPython', 1)
from math import sin
import random

'''# tworzymy obiekt dzieki ktoremu mozna korzystac z pluginu communication
comm = CommunicationPython()

# jesli nie ma kontekstu to oznacza, ze nikt sie nie zalogowal do tej pory
if comm.shallowContextsCount() is 0:
	# wczytujemy konfiguracje 
	config = comm.loadConfig('C:\Users\Wojtek\AppData\Roaming\PJATK\MDE\mde_communication.ini')
	# logujemy sie 
	comm.login(config)

# dla pierwszego zalogowanego uzytkownika ladujemy dostepne triale
context = comm.getContext(0)
for s in context.listTrialsToLoad():
	print s
	context.loadTrial(s)
	'''
	
#tworzymy tablicę wektorów
v = scalarVector()

# wypełniamy wektor danymi
#  2000 próbek - funkcja trygonometryczna
for x in range(1,2000):
	v.append(3*sin(x/200.0) + random.random() - 0.5)
	
mde = MdeBridge()
#tworzymy DataChannel...
vc = mde.createScalarChannel()
#... wypełniamy danymi ...
vc.setData(v)
#... ustalamy próbkowanie ...
vc.setFrequency(100)
#... i nazwę
vc.setName("Test1")
# utworzenie wpisu w drzewie analiz
mde.addScalarChannel(vc)

#wczytujemy dostepne helpery (mozliwe warianty wizualizacji danych)
helpers = mde.getHelpers()
mde.log('number of helpers - ' + str(len(helpers)))
# dla kazdego tworzymy wizualizator
for h in helpers:
	n = h.name();
	mde.log(n)
	print n
	#h.createVisualizer()
#mde.close(1)