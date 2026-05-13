#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define maxi 256

void print_comenzi()
{
    printf("Comenzi disponibile:\n");
    printf("  start_monitor\n");
    printf("  calculate_scores\n");
    printf("  exit()\n");
}

int main(void)
{
    char input[maxi];
    char *cmd = NULL;
    while(1)
    {
        if(fgets(input, sizeof(input), stdin) == NULL)
        {
            break;
        }
        input[strcspn(input, "\n")] = 0;
        cmd = strtok(input, " ");
        if(cmd == NULL)
        {
             continue;
        }
        else if(strcmp(cmd, "exit") == 0)
        {
            printf("Inchidere...");
            exit(1);
        }
        else if(strcmp(cmd, "start_monitor") == 0)
        {
            printf("Logica pentru start_monitor\n");
        }
        else if(strcmp(cmd, "calculate_scores") == 0)
        {
            printf("Logica pentru calculate_scores\n");
        }
        else
        {
            printf("Comanda necunoscuta\n");
            print_comenzi();
        }
    }
    return 0;
}


