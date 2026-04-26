# City Manager - Phase 1

Sistem de gestionare a incidentelor urbane bazat pe fișiere binare și ierarhii de permisiuni Linux.

## Compilare

```bash
gcc -Wall city_manager.c -o city_manager
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

### Ștergere (doar Manager):

```bash
./city_manager --role manager --user alice --remove_report midtown 1695
```

**Notă:** Pentru operatorii > și < se folosește backslash (\) pentru a preveni redirecționarea în shell (ex: severity:\>=:2).

## Structura

* city_manager.c: Codul sursă complet.
* ai_usage.md: Documentația privind utilizarea și corectarea codului AI.
* downtown/, midtown/, uptown/: Structura de directoare și fișiere de date.
* active_reports-*: Link-uri simbolice (gestionate via lstat)
