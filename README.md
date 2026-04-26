# Proiect City Manager - Etapa 1

Acest program gestionează rapoartele de incidente urbane pe districte, folosind fișiere binare și permisiuni Linux.

## Compilare

```bash
gcc -Wall city_manager.c -o city_manager
```

## Exemple de utilizare

### 1. Adăugare raport (Manager/Inspector)

```bash
./city_manager --role manager --user alice --add downtown < test_add.txt
```

### 2. Listare rapoarte (Manager/Inspector)

```bash
./city_manager --role inspector --user bob --list downtown
```

### 3. Vizualizare detaliată

```bash
./city_manager --role inspector --user bob --view downtown <ID_RAPORT>
```

### 4. Filtrare (AI Assisted)

```bash
./city_manager --role manager --user alice --filter downtown severity:>=:2 category:==:road
```

### 5. Ștergere raport (Doar Manager)

```bash
./city_manager --role manager --user alice --remove_report downtown <ID_RAPORT>
```

## Structura Arhivei

* `city_manager.c`: Codul sursă.
* `ai_usage.md`: Documentația privind utilizarea AI.
* `downtown/`, `midtown/`, `uptown/`: Structura de directoare generată.
* `active_reports-*`: Link-uri simbolice către fișierele .dat.
