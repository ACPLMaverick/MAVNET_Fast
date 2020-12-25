Instalacja

1. Zainstalować najnowszą wersję Pythona 3 w wersji 64-bitowej. Obecnie najwyższa wersja to 3.9.0 i można ją pobrać stąd: https://www.python.org/ftp/python/3.9.0/python-3.9.0-amd64.exe
2. W trakcie instalacji upewnić się, że pola "pip", "tck/tk and IDLE" i "py launcher" są zaznaczone. Upewnić się, że opcja "dodaj Pythona do zmiennej środowiskowej PATH" jest zaznaczona.
3. Po pomyślnej instalacji otworzyć wiersz poleceń - Start -> Wyszukaj -> "Wiersz polecenia" lub "Command Prompt".
4. Upewnić się, że Python został poprawnie zainstalowany wpisując "python --version" (bez cudzysłowia) i wciskając Enter. Powinno wypisać się w okienku "Python 3.9.0" (lub inna wersja, którą zainstalowaliśmy).
4. Wpisać i zatwierdzić kolejno komendy instalujące dodatkowe pakiety Pythona:
    pip install wheel
    pip install py-pptx
    pip install PyMuPDF
5. Jeśli wszystko zostało poprawnie zainstalowane, można uruchomić skrypt "distribute_images.py" klikając dwukrotnie w jego ikonę. Jeśli zamiast tego pojawia się notatnik, można uruchomić go z wiersza poleceń, podając jego pełną ścieżkę z rozszerzeniem, np. "C:\Users\Janusz\Desktop\distribute_images.py" lub za pomocą komendy py, czyli:
    py C:\Users\Janusz\Desktop\distribute_images.py



Działanie programu

W pierwszej kolejności należy załadować plik prezentacji PPTX, którą chcemy edytować przyciskiem Presentation.

Następnie należy wybrać źródło obrazków przyciskiem PDF lub Images.
    W przypadku opcji PDF, program zapyta o zakres obrazków jaki ma wykopiować z danego PDF'a. Obrazki pojawią się w tej samej lokalizacji, w której znajduje się skrypt, w podkatalogu .distribute_images\<nazwa_pdf>. Następnie pojawi się lista, z której możemy wybrać, które z wypakowanych obrazków chcemy załadować do programu. Domyślnie wybrane są wszystkie.
    W przypadku opcji Images należy wybrać katalog, w którym znajdują się obrazki w formatach PNG, JPG, GIF, etc. Dalsza część procesu przebiega podobnie.

Na środku ekranu zobaczymy obrazki w domyślnym ułożeniu. Klikając dwukrotnie w dany obrazek możemy przypisać mu odpowiedni podpis. Podpisy dla wszystkich obrazków można także ustalić klikając w przycisk "Word settings" w prawym dolnym rogu lub klikając w okienku podglądu prawym przyciskiem myszy i wybierając "Word settings".

Dalej omówimy poszczególne opcje i przełączniki programu.

Source slide - tutaj ustawiamy slajd, z którego generator slajdów skopiuje styl graficzny i układ nowego/nowych slajdów. Domyślnie ostatni slajd w prezentacji.
Work slide - opcja "New slide" spowoduje utworzenie nowego slajdu i umieszczenie na nim wybranych obrazków. Opcja "Current slide" zrobi to na obrazku wyznaczonym w opcji "Source slide". W przypadku gdy generator tworzy więcej niż jeden slajd, ta opcja ma zastosowanie tylko dla pierwszego z wygenerowanych slajdów.
Word font - font, którym będą wypisane nazwy obrazków. Póki co jest tylko Impact do wyboru.
Distribution - wybieramy tutaj tryb, w którym ma pracować program.
    No words shuffled - obrazki będą rozmieszczone bez podpisów, losowo przemieszane
    No words - obrazki będą rozmieszczone bez podpisów, kolejno względem ich nazw
    One word per slide - zostanie wygenerowane tyle slajdów, ile jest obrazków. Na każdym znajdować się będą wszystkie obrazki kolejno oraz jedna z nazw obrazków
    All words - obrazki będą rozmieszczone kolejno, każdy z nich będzie mieć poniżej podpis ze swoją nazwą
    Guess what's wrong - tak samo, jak w All words, tyle że część z podpisów będzie losowo przemieszana. To, ile będzie przemieszań, a zarazem błędów, określa parametr "Mistakes". To, ile slajdów z błędami zostanie wygenerowanych, określa opcja "Mistake slides".
    All modes combined - zostaną wygenerowane wszystkie slajdy dla wszystkich trybów po kolei.
Small text - rozmiar tekstu w trybach "All words" i "Guess what's wrong".
Large text - rozmiar tekstu w trybie "One word per slide"
Background color - dodatkowo można tu określić kolor tła generowanych slajdów. Jeśli odznaczymy checkbox, kolor nie będzie brany pod uwagę (zostanie użyte tło ze slajdu określonego w polu "Source slide").
Word position - położenie słowa w trybie "One word per slide".
Font color - kolor napisów.
Uniform width - zależnie od włączenia lub wyłączenia tej opcji można uzyskać różne efekty rozmieszczenia obrazków w poziomie.
Mistake slides - liczba wygenerowanych slajdów w trybie "Guess what's wrong"
Image padding - ilość "marginesu" wolnego miejsca wokół każdego z obrazków, określona w pikselach. Można za pomocą tej opcji także przesuwać obrazki, co daje różne efekty.
Active word - opcja tyczy się tylko trybu "One word per slide". Określa ona, które słowo aktualnie widzimy w okienku podglądu. Nie ma wpływu na wygenerowane slajdy.
Mistakes - liczba błędów na jednym slajdzie w trybie "Guess what's wrong". Musi być ona parzysta, gdyż tworzenie błędów działa na zasadzie zamiany miejscami par podpisów.
Word padding - ilość "marginesu" wolnego miejsca dla tekstu w trybie "One word per slide". Można za pomocą tej opcji także przesuwać tekst, co daje różne efekty.

Po ustawieniu wszystkich opcji tak, jak chcemy, wciskamy przycisk Create. Jeżeli wszystko się powiodło, program wyświetli odpowiednią informację zwrotną. Jeżeli chcemy być ostrożni, warto przed operacją utworzyć kopię zapasową prezentacji - program tego nie zrobi za użytkownika.
