# City Manager

Sistem de gestionare a incidentelor urbane bazat pe fișiere binare, ierarhii de permisiuni Linux și comunicare între procese prin conducte (pipes) și semnale.

## Structura Proiectului

* city_manager.c: Codul sursă complet al managerului de districte.
* monitor_reports.c: Programul de monitorizare a incidentelor.
* city_hub.c: Interfața interactivă principală (Hub-ul CLI).
* calculate_scores.c: Codul sursă pentru calcularea scorurilor de severitate.
* AI_usage-ALL-phases.md: Documentația privind utilizarea și corectarea codului AI - toate fazele.
* ai-usage-by-phases/: Director ce conține pentru fiecare faza în parte utilizarea AI.
* test-inputs/: Folderul cu fișierele text ce conțin datele de test.
* another_town2/, downtown/, midtown/, uptown/: Structura de directoare și fișiere de date per district.
* active_reports-*: Link-uri simbolice către fișierele de date (gestionate via lstat).
* .monitor_pid: Fișier ascuns creat automat pentru stocarea PID-ului monitorului.

## Observații Organizare Districte

- **Faza 1:** Districtele inițiale folosite au fost `downtown`, `midtown` și `uptown`.
- **Faza 2:** S-au utilizat în principal `another_town2` și `uptown` pentru testarea monitorizării și a semnalului `SIGUSR1`.
- **Faza 3:** S-a utilizat în principal `another_town2`.

**Notă:** Avem `another_town2` deoarece `another_town` a fost șters pentru testarea `remove_district` și nu mai există în prezent.

## Compilare

```bash
gcc -Wall -o city_manager city_manager.c
gcc -Wall -o monitor_reports monitor_reports.c
gcc -Wall -o city_hub city_hub.c
gcc -Wall -o calculate_scores calculate_scores.c
```

**Notă:** Înainte de a trece la exemple de utilizare & testare, compilați fișierele `.c`.

## Exemple de Utilizare

**Observație** Comenzile `update_threshold`, `remove_report` și `remove_district` pot fi executate **exclusiv de rolul de manager**. Dacă sunt rulate de un inspector, programul va afișa un mesaj de avertizare privind lipsa permisiunilor și se va opri imediat.

### Adăugare (Manager/Inspector):

```bash
./city_manager --role manager --user alice --add midtown < test-inputs/water.txt
```

### Listare (vezi permisiuni & ID-uri):

```bash
./city_manager --role inspector --user maria --list midtown
```

### Vizualizare Detaliată (ID: 1695):

```bash
./city_manager --role inspector --user maria --view midtown 1695
```

### Update threshold (pragul de severitate)
```bash
./city_manager --role manager --user maria --update_threshold midtown 2
```

### Filtrare:

```bash
./city_manager --role inspector --user maria --filter midtown severity:\>=:2
```

### Ștergere raport cu un anumit ID:

```bash
./city_manager --role manager --user alice --remove_report midtown 1695
```

**Notă:** Pentru operatorii `>` și `<` se folosește backslash `\` pentru a preveni redirecționarea în shell (ex: `severity:\>=:2`).

### Ștergere district:

```bash
./city_manager --role manager --user alice --remove_district midtown
```

**Notă:** Această comandă șterge recursiv folderul districtului și link-ul simbolic asociat.

## Ghid de Testare a Funcționalităților

### Schimbare comportament monitor (Faza 2 vs. Faza 3)

* **Înainte (Faza 2):** Monitorul era strâns legat de sesiunea curentă. Trebuia deschis manual un terminal separat pentru a rula `./monitor_reports`. Dacă acel terminal se închidea sau se omitea rularea, în log apărea eroarea *"Monitor could not be informed"*.
* **Acum (Faza 3):** Monitorul a devenit un proces independent de fundal (de tip *daemon*). Acesta este lansat inițial din interfața `city_hub` prin comanda `start_monitor`, însă odată pornit, el rulează permanent și asincron în background-ul sistemului de operare (până la primirea unui semnal explicit de oprire). Din acest motiv, chiar și dacă părăsim interfața Hub-ului cu `exit`, procesul de monitorizare rămâne activ în memorie, interceptează semnalele din alte terminale, iar acțiunea este înregistrată mereu cu succes în log ca *"Monitor informed"*.

### Testare – city_hub

Deschideți un terminal.
Asigurați-vă că ați compilat fișierele `.c` și rulați:

```bash
./city_hub
```

Vă vor apărea câteva comenzi din care puteți alege (`start_monitor`, `exit`, `calculate_scores`).
Porniți monitorul în fundal tastând comanda:

```bash
start_monitor
```

Ar trebui să apară imediat pe ecran un mesaj de tipul: `[hub_mon] Running... PID: <un_număr_de_PID>`

Deschideți un al doilea terminal, intrați în folderul proiectului și adăugați un raport, de exemplu:

```bash
./city_manager --role manager --user ana --add another_town2 < test-inputs/flooding.txt
```

Veți vedea acum în primul terminal mesajul: `[hub_mon] Un nou raport a fost adaugat`

Pentru a testa și comanda `calculate_scores`, introduceți în primul terminal (cel în care ați introdus comanda `./city_hub`) o comandă precum:

```bash
calculate_scores midtown another_town2
```

Va apărea un rezultat de forma:

```
District: [nume_district]
  Inspector: [nume_inspector], Total Score: [număr]
  ...
```

La final, alegeți comanda `exit` pentru a ieși din program:
```bash
exit
```
