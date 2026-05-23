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

Am integrat funcțiile în filter_reports() astfel încât să pot aplica mai multe condiții simultan.

Fiecare condiție e evaluată și combinate cu AND logic.

---

## Teste efectuate

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

## Ce am învățat

Lucrul cu AI-ul la această parte din proiect m-a ajutat să înțeleg:

- Cum să folosesc sscanf() în mod sigur, cu limite de buffer
- Cum să parsez expresii structurate din linia de comandă
- Cum să compar valori în funcție de tipul câmpului
- Cum să integrez cod generat într-un proiect C existent


Notă privind redactarea:
Pe lângă generarea logicii de filtrare, am utilizat asistentul AI și pentru a structura acest document (ai_usage.md) și pentru un scurt README.md, într-un format Markdown lizibil. Consider că vizual este mai ușor de citit decât un text simplu, cum ar fi fost inițial.
