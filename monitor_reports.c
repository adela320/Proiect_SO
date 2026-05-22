#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#define max_pid 15 //sa am destul spatiu pt int care are max 10 cifre
#define pid_file ".monitor_pid"

void handle(int sig)
{
    if(sig == SIGUSR1)
    {
        printf("Un nou report a fost adaugat\n");
        fflush(stdout); //ca sa plece notificarea instant
    }
    else if(sig == SIGINT)
    {
        printf("Programul s-a terminat\n"); //ctrl + c
        unlink(pid_file);
        exit(0); //succes
    }
}

int main(void)
{
    int fid = open(pid_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fid == -1)
    {
        fprintf(stderr, "Eroare la deschiderea fisierului pid_file\n");
        exit(1);
    }

    char pid_str[max_pid];
    int len = sprintf(pid_str, "%d", getpid());
    if(write(fid, pid_str, len) == 1)
    {
        printf("Eroare la scrierea pid-ului\n");
        exit(1);
    }
    close(fid);


    struct sigaction sa;
    sa.sa_handler = handle;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    //sigaction(signum, act, oldact)
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

    printf("Running...PID : %d\n", getpid());
    fflush(stdout);
    while(1) //bucla infinita pana la SIGINT
    {
       pause();
    }

    return 0;
}
