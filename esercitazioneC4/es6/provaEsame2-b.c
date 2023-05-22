#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>

typedef int pipe_t[2];
typedef struct{
    char SCf;
    long int Soccorrenze;
} structFiglio;

int main(int argc, char ** argv){
    /*------------variabili------------*/
    int N;                                         //per contenere il numero di caratteri da controllare
    int F;                                         //per contenere fd del parametro file
    int pidFiglio, status, ritorno;                //per fork e wait
    pipe_t pipedFiglio;                            //pipe padre figli
    char ch;                                       //per carattere letto dal file F
    char * Cf;                                     //per carattere associato a ogni figlio
    long int occorrenze;                           //per numero di occorrenze dle carattere Cf nel file F
    structFiglio infoFiglio;                       //per contenere info da passare da figlio a padre 
    /*---------------------------------*/

    //controllo: numero di parametri almeno 3
    if(argc<4){
        printf("Errore: numero di parametri %d, ma deve essere almeno 3\n", argc-1);
        exit (1);
    }
    N=argc-2;
    
    //controllo: primo parametro deve essere un file 
    if((F=open(argv[1], O_RDONLY))<0){
        printf("Errore: il primo parametro %s non è un file apribile in lettura\n", argv[1]);
        exit (2);
    }
    close(F);
    //controllo: ultimi N parametri sono singoli caratteri
    for(int i=2; i<argc; ++i){
        if(strlen(argv[i])!=1){
            printf("Errore: il parametro %s non è un singolo carattere\n", argv[i]);
            exit (3);
        }
    }
    
    //il processo padre crea la pipe
    if(pipe(pipedFiglio)<0){
    printf("Errore: problemi nella creazione della pipe\n");
    exit (2);
    }
    
    //il processo padre crea N figli
    printf("Sono il processo padre con pid %d e sto per creare %d figli\n", getpid(), N);
    for(int i=0; i<N; ++i){
        pidFiglio=fork();
            if(pidFiglio<0){
                printf("Errore: problemi nella fork\n");
                exit (-1);
            }  
            if(pidFiglio==0){
                //il figlio chiude i lati di lettura della pipe, e tutti i lati di scrittura tranne il suo
                close(pipedFiglio[0]); 
                
                //ogni figlio apre il file, così il file pointer è sempre all'inizio
                if((F=open(argv[1], O_RDONLY))<0){
                printf("Errore: il primo parametro %s non è un file apribile in lettura\n", argv[1]);
                exit (-1);
                }
                Cf= argv[i+2];
                occorrenze=0;

                //il figlio conta le occorrenze del carattere Cf associato nel file F
                while(read(F, &ch, 1)>0){
                    if(ch==*Cf){
                        occorrenze = occorrenze+1;
                    }
                }
                
                //il figlio comunica al padre il numero di occorrenze trovate
                infoFiglio.SCf=*Cf;
                infoFiglio.Soccorrenze=occorrenze;
                write(pipedFiglio[1], &infoFiglio, sizeof(structFiglio));

                exit (*Cf);
            }
    }
    
    //il padre legge il numero di occorrenze di ogni figlio
    for(int i=0; i<N; ++i){
        //il padre chiude i lati di scrittura della pipe
        close(pipedFiglio[1]);
        //il padre legge le occorrenze del carattere dalla pipe
        read(pipedFiglio[0], &infoFiglio,  sizeof(structFiglio));
        printf("Nel file %s sono state trovate %ld occorrenze del carattere %c\n", argv[1], infoFiglio.Soccorrenze, infoFiglio.SCf);
    }

    //il padre aspetta gli N figli
    for(int i=0; i<N; ++i){
        if ((pidFiglio=wait(&status)) < 0){
                printf("Errore wait\n");
                exit(5);
        }
        if ((status & 0xFF) != 0){
                printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
        }
        else{
                ritorno=(int)((status >> 8) & 0xFF);
                printf("Il figlio con pid=%d ha ritornato %c (%d se 255 problemi!)\n", pidFiglio, ritorno, ritorno);
        }
    }
    exit (0);
}

