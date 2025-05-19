# GranniesAndStudents
Sprawozdanie przygotowane spejclanie do rozwiazania projektu: Stowarzyszenie babci i wnusi. 

Przedmiot: Przetwarzanie Rozproszone

### Autorzy:
- Wiktoria Białasik 151436
- Mateusz Czechowski 155921

## Treść zadania
Grupa emerytek bardzo chciałaby robić konfitury dla wnuczek, tylko że niestety wnuczek nie mają. Na szczęście grupa studentek Uniwersytetu Artystycznego chciałaby, by ktoś je dokarmiał - więc wspólnie emerytki i studentki założyły stowarzyszenie, gdzie babcie tworzą konfitury, a studentki je zjadają, po czym oddają słoiki emerytkom. 

Procesy: B babć i S studentek 
Zasoby: P słoików, K konfitur - ale P i K to wartości maksymalne 
Wymogi: Babcie ubiegają się o słoiki. Gdy mają słoik, tworzą konfiturę. Studentki ubiegają się o konfitury. Gdy dostaną konfiturę, zjadają ją i oddają słoik. 

Procesy działają z różną prędkością, mogą wręcz przez jakiś czas nie chcieć robić albo jeść konfitur. Nie powinno to blokować pracy innych procesów.

## Opis problemu
Kod programu został podzielony na cztery pliki: główny plik main, dwa pliki zawierające logikę poszczególnych wątków oraz jeden plik pomocniczy, zawierający funkcje wykorzystywane w całym projekcie. Logiczna struktura aplikacji opiera się na dwóch rodzajach wątków: głównym oraz komunikacyjnym. Dane wejściowe obejmują B procesów reprezentujących babcie, S procesów reprezentujących studentki oraz dwa typy zasobów, które są nierozróżnialne z punktu widzenia systemu: puste słoiki (P) oraz słoiki z konfiturą (K). Każdy z procesów wchodzi do sekcji krytycznej związanej z zasobem na pseudolosowy czas, który nie jest znany pozostałym procesom, co odwzorowuje asynchroniczność działania systemu rozproszonego.

## Opis procesu
W pliku main uruchamiany jest wątek główny, który odpowiada za inicjalizację oraz zarządzanie wszystkimi procesami — zarówno babć, jak i studentek. 
Wątek główny służy za zarządzanie poszczególnymi procesami w obrębie procesów babć oraz studentek. Odwołuje się do zainicjalizowanej w pętli głównej, w której przydzielone wcześniej wątki, posiadają własną logikę w zależności od przydziału roli. Babcie, jeżeli są chętne, zgłaszają się w kolejce po pusty słoik, a następnie przygotowują konfiturę. Studentki, z kolei informują nas o tym, czy są głodne, jeżeli tak i istnieje gotowa konfitura, to mogą się nią poczęstować. Ostatecznie studentka po zjedzeniu konfitury, zwalnia zasoby dotyczące konfitur oraz słoików. 

Wątek komunikacyjny odpowiada za obsługę wszystkich wiadomości przesyłanych pomiędzy procesami. Każda wiadomość zawiera identyfikator nadawcy, wartość zegara logicznego Lamporta oraz tag informujący o typie komunikatu — na przykład: żądanie, potwierdzenie lub zwrot zasobu. Dzięki tej strukturze każdy proces jest w stanie określić intencje nadawcy oraz zaktualizować swój stan zgodnie z zaimplementowanym algorytmem zegara Lamporta. Przykładowo, po otrzymaniu żądania procesu o niższym priorytecie (według zegara), proces może natychmiast przesłać potwierdzenie. W przeciwnym wypadku umieszcza nadawcę w buforze oczekujących odpowiedzi. 

Plik z funkcjami pomocniczymi zawiera zbiór funkcji wspomagających działanie wątków oraz używanych do komunikacji.

## Opis struktur i zmiennych 
- JarQueue - kolejka procesów oczekujących na dostęp do zasobu: Słoiki, [<Queue> początkowo pusta]
- JamQueue - kolejka procesów oczekujących na dostęp do zasobu: Konfitura, [<Queue> początkowo pusta]
- JarAckNum - liczba otrzymanych potwierdzeń, w przypadku zasobu typu Słoik, [Int, początkowo 0]
- JamAckNum - liczba otrzymanych potwierdzeń, w przypadku zasobu typu Konfitura, [Int, początkowo 0] 
- B - liczba procesów Babć
- S - liczba procesów Studentek
- P - liczba zasobów słoików
- K - liczba zasobów konfitur
- availableJars - obecna liczba dostępnych słoików, [Int, początkowo P]
- usingJams - obecna liczba używanych konfitur, [Int, początkowo 0]

## Dostępne stany procesów
- INACTIVE - początkowy stan procesu, nie ubiega się o dostęp do sekcji krytycznej
- WAIT - proces chce skorzystać z zasobu, czeka na dostęp do sekcji krytycznej
- INSECTION - proces znajduje sie w sekcji krytycznej

## Pseudokod
Proces Babci
```
[Bi]:
  state := INACTIVE
  while true:
    if wants_to_make_jam: 
      state := WAIT
      priority := clock+1
      ack := 0
      adds to JarQueue
      sentToAll({REQ_JAR, priority})

      while (B-1)-ack < P and availableJars  > 0:
        if recived({ACK_JAR}):
          ack := ack+1

      wait until JarQueue.peak == Bi
      removes from JarQueue
      availableJars := availableJars - 1

      state := INSECTION
      make_jam()
      usingJams := usingJams + 1
      sendToAll({NEW_JAM})
      state := INACTIVE

[Bj]:
  if recived({REQ_JAR, priority_i}):
    if not request_jar:
      sendTo(Bi, {ACK_JAR})
    else
      if priority_i < own_priority:
        sendTo(Bi, {ACK_JAR})
      else
        JarQueue.enqueue(Bi, priority_i)

  if recived({NEW_JAM}):
      availableJars := availableJars - 1
      usingJams := usingJams + 1

  if recived({FRE_RES}):
      usingJams := usingJams - 1
      availableJars := availableJars + 1
```

Proces Studentki
```
[Si]:
  state := INACTIVE
  while true:
    if wants_to_eat_jam: 
      state := WAIT
      priority := clock+1
      ack := 0
      adds to JamQueue
      sentToAll({REQ_JAM, priority})

      while (S-1)-ack < K and usingJam < K:
        if recived({ACK_JAM}):
          ack := ack+1

      wait until JamQueue.peak() == Si
      removes from JamQueue
      usingJams := usingJams - 1

      state := INSECTION
      eat_jam()
      availableJars := availableJars + 1
      sendToAll({FRE_RES})
      state := INACTIVE

[Sj]:
  if recived({REQ_JAM, priority_i}):
    if not request_jam:
      sendTo(Si, {ACK_JAM})
    else
      if priority_i < own_priority:
        sendTo(Si, {ACK_JAM})
      else
        JamQueue.enqueue(Si, priority_i)

  if recived({NEW_JAM}):
      availableJars := availableJars - 1
      usingJams := usingJams + 1

  if recived({FRE_RES}):
      usingJams := usingJams - 1
      availableJars := availableJars + 1
```

## Wygląd komunikatów
Wszystkie komunikaty zawierają znacznik czasowy (timestamp), modyfikowany zgodnie z zasadami zegara logicznego Lamporta, id nadawcy oraz tag określający rodzaj komunikatu:
- REQ_JAR - żądanie o dostęp do sekcji krytycznej dla zasobu typu słoik,
- ACK_JAR - potwierdzenie dostępu do sekcji krytycznej dla zasobu typu słoik,
- REQ_JAM - żądanie o dostęp do sekcji krytycznej dla zasobu typu konfitura,
- ACK_JAM - potwierdzenie dostępu do sekcji krytycznej dla zasobu typu konfitura,
- NEW_JAM - informacja o wyprodukowaniu zasobu typu konfitura przez proces babci,
- FRE_RES - zwolnienie zasobów. 

