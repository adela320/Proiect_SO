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
    if (stat(filename, &st) < 0){
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

int main(int argc, char **argv)
{
     char *role = NULL;
     char *user = NULL;
     char *district_id = NULL;
     char *cmd = NULL;
     printf("Nr de argumente: %d\n", argc);

     for(int i = 1; i < argc; i ++)
     {
         if(strcmp(argv[i], "--role") == 0)
         {
              role = argv[++i];
         }
         if(strcmp(argv[i], "--user") == 0)
         {
              user = argv[++i];
         }
         if(strcmp(argv[i], "--add") == 0)
         {
             district_id = argv[++i];
             cmd = "add";
         }
         if(strcmp(argv[i], "--list") == 0)
         {
             district_id = argv[++i];
             cmd = "list";
         }
     }

     if (role == NULL || user == NULL || district_id == NULL || cmd == NULL) {
        fprintf(stderr, "lipsesc argumente necesare\n");
        exit(1);
    }

     printf("role: %s, user: %s, district_id: %s, cmd: %s\n", role, user, district_id, cmd);

    creare_fisiere(district_id);

     char link_name[MAX], target_path[MAX];

     //"Symbolic links pointing to each district's report file"
     snprintf(link_name, sizeof(link_name), "active_reports-%s", district_id);
     snprintf(target_path, sizeof(target_path), "%s/reports.dat", district_id);

    //daca cumva exista legatura mai intai trb stearsa
    struct stat link_stat;
    if (lstat(link_name, &link_stat) == 0) {
        unlink(link_name);
    }
    symlink(target_path, link_name);

    verif_permisiuni(district_id);

    //check if cmd != NULL

    if (cmd == NULL) {
        fprintf(stderr, "Nu a fost specificata nicio comanda add/list.\n");
        exit(1);
    }

     if(strcmp(cmd, "list") == 0)
     {
        char path[MAX];
        snprintf(path, sizeof(path), "%s/reports.dat", district_id);

        if (verificare_acces(path, role, 'r')) {
            printf("Succes : Acces permis pentru list in districtul %s\n", district_id);

            list_reports(district_id);

            //verif permisiunea de scriere pe log
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
     else if (strcmp(cmd, "add") == 0) {
        // logica de citire a datelor pentru report
        // scriem in reports.dat si setezi chmod
        log_action(district_id, role, user, "add");
      }

     return 0;
}
