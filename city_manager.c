#include <stdio.h>
#include <string.h>

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
