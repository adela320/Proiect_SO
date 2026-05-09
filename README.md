# City Manager - Faza 1 & 2

Sistem de gestionare a incidentelor urbane bazat pe fișiere binare, ierarhii de permisiuni Linux și comunicare între procese prin semnale.

## Structura

* city_manager.c: Codul sursă complet.
* monitor_reports.c: Programul de monitorizare a incidentelor
* ai_usage.md: Documentația privind utilizarea și corectarea codului AI - Faza 1
* ai_usage-phases_1_and_2.md : Documentația privind utilizarea și corectarea codului AI - Faza 2
* downtown/, midtown/, etc: Structura de directoare și fișiere de date.
* active_reports-*: Link-uri simbolice (gestionate via lstat)
* .monitor_pid : Fișier ascuns creat automat pentru stocarea PID-ului monitorului

## Compilare

```bash
gcc -Wall -o city_manager city_manager.c
gcc -Wall -o monitor_reports monitor_reports.c
```


## Exemple de utilizare (District: midtown)

### Adăugare (Manager/Inspector):

```bash
./city_manager --role manager --user alice --add midtown < water.txt
```

### Listare (vezi permisiuni & ID-uri):

```bash
./city_manager --role inspector --user maria --list midtown
```

### Vizualizare Detaliată (ID: 1695):

```bash
./city_manager --role inspector --user maria --view midtown 1695
```

### Filtrare:

```bash
./city_manager --role inspector --user maria --filter midtown severity:\>=:2
```

### Ștergere raport cu un anumit ID:

```bash
./city_manager --role manager --user alice --remove_report midtown 1695
```

**Notă:** Pentru operatorii > și < se folosește backslash \ pentru a preveni redirecționarea în shell (ex: severity:\>=:2).

### Ștergere district:

```bash
./city_manager --role manager --user alice --remove_district midtown
```
**Notă:**: Această comandă șterge recursiv folderul districtului și link-ul simbolic asociat.

### Procedura de Testare (Monitorizare prin Semnale)

Deschideți un terminal.
După ce ați compilat fișierele .c, rulați:

```bash
./monitor_reports
```

În același timp, fără să închideți terminalul, deschideți altul.
Adăugați un raport, de exemplu:

```bash
./city_manager --role manager --user alice --add another_town2 < flooding.txt
```

În terminalul 1 (cel cu monitorul) ar trebui să apară imediat mesajul că s-a adăugat un raport nou.
Puteți să mai adăugați și alte rapoarte.
Pentru a opri monitorul, folosiți CTRL + C în terminalul 1.
În fișierul logged_district corespunzător districtului utilizat veți putea vedea mesajele care vă spun dacă monitorul a fost informat cu succes.

