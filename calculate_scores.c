#include <stdlib.h>
#include <stdio.h>
#include <time.h>
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

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        printf("Nr incorect de argumente\n");
        exit(1);
    }
    char path[MAX];
    snprintf(path, sizeof(path), "%s/reports.dat", argv[1]);

    FILE *f = fopen(path, "rb");
    if(f == NULL)
    {
        perror("Eroare la deschiderea fisierului\n");
        exit(1);
    }
    Report r;
    int total_severity = 0;
    int cnt = 0;
    //citim tot din fisierul binar si adunam severitatea
    while(fread(&r, sizeof(Report), 1, f))
    {
        total_severity += r.severity;
        cnt++;
    }
    fclose(f);
    //afis rez, citire prin pipe later
	printf("District: %s | Rapoarte: %d | Scor Total: %d\n", argv[1], cnt, total_severity);

    return 0;
}
