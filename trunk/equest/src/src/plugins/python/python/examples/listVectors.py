
#Skrypt tworzy listê wszystkich kolekcji wektorów, które zosta³y za³adowane do aplikacji
import plugin_simplePythond
# pobieranie tablic wraz z opisem 
v = mde.listLoadedVectors()
# konwersja do listy
dataList = [(x.first, x.second) for x in v]
# wypisanie wyników na konsolê
print dataList



