#include <stdio.h>
#include <string.h>
#include <time.h>
#define MAX 100

// functie modif(char *pathname, mode_t mode) -> modific drepturile de acces ale fisierului (S_IRUSR, S_IWUSR etc)


typedef struct Report{
     int report_id;
     char user[MAX];
     float latitude, longitude;
     int severity;
     time_t timestamp;
     char description_text[50];
}Report;

int main(int argc, char **argv)
{
     char *role = NULL;
     char *user = NULL;
     char *district_id = NULL;
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
         if((strcmp(argv[i], "--add") == 0) || (strcmp(argv[i], "--list") == 0))
         {
             district_id = argv[++i];
         }
     }

     printf("role: %s, user: %s, district_id: %s\n", role, user, district_id);

     return 0;
}
