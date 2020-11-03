1. Należy pobrać i zainstalować:
	a) ImageMagick: https://imagemagick.org/download/binaries/ImageMagick-7.0.10-14-Q16-x64-static.exe
	b) Ghostscript: https://github.com/ArtifexSoftware/ghostpdl-downloads/releases/download/gs952/gs952w64.exe

	WAŻNE - przy instalacji ImageMagick opcja "Add application directory to your system path" MUSI być zaznaczona.

2. Skrypt extract_jpg.bat skopiować do miejsca, w którym znajduje się plik PDF, z którego chcemy wypakować obrazki.
3. Skrypt można uruchomić na dwa sposoby:
	a) Przeciągnąć ikonkę pliku PDF na ikonkę skryptu.
	b) Poprzez wiersz poleceń - trzymając wciśnięty klawisz SHIFT należy kliknąć prawym przyciskiem myszy w wolnym miejscu w katalogu, a następnie wybrać z menu "Open command prompt window here" lub "Open PowerShell window here"
		Następnie należy wpisać następujące polecenie: 
			.\extract_jpg.bat nazwa_pliku.pdf

4. Skrypt powinien utworzyć w miejscu wywołania katalog nazwa_pliku_output i wypełnić go obrazkami o nazwach nazwa_pliku_n.jpg, gdzie n to kolejne liczby naturalne.


***


Można przy pomocy skryptu ustawić rozmiar wypakowywanych plików. W tym celu należy uruchomić poprzez wiersz poleceń, co opisano w punkcie 3b. Przykład:
	
	.\extract_jpg.bat nazwa_pliku.pdf 1024x768

	Gdzie w miejsce 1024x768 należy wstawić żądaną rozdzielczość. Obrazki nie zostaną rozciągnięte - jeśli proporcje wybranej rozdzielczości i obrazka nie będą jednakowe, aplikacja automatycznie dobierze odpowiedni wymiar poziomy lub pionowy.


Skrypt posiada parametr wewnętrzny default_density równy 300. Można zmienić tę wartość na większą, jeżeli obrazki będą wydawać się zbyt rozmyte.