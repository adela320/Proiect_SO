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

void proceseaza_iesire_monitor(int read_fd)
{
   char buff[maxi];
   ssize_t bytes_read;
   while(1)
   {
       bytes_read = read(read_fd, buff, sizeof(buff) - 1);
       if(bytes_read <= 0)
       {
           break;
       }
       buff[bytes_read] = '\0';
       printf("\n[hub_mon] %s", buff);
       fflush(stdout);
       if(strstr(buff, "ended") != NULL || strstr(buff, "Eroare") != NULL)
       {
        printf("[hub_mon] Notificare: Procesul monitor s-a terminat.\n");
       }
       close(read_fd);
       wait(NULL); //asteptam procesul monitor sa se inchida de tot
       exit(0);
   }
}

void start_monitor()
{
    //incepem un proces hub_mon in background
    pid_t hub_mon_pid = fork();
    if(hub_mon_pid < 0)
    {
        perror("Eroare fork pentru hub_mon");
        return;
    }
    else if(hub_mon_pid == 0)
    {
        //suntem in procesul hub_mon_pid
        pid_t fd_pipe[2];
        if(pipe(fd_pipe) < 0)
        {
            perror("Eroare creare pipe hub_mon");
            exit(1);
        }
        pid_t monitor_pid = fork();
        if(monitor_pid < 0)
        {
            perror("Eroare fork monitor");
            exit(1);
        }
        else if(monitor_pid == 0)
        {
            //suntem in procesul copil al lui hub_mon -> executam monitorul modificat
            dup2(fd_pipe[1], STDOUT_FILENO);
            close(fd_pipe[0]);
            close(fd_pipe[1]);

            execl("./monitor_reports", "monitor_reports", NULL);
            perror("Eroare execl monitor_reports");
            exit(1);
        }
        else
        {
            //in interiorul hub_mon, parintele monitorului
            close(fd_pipe[1]);

            proceseaza_iesire_monitor(fd_pipe[0]);
        }
    }
}

int main(void)
{
    char input[maxi];
    char *cmd = NULL;
    print_comenzi();
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
            start_monitor();
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


