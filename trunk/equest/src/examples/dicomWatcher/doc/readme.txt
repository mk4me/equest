Dicom watcher, wersja 0.14, 22.01.2016

1.
Program obserwuje podany folder. Jeśli trafi tam spakowany plik zawierający strukturę DICOM, to zostanie on rozpakowany, skonwertowany i umieszczony w folderze wynikowym.

2.
Wywołanie:
	dicomWatcher [opcje] folderObserwowany folderWyjściowy
Dostępne opcje:
  -h, --help                Wyświetla pomoc
  -v, --version             Wyświetla info o wersji
  -s, --status-file <file>  Plik statusu. Przechowuje on informacje o już przetworzonych plikach. Unika się wtedy wielokrotnego przetwarzania tych samych danych.
  -p, --print-on-console    Program będzie logował wszystkie operacje na konsoli. 
  -l, --log-file <file>     Informacje o wszystkich wykonanych operacjach trafią do pliku 
  -r --run-once				Aplikacja tylko raz czeka na dane wejściowe, po ich przetworzeniu kończy działanie zwracając wartość 0 gdy wszystko przebiegło bez problemów, 1 gdy wysąpił jakiś błąd
  -f --single-folder		Dane wyjściowe trafią do pojedynczego foldera (folderWyjściowy)

3.
Przykład użycia:
	dicomWatcher -s E:\watcher\status.sw -p -l E:\watcher\log.txt E:\watcher\input E:\watcher\output
	
4.
Linux wymaga podania ścieżki do pliku ze słownikiem dicom. Można to osiągnąć wpisując w konsoli lub skrypcie:
	export DCMDICTPATH=/sciezka_do/dicom.dic
	
	
zmiany:
-v 0.14
Dodano oczekiwanie aż zakończy się tworzenie pliku, dopiero potem zip będzie rozpakowany.