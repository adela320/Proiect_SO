## AI usage phase 3

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

### De asemenea..
De asemenea, am utilizat AI pentru formatarea, structurarea și redactarea curată a acestui fișier Markdown, asigurând o așezare în pagină clară și ușor de înțeles.
