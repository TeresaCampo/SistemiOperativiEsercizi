#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#define PERM 0644

int main(int argc, char ** argv){
        /*-------------*variabili locali-------------*/
    int N;                          //contiene numero di file
    int C;                          //contiene grandezza di un blocco
    char *b;                        //contiene il blocco appena letto (deve essere grande C byte)
    int nro;                        //contiene numero di blocchi letto da ogni processo dal file associato
    int lenFile;                    //contiene lunghezza del file in termini di blocchi b
    int fcreato;                    //contiene fd del file creato dal processo 2 della coppia
    int Fi;                         //contiene fd del file associato a ogni coppia di processi
    char *nomeFile;                 //contiene nome del file creato dal processo 2 della coppia

    typedef int pipe_t[2];
    pipe_t *pipedFf;                //contiene pipe tra figio(i)->figlio(1+N)
    int nr, nw;                     //per controllare lettura e scrittura pipe
    int pidFiglio, status, ritorno; //per fork e wait
        /*-------------------------------------------*/
    
    //controllo: almeno 2 parametri
    if(argc<3){
        printf("Errore: passati %d parametri, ma ne servono almeno 2\n", argc-1);
        exit (1);
    }
    N=argc-2;
    C=atoi(argv[argc-1]);
    //controllo: C numero strettamente positivo e dispari
    if(C<=0 || (C%2)==0){
        printf("Errore: il parametro %d dovrebbe essere un numero strettamente positivo e dispari\n", C);
        exit (2);
    }

    //controllo la lunghezza di un file(uguale per tutti)--> per sapere a cosa corrisponde metà file
    if((Fi=open(argv[1], O_RDONLY))<0){
    printf("Errore: impossibile aprire in lettura il file %s\n", argv[1]);
    exit (3);
    }
    //alloco b
    b= (char*) malloc(C*sizeof(char));
    if(b==NULL){
        printf("Errore: problemi nell'allocazione di memoria per l'array b\n");
        exit (4);
    }
    lenFile=0;
    while(read(Fi, b, C)>0){
        ++lenFile;
    }
    printf("DEBUG-Ogni file è lungo %d blocchi\n", lenFile);
    //alloco spazio per N pipe
    pipedFf= (pipe_t*) malloc(N*sizeof(pipe_t));
    if(pipedFf==NULL){
        printf("Errore: problemi nell'allocazione di memoria per le pipe pipedFf\n");
        exit (5);
    }
    //creo N pipe
    for(int n=0; n<N; ++n){
         if(pipe(pipedFf[n])<0){
            printf("Errore: problemi nella creazione della pipe pipedFf\n");
            exit (6);
        }
    }
    //creo 2*N processi figli
    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli\n", getpid(), (2*N));
    for(int i=0; i<(2*N); ++i){
        pidFiglio=fork();
        if(pidFiglio<0){
             printf("Errore nella fork\n");
             exit (7);
        }
         if(pidFiglio==0){
            printf("DEBUG-Sono il processo figlio con pid %d\n", getpid());
            //chiudo lati pipe
            for(int j=0; j<N; ++j){
                if(i<N){
                    close(pipedFf[j][0]);
                    if(j!=i){
                    close(pipedFf[j][1]);
                    }
                }
                else{
                    close(pipedFf[j][1]);
                    if(j!=(i%N)){
                    close(pipedFf[j][1]);
                    }
                }
                
            }

            //apro il file associato
            if((Fi=open(argv[1+(i%N)], O_RDONLY))<0){
            printf("Errore: impossibile aprire in lettura il file %s\n", argv[1+(i%N)]);
            exit (-1);
            }   

            //il processo 1 della coppia
            if(i<N){
                //legge il file fino a metà
                nro=0;
                while(nro<(lenFile/2)){
                    //legge un blocco
                    nr=read(Fi, b, C);
                    if(nr!=C){
                        printf("Errore: il figlio non riesce a leggere dal file %s\n", argv[1+(1%N)]);
                        exit (-1);
                    }
                    //scrive blocco al processo 2 della coppia
                    nw=write(pipedFf[i][1], b, C);
                    if(nw!=C){
                         printf("Errore: il figlio non riesce a scrivere sulla pipe pipedFf\n");
                        exit (-1);
                    }
                    ++nro;
                }
                exit (nro);     
            }
            //il processo 2 della coppia
            else{
                //crea il file .mescolato
                nomeFile= (char*) malloc((11+strlen(argv[1+(i%N)]))*sizeof(char));
                if(nomeFile==NULL){
                    printf("Errore: problemi nell'allocazione di memoria per l'array nomeFile\n");
                    exit (-1);
                }
                strcpy(nomeFile, argv[1+(i%N)]);
                strcat(nomeFile, ".mescolato");

                fcreato=creat(nomeFile, PERM);
                printf("DEBUG-Creato il file %s\n", nomeFile);

                //legge il file da metà fino alla fine
                lseek(Fi, (lenFile/2)*C, SEEK_CUR);
                
                nro=0;
                while(nro<(lenFile/2)){
                    //legge un blocco
                    nr=read(Fi, b, C);
                    if(nr!=C){
                        printf("Errore: il figlio non riesce a leggere dal file %s\n", argv[1+(1%N)]);
                        exit (-1);
                    }
                    //scrive blocco nel file appena creato
                    nw=write(fcreato, b, C);
                    if(nw!=C){
                         printf("Errore: il figlio non riesce a scrivere sul file %s\n", nomeFile);
                        exit (-1);
                    }
                    //legge blocco inviato dal processo 1
                    nr=read(pipedFf[i%N][0], b, C);
                    if(nr!=C){
                        printf("Errore: il figlio non riesce a leggere dalla pipe pipedFf\n");
                        exit (-1);
                    }
                     //scrive blocco nel file appena creato
                    nw=write(fcreato, b, C);
                    if(nw!=C){
                         printf("Errore: il figlio non riesce a scrivere sul file %s\n", nomeFile);
                        exit (-1);
                    }
                    ++nro;
                }
                exit (nro);     
            }
        }
    }
    //chiudo lati pipe
    for(int j=0; j<N; ++j){
        close(pipedFf[j][1]);
        close(pipedFf[j][0]);
    }

    //il padre aspetta tutti i processi figli
    for(int i=0; i<(2*N); ++i){
         if ((pidFiglio=wait(&status)) < 0){
             printf("Errore wait\n");
             exit(8);
        }
         if ((status & 0xFF) != 0){
             printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);}
         else{
             ritorno=(int)((status >> 8) & 0xFF);
             printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi!)\n", pidFiglio, ritorno);
         }
    }
    exit (0);
}