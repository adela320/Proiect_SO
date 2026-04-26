# Documentație privind utilizarea AI

## Context

La partea de filtrare din proiect am apelat la un asistent AI pentru a genera structura inițială a două funcții:

- parse_condition()
- match_condition()

Am folosit AI-ul ca punct de plecare, nu ca soluție finală. Codul generat l-am revizuit, testat și adaptat manual înainte să-l integrez în proiect.

---

## Ce i-am cerut AI-ului

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

## Ce a generat AI-ul

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

## Ce am modificat eu

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

Am integrat funcțiile în filter_reports() astfel încât să pot aplica mai multe condiții simultan:

```bash
./city_manager --role inspector --user bob --filter downtown severity:>=:2 category:==:lighting
```

Fiecare condiție e evaluată și combinate cu AND logic.

---

## Teste efectuate

Am testat manual următoarele cazuri:

**Condiție simplă:**
```bash
./city_manager --role inspector --user bob --filter downtown severity:>=:2
```

**Condiții multiple:**
```bash
./city_manager --role inspector --user bob --filter downtown severity:>=:2 category:==:lighting
```

**Filtrare pe câmp text:**
```bash
./city_manager --role inspector --user bob --filter downtown inspector:==:alice
```

Toate au funcționat corect după modificările mele.

---

## Ce am învățat

Lucrul cu AI-ul la această parte din proiect m-a ajutat să înțeleg:

- Cum să folosesc sscanf() în mod sigur, cu limite de buffer
- Cum să parsez expresii structurate din linia de comandă
- Cum să compar valori în funcție de tipul câmpului
- Cum să integrez cod generat într-un proiect C existent

AI-ul mi-a oferit un schelet util, dar verificarea și corectarea manuală au fost esențiale pentru ca totul să meargă cum trebuie.
