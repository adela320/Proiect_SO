#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h> // folosit pt mode_t
#include <sys/stat.h>   // stat(), chmod(), mkdir()
#include <fcntl.h>    // pentru open() si flag-uri
#include <unistd.h>   // pentru read(), write(), lseek() si altele
#define MAX 100


typedef struct Report{
     int report_id;
     char inspector_name[MAX];
     float latitude, longitude;
     char category[MAX];
     int severity;
     time_t timestamp;
     char description[MAX];
}Report;



void modif(mode_t mode, char *str) //pentru afisare drepturilor
{
     str[0] = (mode & S_IRUSR) ? 'r' : '-';
     str[1] = (mode & S_IWUSR) ? 'w' : '-';
     str[2] = (mode & S_IXUSR) ? 'x' : '-';
     str[3] = (mode & S_IRGRP) ? 'r' : '-';
     str[4] = (mode & S_IWGRP) ? 'w' : '-';
     str[5] = (mode & S_IXGRP) ? 'x' : '-';
     str[6] = (mode & S_IROTH) ? 'r' : '-';
     str[7] = (mode & S_IWOTH) ? 'w' : '-';
     str[8] = (mode & S_IXOTH) ? 'x' : '-';
     str[9] = '\0';

}
void creare_fisiere(const char *district_id)
{
    char path[MAX];
    if(mkdir(district_id, 0750) == -1) //se creaza directorul, iar daca exista atunci
    {
        chmod(district_id, 0750); //modific permisiunile
    }

    int fd; //file descriptor

    snprintf(path, sizeof(path), "%s/reports.dat", district_id); //creez reports.dat daca nu exista
    fd = open(path, O_WRONLY | O_CREAT, 0664); //write only + create
    if(fd != -1)
    {
       chmod(path, 0664);
       close(fd);
    }

    snprintf(path, sizeof(path), "%s/district.cfg", district_id);
    fd = open(path, O_WRONLY | O_CREAT, 0640);
    if (fd != -1) {
        chmod(path, 0640);
        close(fd);
    }

    snprintf(path, sizeof(path), "%s/logged_district", district_id);
    fd = open(path, O_WRONLY | O_CREAT, 0644);
    if (fd != -1) {
        chmod(path, 0644);
        close(fd);
    }

}

int verificare_acces(const char *filename, const char *role, char mode) { //implementare logica permisiuni
    struct stat st;
    if (stat(filename, &st) < 0)
    {
        return 0;
    }
    if (strcmp(role, "manager") == 0) {
        if (mode == 'r')
        {
            return (st.st_mode & S_IRUSR);
        }
        if (mode == 'w')
        {
            return (st.st_mode & S_IWUSR);
        }
    }
    else if (strcmp(role, "inspector") == 0) {

        if (mode == 'r')
        {
            return (st.st_mode & S_IRGRP);
        }
        if (mode == 'w')
        {
            return (st.st_mode & S_IWGRP);
        }
    }
    return 0;
}

//Partea cu AI:


/**
   Functia 1: parse_condition
   Separa un sir de forma "camp:operator:valoare" (ex: "severity:>=:2")
   Returneaza 1 la succes, 0 la format invalid.
 */

int parse_condition(const char *input, char *field, char *op, char *value) {
    return sscanf(input, "%99[^:]:%9[^:]:%99[^\n]", field, op, value) == 3;
}

/**
  Functia 2: match_condition
  Verifica daca un raport 'r' indeplineste conditia data.
  Returneaza 1 (True) sau 0 (False).
 */

int match_condition(Report *r, const char *field, const char *op, const char *value) {
    // Filtrarea dupa Severity
   // printf("field=[%s] op=[%s] value=[%s] category=[%s]\n", field, op, value, r->category);
    if (strcmp(field, "severity") == 0) {
        int val = atoi(value);
        if (strcmp(op, "==") == 0)
        {
            return r->severity == val;
        }
        if (strcmp(op, "!=") == 0)
        {
            return r->severity != val;
        }
        if (strcmp(op, ">") == 0)
        {
            return r->severity > val;
        }
        if (strcmp(op, ">=") == 0)
        {
            return r->severity >= val;
        }
        if (strcmp(op, "<") == 0)
        {
            return r->severity < val;
        }
        if (strcmp(op, "<=") == 0)
        {
            return r->severity <= val;
        }
    }
    //  Filtrare dupa category
    else if (strcmp(field, "category") == 0) {
        int res = strcmp(r->category, value);
        if (strcmp(op, "==") == 0)
        {
            return res == 0;
        }
        if (strcmp(op, "!=") == 0)
        {
            return res != 0;
        }
        // Operatorii <, >, <=, >= functioneaza lexicografic pentru string-uri
        if (strcmp(op, ">") == 0)
        {
            return res > 0;
        }
        if (strcmp(op, ">=") == 0)
        {
            return res >= 0;
        }
        if (strcmp(op, "<") == 0)
        {
            return res < 0;
        }
        if (strcmp(op, "<=") == 0)
        {
            return res <= 0;
        }
    }
    //  Filtrare dupa inspector (String)
    else if (strcmp(field, "inspector") == 0) {
        int res = strcmp(r->inspector_name, value);
        if (strcmp(op, "==") == 0)
        {
            return res == 0;
        }
        if (strcmp(op, "!=") == 0)
        {
            return res != 0;
        }
    }
    //  Filtrare dupa timestamp (time_t / long)
    else if (strcmp(field, "timestamp") == 0) {
        long long val = atoll(value);
        if (strcmp(op, "==") == 0)
        {
            return r->timestamp == val;
        }
        if (strcmp(op, ">") == 0)
        {
            return r->timestamp > val;
        }
        if (strcmp(op, "<") == 0)
        {
            return r->timestamp < val;
        }
    }

    return 0; // Camp necunoscut
}

//list_reports -> extrag si afisez toate datele pentru un anumit district
void list_reports(const char *district){
     char path[MAX];
     snprintf(path, sizeof(path), "%s/reports.dat", district);

    struct stat st;
    if (stat(path, &st) < 0) {
        perror("Eroare stat");
        return;
    }
    char modif_str[11];
    modif(st.st_mode, modif_str);
    printf("Fisier: %s, Permisiuni: %s, Dimensiune: %ld bytes, Ultimul modif: %ld\n", path, modif_str, st.st_size, st.st_mtime);

    int fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        return;
    }

    Report r;
    while (read(fd, &r, sizeof(Report)) > 0) {
        printf("%d %s, %s, severity: %d, description: %s\n", r.report_id, r.category, r.inspector_name, r.severity, r.description);
    }
    close(fd);
}


//logged_district permisiuni rw-r--r-- -> 4+2, 4, 4, -> 644

void log_action(const char *district, const char *role, const char *user, const char *action)
{
    char path[MAX];
    snprintf(path, sizeof(path), "%s/logged_district", district);

     //file descriptor
    int fd = open(path, O_WRONLY | O_APPEND | O_CREAT, 0644); //doar pt scriere, adaug la final, creaza
    if (fd < 0) return;

    char buffer[256];
    time_t now = time(NULL); //timestamp pt fiecare log
    int len = snprintf(buffer, sizeof(buffer), "%ld %s %s %s\n", now, user, role, action);
    write(fd, buffer, len);
    close(fd);
}

void verif_permisiuni(const char *path)
{
    struct stat st;
    if(stat(path, &st) == 0)
    {
        char aux[10];
        modif(st.st_mode, aux);
        printf("Permisiunile %s sunt: %s\n", path, aux);
    }
}

void add_report(const char *district_id, const char *role, const char *user)
{
     char path[MAX];
        snprintf(path, sizeof(path), "%s/reports.dat", district_id);

        // verificam accesul
        if (!verificare_acces(path, role, 'w')) {
            fprintf(stderr, "Acces refuzat");
            exit(1);
        }

        Report r;

        // date automate
        r.report_id = (int)time(NULL) % 10000;
        strncpy(r.inspector_name, user, MAX); //salvam numele inspectorului
        r.timestamp = time(NULL); //generam timestamp

        // datele le voi redirectiona dintr-un fisier

        if (scanf("%f %f %s %d", &r.latitude, &r.longitude, r.category, &r.severity) != 4) {
            fprintf(stderr, "Eroare: Format date de intrare invalid.\n");
            exit(1);
        }
        getchar(); // get rid of space / newline
        fgets(r.description, MAX, stdin);
        r.description[strcspn(r.description, "\n")] = 0; // elimina \n de la fgets

        //scrierea binara
        int fd = open(path, O_WRONLY | O_APPEND);
        if (fd != -1) {
            write(fd, &r, sizeof(Report));
            chmod(path, 0664); // ne asiguram ca ramane 664
            close(fd);

            log_action(district_id, role, user, "add");
            printf("Succes: Raport %d adaugat in %s\n", r.report_id, district_id);
        }
}

void list_district(const char *district_id, const char *role, const char *user)
{
    char path[MAX];
    snprintf(path, sizeof(path), "%s/reports.dat", district_id);

    if (verificare_acces(path, role, 'r')) {
        printf("Succes : Acces permis pentru list in districtul %s\n", district_id);

        list_reports(district_id);

        //verif permisiunea de scriere
        char log_path[MAX];
        snprintf(log_path, sizeof(log_path), "%s/logged_district", district_id);

        if (verificare_acces(log_path, role, 'w')) {
            log_action(district_id, role, user, "list");
        }
        else
        {
            fprintf(stderr, "Rolul %s nu are drept de scriere in %s\n", role, log_path);
            exit(1);
        }
    }
    else
    {
        fprintf(stderr, "Acces refuzat. Rolul %s nu poate citi %s\n", role, path);
        exit(1);
    }
}



void update_threshold(const char *district_id, const char *role, const char *user, const char *value) {
    // verificare rol -> doar managerul are voie
    if (strcmp(role, "manager") != 0) {
        fprintf(stderr, "Acces refuzat. Doar managerul poate schimba pragul de severitate\n");
        exit(1);
    }

    char path[MAX];
    snprintf(path, sizeof(path), "%s/district.cfg", district_id);

    // verif permisiuni -> trebuie sa fie exact 640
    struct stat st;
    if (stat(path, &st) == -1) {
        perror("Eroare stat config");
        exit(1);
    }

    // izolam doar bitii de permisiuni (ultimii 9 biti)
    mode_t current_mode = st.st_mode & 0777;
    if (current_mode != 0640) {
        fprintf(stderr, "Atentie: Permisiunile pentru %s sunt %o, nu 640! Operatie refuzata\n", path, current_mode);
        exit(1);
    }

    //Scrierea noii valori (fisier text)
    int fd = open(path, O_WRONLY | O_TRUNC); // O_TRUNC sterge continutul vechi
    if (fd != -1) {
        write(fd, value, strlen(value));
        close(fd);

        log_action(district_id, role, user, "update_threshold");
        printf("Succes: Pragul de severitate pentru %s a fost actualizat la %s\n", district_id, value);
    }
}


void view_report(const char *district_id, const char *role, const char *user, int target_id) {
    char path[MAX];
    snprintf(path, sizeof(path), "%s/reports.dat", district_id);

    // verificam accesul de citire (ambele roluri au voie pe 664)
    if (!verificare_acces(path, role, 'r')) {
        fprintf(stderr, "Eroare: Acces refuzat la citirea rapoartelor\n");
        exit(1);
    }

    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("Eroare deschidere reports.dat");
        return;
    }

    Report r;
    int gasit = 0;
    // citim inregistrari de dimensiune fixa pana gasim id-ul
    while (read(fd, &r, sizeof(Report)) > 0) {
        if (r.report_id == target_id) {
            printf("\nRAPORT %d \n", r.report_id);
            printf("Inspector:   %s\n", r.inspector_name);
            printf("Location:     Lat: %.4f, Lon: %.4f\n", r.latitude, r.longitude);
            printf("Category:   %s\n", r.category);
            printf("Severity:  %d\n", r.severity);
            printf("Time:        %s", ctime(&r.timestamp));
            printf("Description:   %s\n", r.description);
            printf("\n");
            gasit = 1;
            break;
        }
    }

    if (!gasit) {
        printf("Raportul cu ID %d nu a fost gasit in %s\n", target_id, district_id);
    }

    close(fd);
}


void filter_reports(const char *district_id, const char *role, int argc, char **argv, int start_index) {
    char path[MAX];
    snprintf(path, sizeof(path), "%s/reports.dat", district_id);

    if (!verificare_acces(path, role, 'r')) {
        fprintf(stderr, "Acces refuzat la citire.\n");
        exit(1);
    }

    int fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        return;
    }

    Report r;
    while (read(fd, &r, sizeof(Report)) > 0) {
        int all_match = 1;
        // parcurgem toate conditiile primite ca arg // ex : severity:>:2 category:==:road)
        for (int i = start_index; i < argc; i++) {
            char field[MAX], op[MAX], value[MAX];
            if (!parse_condition(argv[i], field, op, value)) {
                all_match = 0;
                break;
            }
              if (!match_condition(&r, field, op, value)) {
                    all_match = 0;
                    break;
            }
        }

        if (all_match) {
            printf("ID: %d\nCategory: %s\nSeverity: %d\nInspector: %s\n", r.report_id, r.category, r.severity, r.inspector_name);
        }
    }
    close(fd);
}


void remove_report(const char *district_id, const char *role, const char *user, int target_id) {
    // verificare rol -> doar managerul are voie
    if (strcmp(role, "manager") != 0) {
        fprintf(stderr, "Eroare: Acces refuzat. Doar managerul poate sterge rapoarte.\n");
        exit(1);
    }

    char path[MAX];
    snprintf(path, sizeof(path), "%s/reports.dat", district_id);

    // deschidem cu O_RDWR pentru a putea si citi si scrie in acelasi timp
    int fd = open(path, O_RDWR);
    if (fd == -1) {
        perror("Eroare deschidere reports.dat");
        return;
    }

    Report r;
    off_t pos_to_delete = -1;

    // cautam pozitia raportului de sters
    while (read(fd, &r, sizeof(Report)) > 0) {
        if (r.report_id == target_id) {
            // retinem pozitia de inceput a inregistrarii gasite
            pos_to_delete = lseek(fd, 0, SEEK_CUR) - sizeof(Report);
            break;
        }
    }

    if (pos_to_delete == -1) {
        printf("Raportul %d nu a fost gasit\n", target_id);
        close(fd);
        return;
    }

    // shiftare:  mutam restul rapoartelor inapoi
    Report next_r;
    off_t current_read_pos = pos_to_delete + sizeof(Report);
    off_t current_write_pos = pos_to_delete;

    while (1) {
        // sarim la pozitia de unde trebuie sa citim "urmatorul"
        lseek(fd, current_read_pos, SEEK_SET);
        ssize_t bytes_read = read(fd, &next_r, sizeof(Report));

        if (bytes_read <= 0) break; // finalul fisierului

        // ne intoarcem la poz de scriere si punem datele citite
        lseek(fd, current_write_pos, SEEK_SET);
        write(fd, &next_r, sizeof(Report));

        // avansam pozitiile pt urm iteratie
        current_read_pos += sizeof(Report);
        current_write_pos += sizeof(Report);
    }

    // Truncate -> stergem ultimul bloc ramas duplicat
    struct stat st;
    fstat(fd, &st);
    off_t new_size = st.st_size - sizeof(Report);

    if (ftruncate(fd, new_size) == -1) {
        perror("Eroare ftruncate");
    } else {
        log_action(district_id, role, user, "remove_report");
        printf("Succes: Raportul %d a fost eliminat, ftruncate a mers\n", target_id);
    }

    close(fd);
}

int main(int argc, char **argv)
{
     char *role = NULL;
     char *user = NULL;
     char *district_id = NULL;
     char *cmd = NULL;
     char *extra_arg = NULL;
     int filter_start_index = 0;
     printf("Nr de argumente: %d\n", argc);

     for(int i = 1; i < argc; i ++)
     {
         if(strcmp(argv[i], "--role") == 0)
         {
              role = argv[++i];
         }
         else if(strcmp(argv[i], "--user") == 0)
         {
              user = argv[++i];
         }
         else if(strcmp(argv[i], "--add") == 0)
         {
             district_id = argv[++i];
             cmd = "add";
         }
         else if(strcmp(argv[i], "--list") == 0)
         {
             district_id = argv[++i];
             cmd = "list";
         }
         else if(strcmp(argv[i], "--view") == 0)
        {
             district_id = argv[++i];   // Primul param: districtul
             extra_arg = argv[++i];     // Al doilea param: ID-ul raportului
             cmd = "view";
        }
        else if(strcmp(argv[i], "--update_threshold") == 0)
        {
             district_id = argv[++i];   // Primul param: districtul
             extra_arg = argv[++i];     // Al doilea param: valoarea pragului
             cmd = "update_threshold";
        }
        else if (strcmp(argv[i], "--remove_report") == 0) {
            district_id = argv[++i];
            extra_arg = argv[++i]; // ID-ul raportului de sters
            cmd = "remove_report";
        }
        else if (strcmp(argv[i], "--filter") == 0) {
            district_id = argv[++i];
            cmd = "filter";
            filter_start_index = i + 1; // conditiile incep dupa district_id
            break; // ne oprim aici pt ca restul op sunt conditii
        }

     }

     if (role == NULL || user == NULL || district_id == NULL || cmd == NULL) {
        fprintf(stderr, "lipsesc argumente necesare\n");
        exit(1);
    }

     printf("role: %s, user: %s, district_id: %s, cmd: %s\n", role, user, district_id, cmd); //pt verificare

    if (strcmp(cmd, "filter") != 0) {
        creare_fisiere(district_id);
    }

     char link_name[MAX], target_path[MAX];

     if (strcmp(cmd, "filter") != 0) {

        //"Symbolic links pointing to each district's report file"
        snprintf(link_name, sizeof(link_name), "active_reports-%s", district_id);
        snprintf(target_path, sizeof(target_path), "%s/reports.dat", district_id);

        //daca cumva exista legatura mai intai trb stearsa
        struct stat link_stat;
        if (lstat(link_name, &link_stat) == 0) {
            unlink(link_name);
        }
        symlink(target_path, link_name);
     }

    verif_permisiuni(district_id);

    //check if cmd != NULL

    if (cmd == NULL) {
        fprintf(stderr, "Nu a fost specificata nicio comanda add/list.\n");
        exit(1);
    }

     if(strcmp(cmd, "list") == 0)
     {
          list_district(district_id, role, user);
     }
     else if (strcmp(cmd, "add") == 0) {
         add_report(district_id, role, user);

      }
      else if (strcmp(cmd, "view") == 0)
    {
         if (extra_arg == NULL) {
             fprintf(stderr, "Eroare: Comanda view necesita un ID de raport\n");
             exit(1);
         }
         // Transformăm extra_arg din string in int pentru funcția view
         view_report(district_id, role, user, atoi(extra_arg));
    }
    else if (strcmp(cmd, "update_threshold") == 0)
    {
         if (extra_arg == NULL) {
             fprintf(stderr, "Eroare: Comanda update_threshold necesita o valoare\n");
             exit(1);
         }
         update_threshold(district_id, role, user, extra_arg);
    }
    else if (strcmp(cmd, "remove_report") == 0) {
        if (extra_arg == NULL) {
            fprintf(stderr, "Eroare: remove_report necesita un ID.\n");
            exit(1);
        }
       remove_report(district_id, role, user, atoi(extra_arg));
    }
    else if (strcmp(cmd, "filter") == 0) {
         filter_reports(district_id, role, argc, argv, filter_start_index);
    }

     return 0;
}
