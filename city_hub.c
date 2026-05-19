#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define maxi 256



void print_comenzi();
void proceseaza_iesire_monitor(int read_fd);
void start_monitor();
void calculate_scores();

void print_comenzi()
{
    printf("Comenzi disponibile:\n");
    printf("  start_monitor\n");
    printf("  calculate_scores\n");
    printf("  exit\n");
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

        if(strcmp(cmd, "exit") == 0)
        {
            printf("Inchidere...\n");
            break;
        }
        else if(strcmp(cmd, "start_monitor") == 0)
        {
           // start_monitor();
        }
        else if(strcmp(cmd, "calculate_scores") == 0)
        {
           // calculate_scores();
        }
        else
        {
            printf("Comanda necunoscuta\n");
            print_comenzi();
        }
    }
    return 0;
}


