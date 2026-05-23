# Documentație privind utilizarea AI

Această documentație menționează modul în care am utilizat AI pentru fiecare fază în parte. Regăsiți aceleași informații și în documentele dedicate fiecărei faze (`ai_usage.md` - faza 1, `ai_usage-phases_1_and_2.md` - faza 2, `ai-usage-phase-3.md` - faza 3).

## Faza 1

La partea de filtrare din proiect am apelat la un asistent AI pentru a genera structura inițială a două funcții:

- parse_condition()
- match_condition()

Am folosit AI-ul ca punct de plecare, nu ca soluție finală. Codul generat l-am revizuit, testat și adaptat manual înainte să-l integrez în proiect.

---

### Ce i-am cerut AI-ului

I-am dat următorul prompt:

Am următoarea structură în C:

```c
typedef struct Report {
    int report_id;
    char inspector_name[100];
    float latitude, longitude;
    char category[100];
    int severity;
    time_t timestamp;
    char description[100];
} Report;
```

Generează:
1. int parse_condition(const char *input, char *field, char *op, char *value)
   care să parseze un string de forma:
   câmp:operator:valoare

2. int match_condition(Report *r, const char *field, const char *op, const char *value)
   care returnează 1 dacă raportul îndeplinește condiția și 0 altfel.

Câmpuri suportate:
- severity
- category
- inspector
- timestamp

Operatori suportați:
== != < <= > >="

---

### Ce a generat AI-ul

AI-ul mi-a dat o prima implementare bazata pe:

- sscanf() pentru parsing
- strcmp() pentru comparații de stringuri
- atoi() / atoll() pentru conversii numerice

De exemplu, pentru parsing:

```c
int parse_condition(const char *input, char *field, char *op, char *value) {
    if (sscanf(input, "%[^:]:%[^:]:%s", field, op, value) == 3) {
        return 1;
    }
    return 0;
}
```

Ideea de bază era bună, dar implementarea avea câteva probleme.

---

### Ce am modificat eu

După ce am testat codul generat, am făcut următoarele îmbunătățiri:

### 1. Protecție împotriva buffer overflow

Varianta inițială folosea %[^:] fără limită, ceea ce putea duce la depășirea bufferelor.

Am înlocuit cu:

```c
return sscanf(input, "%99[^:]:%9[^:]:%99[^\n]", field, op, value) == 3;
```

Astfel mă asigur că nu depășesc dimensiunea bufferelor alocate.

### 2. Comparații adaptate la tipul fiecărui câmp

AI-ul făcea comparații generice, dar eu am tratat fiecare câmp în funcție de tipul lui:

- Pentru severity (care e int) am folosit atoi() să convertesc string-ul în număr
- Pentru category și inspector (care sunt stringuri) am folosit strcmp() direct
- Pentru timestamp (care e time_t, deci un long) am folosit atoll()

Asta era necesar ca filtrarea să funcționeze corect pentru fiecare tip de date.

### 3. Suport pentru comparații lexicografice

Pentru câmpurile text (category, inspector) am păstrat suportul pentru toți operatorii:

- == și !=
- <, <=, >, >=

Am folosit strcmp() care respectă ordinea lexicografică.

### 4. Integrare în filtrul din linia de comandă

Am integrat funcțiile în filter_reports() astfel încât să pot aplica mai multe condiții simultan.

Fiecare condiție e evaluată și combinate cu AND logic.

---

### Teste efectuate

Prima data, identificarea datelor:
```bash
./city_manager --role inspector --user maria --list midtown
```

Vom găsi, printre altele, aceste date:

report_id: 9166, category: water, inspector: alice, severity: 2, description: Conducta de apa sparta
report_id: 1695, category: lighting, inspector: maria, severity: 2, description: Stalp de iluminat daramat la intersectia principala.

Am testat manual următoarele cazuri:

**Condiție simplă:**
```bash
./city_manager --role inspector --user maria --filter midtown severity:\>=:2
```
Rezultat așteptat: Afișează ambele rapoarte (9166 și 1695), deoarece ambele au severitatea 2.

**Condiții multiple:**
```bash
./city_manager --role inspector --user maria --filter midtown severity:==:2 category:==:lighting
```
Afișează doar raportul 1695, deoarece este singurul care este și de tip lighting și are severitatea 2.

**Filtrare pe câmp text:**
```bash
./city_manager --role manager --user alice --filter midtown inspector:==:alice
```
Rezultat așteptat: Afișează doar raportul 9166, filtrând restul activității din district.


---

### Ce am învățat

Lucrul cu AI-ul la această parte din proiect m-a ajutat să înțeleg:

- Cum să folosesc sscanf() în mod sigur, cu limite de buffer
- Cum să parsez expresii structurate din linia de comandă
- Cum să compar valori în funcție de tipul câmpului
- Cum să integrez cod generat într-un proiect C existent


## Faza 2

### Notificarea prin SIGUSR1

Am utilizat AI pentru a înțelege implementarea corectă a `sigaction()`, deoarece cerința interzicea utilizarea funcției mai simple `signal()`.

### Gestiunea Proceselor

Pentru implementarea acestei comenzi, am ales să folosesc un proces copil pentru a rula comanda de sistem `rm`. Deși înțelegeam conceptul teoretic, am utilizat AI-ul pentru a clarifica mecanismul de sincronizare dintre părinte și copil:
- **Provocarea**: Utilizarea corectă a funcției `wait()` pentru a mă asigura că părintele șterge link-ul simbolic doar după ce procesul copil a terminat execuția comenzii `rm`.
La propunerea AI-ului, am insistat pe verificări de siguranță. Am adăugat logica de a bloca ștergerea dacă `district_id` este empty, `.` (directorul curent) sau `/` (rădăcina), pentru a preveni execuția accidentală a unei comenzi distructive precum `rm -rf /`.


## Faza 3

### 1. Clarificarea rolului și utilizării apelului `dup2()`
* **Context inițial:** Nu înțelegeam exact cum poate programul extern `calculate_scores` să își trimită automat output-ul în pipe-ul din Hub fără ca eu să modific codul scorerului și să îi adaug descriptori de fișiere.
* **Ce am întrebat AI-ul:** Am întrebat AI-ul care este rolul exact al funcției `dup2()` și cum reușește ea să redirecționeze ieșirea unui program extern.
* **Ce a răspuns AI-ul:** AI-ul mi-a explicat pe scurt că `dup2(fd_pipe[1], STDOUT_FILENO)` forțează sistemul de operare să înlocuiască ieșirea standard a procesului curent (ecranul) cu capătul de scriere al pipe-ului. Pentru că acest „plumbing” este făcut în Hub imediat după `fork()`, programul încărcat ulterior prin `execl()` moștenește această configurare. El va scrie pe ecran cu un `printf()` absolut banal, dar sistemul va devia toate datele direct prin conductă.
* **Cum am modificat:** Am plasat apelul `dup2()` exclusiv în procesele copii din `city_hub.c` înainte de apelurile `execl()`, lăsând executabilele externe (`monitor_reports` și `calculate_scores`) să folosească `printf` standard, fără a fi nevoie de vreo modificare structurală în ele pentru gestionarea directă a pipe-urilor.

### 2. Debugging pentru Pipe & Buffering
* **Context inițial:** După ce am implementat crearea pipe-ului și redirectarea `dup2(fd_pipe[1], STDOUT_FILENO)` în `start_monitor`, am observat un comportament ciudat: când rulam comanda în `city_hub`, terminalul rămânea blocat și nu afișa mesajul inițial de pornire de la monitor, deși codul părea corect scris.
* **Ce am întrebat AI-ul:** *„Dacă scriu comanda start_monitor și dau enter nu se întâmplă nimic, nu îmi apare mesajul de Running... PID : XXXX.”*
* **Ce a răspuns AI-ul:** AI-ul mi-a explicat un concept critic de Sisteme de Operare: comportamentul buffer-ului de `stdout` se schimbă automat în funcție de destinație. Când programul scrie direct în terminal, ieșirea este *line-buffered* (se trimite la `\n`), dar când este redirectat într-un *pipe*, devine *fully-buffered* (așteaptă să strângă mulți octeți înainte de a trimite). Din acest motiv, mesajul rămânea blocat în memoria procesului copil.
* **Cum am modificat:** Am adăugat apelul sistem `fflush(stdout);` în `monitor_reports.c` imediat după `printf`-ul de startup din `main`, dar și în interiorul funcției `handle(int sig)` pentru semnalul `SIGUSR1`. Acest lucru a forțat golirea imediată a bufferului și a deblocat citirea asincronă din `city_hub`.

### 3. Gestionarea corectă a descriptorilor de pipe (`close`)
* **Context inițial:** În funcția `calculate_scores`, după ce dădeam `fork()`, lăsam toate capetele pipe-ului deschise peste tot, crezând că nu afectează cu nimic rularea programului. Totuși, părintele se bloca la citire.
* **Ce am întrebat AI-ul:** Am întrebat AI-ul de ce procesul părinte rămâne blocat în bucla `while(read(...))` și nu termină niciodată execuția, deși procesul copilul își încheiase treaba.
* **Ce a răspuns AI-ul:** AI-ul mi-a explicat că apelul `read()` pe un pipe nu va returna niciodată `0` (End of File) atât timp cât există cel puțin un descriptor de scriere (`fd_pipe[1]`) lăsat deschis în sistem. Deoarece părintele nu își închisese propria copie a capătului de scriere înainte de a începe să citească, sistemul aștepta la infinit.
* **Cum am modificat:** Am adăugat imediat după `fork()` instrucțiunile de închidere specifice: copilul închide citirea (`fd_pipe[0]`), iar părintele închide imediat scrierea (`fd_pipe[1]`) înainte de a intra în bucla de citire a datelor, permițând funcției `read()` să termine corect ciclarea când copilul moare.

## Asistență suplimentară pentru redactarea documentației, la fiecare fază

Pe lângă scrierea codului, am utilizat asistența AI pentru:
- **Structurarea fișierelor .md**: Transformarea notițelor într-o documentație lizibilă și bine organizată (README și AI usage).
- **Corectare gramaticală**: Asigurarea unui ton profesional și utilizarea diacriticelor în limba română.

