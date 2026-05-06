#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define pid_file ".monitor.pid"

void handle(int sig)
{
    if(sig == SIGUSR1)
    {
         printf("Un nou report a fost adaugat\n");
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

    char pid_str[10];
    int len = sprintf(pid_str, "%d", getpid());
    write(fid, pid_str, len);
    close(fid);


    struct sigaction sa;
    sa.sa_handler = handle;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    //sigaction(signum, act, oldact)
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

    printf("Running...PID : %d\n", getpid());
    while(1)
    {
       pause();
    }

    return 0;
}
