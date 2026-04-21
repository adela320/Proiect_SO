#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h> // folosit pt mode_t
#include <sys/stat.h>   // stat(), chmod(), mkdir()
#include <fcntl.h>    // pentru open() si flag-uri
#include <unistd.h>   // pentru read(), write(), lseek() si altele
#define MAX 100

// functie modif(char *pathname, mode_t mode) -> modific drepturile de acces ale fisierului (S_IRUSR, S_IWUSR etc)

void modif(mode_t mode, char *str)
{
     str[0] = (mode & S_IRUSR) ? 'r' : '-';
     //(mode & S_IRGRP) ? 'r' : '-';
     //s[9] = 0;

}

//list_reports -> extrag si afisez toate datele pentru un anumit district
void list_reports(const char *district){}



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

typedef struct Report{
     int report_id;
     char inspector_name[MAX];
     float latitude, longitude;
     char category[MAX];
     int severity;
     time_t timestamp;
     char description[MAX];
}Report;

int main(int argc, char **argv)
{
     char *role = NULL;
     char *user = NULL;
     char *district_id = NULL;
     char *cmd = NULL;
     printf("%d\n", argc);

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

     printf("role: %s, user: %s, district_id: %s\n", role, user, district_id);

     mkdir(district_id, 0750); // si chmod

     char link_name[MAX], target_path[MAX];
     snprintf(link_name, sizeof(link_name), "active_reports-%s", district_id); //%s/reports.dat

     //symlink(target_path, link_name);

     /*

     if(strcmp(cmd, "list") == 0)
     {
         //list_reports(district);
         //log_action(district, role, user, "list");
     }
     else if (strcmp(cmd, "add") == 0) {
        // logica de citire a datelor pentru report
        // scriem in reports.dat si setezi chmod
       // log_action(district, role, user, "add");
      }
      */

     return 0;
}
