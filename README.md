# shipsGame Project by Anna Giszczak i Barbara Chytła
There is game in C with socket programming (server-client).

PL:
Przedstawiamy program pisany w języku C. Jest to gra w statki, która składa się z
dwóch plików server.c i client.c. Odbywa się za pomocą połączenia TCP.

## Zasady:
Użytkownik ma do dyspozycji 3 statki: trójmasztowiec, dwumasztowiec i pojedynczy
masztowiec. Rozmieszcza je po mapie. Następnie przechodzi do rozgrywki, która odbywa
się w formie tur- raz gracz 1 zgaduje koordynaty statku gracza 2, gdy trafi ponawia próbę
zgadywania, gdy nie to kolej przeciwnika. Gra dobiega końca wraz z zatopieniem ostat-
niego statku. Wtedy wyłaniany jest zwycięzca, który pierwszy zatopił statki przeciwnika.

## Przebieg:
Serwer jest włączony. Klient 1 dołącza się, a serwer czeka na drugiego klienta. Gdy
drugi klient podłącza się- gra się zaczyna. Pierwszy klient wprowadza swoje koordynaty
statku, drugi w tym czasie czeka na swoją kolej. Gdy kolej drugiego klienta, wprowadza
on swoje koordynaty i gra zaczyna się od tury dla pierwszego klienta. Pierwszy klient
wprowadza koordynaty statku przeciwnika, oczywiście te gdzię myśli, że statek przeciw-
nika się znajduje. Dostaje od servera odpowiedź w postaci mapy strzałów i informację o
trafieniu. Gdy celnie zgadł to może powtórzyć ruch, a jeżeli nie to przeciwnik wykonuje
swój ruch- również zgaduje koordynaty, dostaje informacje o celnym, bądź nietrafionym
ruchu oraz mapę tego i swoich poprzednich strzałów. Rozgrywka toczy się w ten sposób,
aż do zatopienia statków u jakiegoś przeciwnika. Gdy zatopione są wszystkie statki użyt-
kownika to jego przeciwnik wygrywa. Przez cały czas rozgrywki w terminalu, w którym
mieści się serwer wyświetlane są informacje o stanie gry.

POmoc w stworzeniu serwera umieszczony na poniższej stronie:
[tutaj]{https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-
without-multi-threading/}
Plik klienta został stworzony na bazie pliku TCP serwera z zajęć laboratoryjnych.
