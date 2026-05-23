#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define maxi 256
#define max_districts 50

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
   }
    close(read_fd);
    wait(NULL); //asteptam procesul monitor sa se inchida de tot
    exit(0);
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
        int fd_pipe[2];
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

void calculate_scores()
{
    char *districts[max_districts];
    int dist_count = 0;

    char *token = strtok(NULL, " ");
    while(token != NULL && dist_count < max_districts)
    {
        districts[dist_count] = token;
        dist_count++;
        token = strtok(NULL, " ");
    }

    if(dist_count == 0)
    {
        printf("Comanda trebuie sa fie de forma: calculate_scores <district1> <district2> ...\n");
        return;
    }

    int read_pipes[max_districts];

    // lansam toate procesele in paralel
    for(int i = 0; i < dist_count; i++)
    {
        int fd_pipe[2];
        if(pipe(fd_pipe) < 0)
        {
            perror("pipe error:");
            read_pipes[i] = -1;
            continue;
        }

        pid_t pid = fork();
        if(pid < 0)
        {
            perror("fork error:");
            read_pipes[i] = -1;
            continue;
        }
        else if(pid == 0)
        {
            dup2(fd_pipe[1], STDOUT_FILENO);
            close(fd_pipe[0]);
            close(fd_pipe[1]);

            execl("./calculate_scores", "calculate_scores", districts[i], NULL);
            perror("Eroare execl calculate_scores");
            exit(1);
        }
        else
        {
            close(fd_pipe[1]); // inchide capatul de scriere - procesul parinte
            read_pipes[i] = fd_pipe[0]; // salvam capatul de citire pentru mai tarziu
        }
    }

    // luam pe rand rez de la fiecare si le formatam curat
    printf("\nWORKLOAD REPORT\n");
    for(int i = 0; i < dist_count; i++)
    {
        if(read_pipes[i] == -1)
        {
            continue;
        }

        printf("\nDistrict: %s\n", districts[i]);

        char buffer[maxi*2];
        ssize_t bytes_read;

        while(1)
        {
            bytes_read = read(read_pipes[i], buffer, sizeof(buffer) - 1);
            if(bytes_read <= 0)
            {
                break;
            }

            buffer[bytes_read] = '\0';
            printf("%s", buffer);
        }

        close(read_pipes[i]);
    }
    printf("\n");

    // curatam toate procesele zombie
    for(int i = 0; i < dist_count; i++)
    {
        wait(NULL);
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
            // daca monitorul a fost pornit, il oprim fortat
            if (hub_mon_pid > 0) {
                kill(hub_mon_pid, SIGINT); // trimitem SIGINT ca monitorul sa se inchida si sa stearga .monitor_pid
            }
            break;
        }
        else if(strcmp(cmd, "start_monitor") == 0)
        {
            start_monitor();
        }
        else if(strcmp(cmd, "calculate_scores") == 0)
        {
            calculate_scores();
        }
        else
        {
            printf("Comanda necunoscuta\n");
            print_comenzi();
        }
    }
    return 0;
}


