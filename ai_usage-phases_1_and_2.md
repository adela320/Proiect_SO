# Utilizarea AI în Fazele 1 & 2


## Faza 1

Pentru detalii privind modul în care am utilizat & corectat codul AI din prima fază a proiectului, citiți fișierul `ai_usage.md`.

Pe scurt, AI-ul a generat structura inițială pentru funcțiile de filtrare (`parse_condition()`, `match_condition()`). Codul a fost revizuit și îmbunătățit manual cu protecții buffer overflow, comparații adaptate la tipul datelor și integrare corectă în sistemul de filtrare.


## Faza 2

### Notificarea prin SIGUSR1

Am utilizat AI pentru a înțelege implementarea corectă a `sigaction()`, deoarece cerința interzicea utilizarea funcției mai simple `signal()`.
- **De ce**: `sigaction` este mai robustă și standard în programarea de sistem modernă.

### Gestiunea Proceselor

Pentru implementarea acestei comenzi, am ales să folosesc un proces copil pentru a rula comanda de sistem `rm`. Deși înțelegeam conceptul teoretic, am utilizat AI-ul pentru a clarifica mecanismul de sincronizare dintre părinte și copil:
- **Provocarea**: Utilizarea corectă a funcției `wait()` pentru a mă asigura că părintele șterge link-ul simbolic doar după ce procesul copil a terminat execuția comenzii `rm`.
La propunerea AI-ului, am insistat pe verificări de siguranță. Am adăugat logica de a bloca ștergerea dacă `district_id` este empty, `.` (directorul curent) sau `/` (rădăcina), pentru a preveni execuția accidentală a unei comenzi distructive precum `rm -rf /`.

### Asistență suplimentară

Pe lângă scrierea codului, am utilizat asistența AI pentru:
- **Structurarea fișierelor .md**: Transformarea notițelor într-o documentație lizibilă și bine organizată (README și AI usage).
- **Corectare gramaticală**: Asigurarea unui ton profesional și utilizarea diacriticelor în limba română.

## Concluzie

Utilizarea AI în acest proiect a fost esențială pentru înțelegerea și implementarea corectă a conceptelor avansate de programare de sistem, cum ar fi gestionarea semnalelor cu `sigaction()` și sincronizarea proceselor părinte-copil. AI-ul a servit ca un instrument de învățare și clarificare, permițându-mi să aprofundez aspecte tehnice complexe și să implementez verificări de siguranță critice. În plus, asistența în structurarea documentației a contribuit la crearea unui proiect bine organizat și ușor de înțeles.
