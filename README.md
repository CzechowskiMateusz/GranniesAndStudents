# GranniesAndStudents
Sprawozdanie przygotowane spejclanie do rozwiazania projektu: Stowarzyszenie babci i wnusi. 

Przedmiot: Przetwarzanie Rozproszone

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

## Opis procesu
Wątek główny służy za zarządzanie poszczególnymi procesami w obrębie procesów babć oraz studentek. Odwołuje się do zainicjalizowanej w pętli głównej, w której przydzielone wcześniej wątki, posiadają własną logikę w zależności od przydziału roli. Babcie, jeżeli są chętne, zgłaszają się w kolejce po pusty słoik, a następnie przygotowują konfiturę. Studentki, z kolei informują nas o tym, czy są głodne, jeżeli tak i istnieje gotowa konfitura, to mogą się nią poczęstować. Ostatecznie studentka po zjedzeniu konfitury, zwalnia zasoby dotyczące konfitur oraz słoików. 

Wątek komunikacyjny zajmuje się obsługa otrzymywanych komunikatów między procesami. Struktura wiadomości (zawiera: nadawcę, wartość zegara oraz tag) określa nam bardziej dokładny pogląd na oczekiwania procesów względem zasobów oraz umożliwiają wdrożenie algorytmu Zegarów Lamporta. 

## Opis struktur i zmiennych 
- JarQueue - kolejka procesów oczekujących na dostęp do zasobu: Słoiki, [<Queue> początkowo pusta]
- JamQueue - kolejka procesów oczekujących na dostęp do zasobu: Konfitura, [<Queue> początkowo pusta]
- JarAckNum - liczba otrzymanych potwierdzeń, w przypadku zasobu typu Słoik, [Int, początkowo 0]
- JamAckNum - liczba otrzymanych potwierdzeń, w przypadku zasobu typu Konfitura, [Int, początkowo 0] 
- B - liczba procesów Babć
- S - liczba procesów Studentek
- P - liczba zasobów słoików
- K - liczba zasobów konfitur

## Dostępne stany procesów
- INACTIVE - początkowy stan procesu, nie ubiega się o dostęp do sekcji krytycznej
- WAIT - proces chce skorzystać z zasobu, czeka na dostęp do sekcji krytycznej
- INSECTION - proces znajduje sie w sekcji krytycznej

## Pseudokod
```
Proces Babci
[Bi]:
  state := INACTIVE
  while true:
    if wants_to_make_jam: 
      state := WAIT
      priority := clock+1
      ack := 0
      sentToAll({REQ_JAR, priority})

      while (B-1)-ack < P:
        if recived({ACK_JAR}):
          ack := ack+1

      state := INSECTION
      make_jam()
      sendToStudent({NEW_JAM})
      state := INACTIVE

[Bj]:
  if recived({REQ_JAR, priority_i}):
    if not request_jar:
      sendTo(Bi, {ACK_JAR})
    else
      if priority_i < own_priority:
        sendTo(Bi, {ACK_JAR})
      else
        buff := buff+Bi

```

## Wygląd komunikatów
{id_nadawca, zegar, tag}

tagi -> REQ_jar, ACK_jar, NEW_jam, REQ_jam, ACK_jam, FRE_jar

