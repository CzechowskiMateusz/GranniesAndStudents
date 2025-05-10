# GranniesAndStudents
Sprawozdanie przygotowane spejclanie do rozwiazania projektu: Stowarzyszenie babci i wnusi

### Autorzy:
- Wiktoria Białasik 
- Mateusz Czechowski 

## Treść zadania
Grupa emerytek bardzo chciałaby robić konfitury dla wnuczek, tylko że niestety wnuczek nie mają. Na szczęście grupa studentek Uniwersytetu Artystycznego chciałaby, by ktoś je dokarmiał - więc wspólnie emerytki i studentki założyły stowarzyszenie, gdzie babcie tworzą konfitury, a studentki je zjadają, po czym oddają słoiki emerytkom. 

Procesy: B babć i S studentek 
Zasoby: P słoików, K konfitur - ale P i K to wartości maksymalne 
Wymogi: Babcie ubiegają się o słoiki. Gdy mają słoik, tworzą konfiturę. Studentki ubiegają się o konfitury. Gdy dostaną konfiturę, zjadają ją i oddają słoik. 

Procesy działają z różną prędkością, mogą wręcz przez jakiś czas nie chcieć robić albo jeść konfitur. Nie powinno to blokować pracy innych procesów.

## Opis problemu
Kod programu rozdzielono na cztery pliki; główny (main), dwa pliki dotyczące wątków oraz plik zawierający funkcje pomocnicze. Strukturę logiczną podzielono na dwa rodzaje wątków; główny i komunikacyjny. Dane wejściowe to B procesów babć, S procesów studentek oraz dwa rodzaje zasobów nierozróżnialnych; puste słoiki P oraz słoiki z konfiturą K. Każdy z procesów zajmuje się (jest w sekcji krytycznej) danym zasobem pseudolosową ilość czasu - ta wartość jest nieznana dla reszty procesów.

Wątek główny służy za zarządzanie poszczególnymi procesami w obrębie procesów babć oraz studentek. Odwołuje się do zainicjalizowanej w pętli głównej, w której przydzielone wcześniej wątki, posiadają własną logikę w zależności od przydziału roli. Babcie, jeżeli są chętne, zgłaszają się w kolejce po pusty słoik, a następnie przygotowują konfiturę. Studentki, z kolei informują nas o tym, czy są głodne, jeżeli tak i istnieje gotowa konfitura, to mogą się nią poczęstować. Ostatecznie studentka po zjedzeniu konfitury, zwalnia zasoby dotyczące konfitur oraz słoików. 

Wątek komunikacyjny zajmuje się obsługa otrzymywanych komunikatów między procesami. Struktura wiadomości (zawiera: nadawcę, wartość zegara oraz tag) określa nam bardziej dokładny pogląd na oczekiwania procesów względem zasobów oraz umożliwiają wdrożenie algorytmu Zegarów Lamporta. 

## Pseudokod

## Wygląd komunikatów
{nadawca, zegar, tag}

## Rozwiązanie dostępu do sekcji krytycznych
