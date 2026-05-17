#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#define MAX 100

/*
   ./calculate_scores midtown -> exemplu pentru testare
 */

typedef struct Report{
     int report_id;
     char inspector_name[MAX];
     float latitude, longitude;
     char category[MAX];
     int severity;
     time_t timestamp;
     char description[MAX];
}Report;

//struct pentru a tine minte scorul fiecarui inspector
typedef struct {
    char name[MAX];
    int total_score;
}InspectorScore;

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
    InspectorScore scores[MAX];
    int inspector_count = 0;
    //citim tot din fisierul binar si adunam severitatea
    while(fread(&r, sizeof(Report), 1, f))
    {
        int ok = 0;
        for(int i = 0; i < inspector_count; i++) {
            if(strcmp(scores[i].name, r.inspector_name) == 0) {
                scores[i].total_score += r.severity;
                ok = 1;
                break;
            }
        }
        if(!ok && inspector_count < MAX)
        {
            strncpy(scores[inspector_count].name, r.inspector_name, MAX);
            scores[inspector_count].total_score = r.severity;
            inspector_count++;
        }
    }
    fclose(f);
    if(inspector_count == 0)
    {
        printf("Nu exista rapoarte in acest district.\n");
    }
    else
    {
        for(int i = 0; i < inspector_count; i++) {
            printf(" Inspector: %s, Total Score: %d\n", scores[i].name, scores[i].total_score);
        }
    }

    return 0;
}
