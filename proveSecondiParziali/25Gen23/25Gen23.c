#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>

int main(int argc, char ** argv){
        /*-------------*variabili locali-------------*/
    int N;                          //contiene numero di file passati come parametri, tranne l'ultimo
    int fdw;                        //contiene fd dell'ultimo file passato come parametro
    int fdn;                        //contiene fd file associati ai figli
    char chs[2];                    //contiene i 2 caratteri letti dal file associato al figlio
    int nchr;                       //contiene numero di ch letti dal file con la read
    int inviati;                    //contiene numero di caratteri inviati dal figlio al padre
    int pipeLette;                  //contiene numero di pipe lette

    typedef int pipe_t[2];
    pipe_t *pipedFp;
    int nr, nw;                     //per controllare lettura e scrittura pipe
    int pidFiglio, status, ritorno; //per fork e wait
        /*-------------------------------------------*/

    //controlllo: almeno 3
    if(argc<4){
        printf("Errore: passati %d parametri, ma dovrebbero essere almeno 4\n", argc-1);
        exit (1);
    }
    N=argc-2;
    //controllo: file apribili in lettura lo controlleranno i figli
    //il padre apre l'ultimo file in scrittura
    if((fdw=open(argv[argc-1],  O_WRONLY))<0){
    printf("Errore: impossibile aprire in scrittura il file %s\n", argv[argc-1]);
    exit (2);
    }
    //il padre si pone a fine file
    lseek(fdw, 0, SEEK_END);
    
    //alloco spazio per array di pipe
    pipedFp= (pipe_t*) malloc(N*sizeof(pipe_t));
    if(pipedFp==NULL){
        printf("Errore: problemi nell'allocazione di memoria per le pipe pipedFp\n");
        exit (3);
    }
    //creo pipe
    for(int i=0; i<N; ++i){
         if(pipe(pipedFp[i])<0){
            printf("Errore: problemi nella creazione della pipe pipedFp\n");
            exit (4);
        }
    }

    //creo N processi figli
    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli\n", getpid(), N);
    for(int n=0; n<N; ++n){
        pidFiglio=fork();
        if(pidFiglio<0){
             printf("Errore nella fork\n");
             exit (5);
        }
         if(pidFiglio==0){
            printf("DEBUG-Sono il processo figlio con pid %d\n", getpid());
            //figlio chiude lati pipe: chiude tutti i lati in lettura, tutti i lati in scrittura tranne il proprio
            for(int j=0; j<N; ++j){
                close(pipedFp[j][0]);
                if(j!=n){
                     close(pipedFp[j][1]);
                }
            }

            //provo ad aprire il file associato
            if((fdn=open(argv[1+n], O_RDONLY))<0){
            printf("Errore: impossibile aprire in lettura il file %s\n", argv[1+n]);
            exit (-1);
            }

            inviati=0;
            while((nchr=read(fdn, chs, 2))>0){
                //il figlio riferisce al padre i caratteri letti
                nw=write(pipedFp[n][1], &chs, nchr);
                inviati+=nchr;
                if(nw!=nchr){
                     printf("Errore: il figlio non riesce a scrivere sulla pipe pipedFp\n");
                exit (-1);
                }
            }
            exit(inviati);
        }
    }

    //codice del padre
    //il padre chiude lati pipe: tutti i lati di scrittura
    for(int j=0; j<N; ++j){
        close(pipedFp[j][1]);
    }

    //il padre, per ogni figlio, legge 2 caratteri e li scirve nel file fdw
    pipeLette=0;
    for(int i=0; i<N; ++i){
        nr=read(pipedFp[i][0], &chs, sizeof(chs));
        //se non leggo nemmeno un carattere faccio scritta di DEBUG
        if(nr==0){
            printf("DEBUG-la pipe %d è stata chiusa\n", i);
        }
        //se leggo lameno un carattere lo scrivo sul file
        else{
            nw=write(fdw, &chs, nr);
            if(nw!=nr){
                printf("Errore: il padre non riesce a scrivere sulla file %s\n", argv[argc-1]);
                exit (6);
            }
            pipeLette+=1;
        }
    }
    
    while(pipeLette>0){
        pipeLette=0;
        for(int n=0; n<N; ++n){
            nr=read(pipedFp[n][0], &chs, sizeof(chs));
            //se non leggo nemmeno un carattere faccio scritta di DEBUG
            if(nr==0){
                printf("DEBUG-la pipe %d è stata chiusa\n", n);
            }
            //se leggo lameno un carattere lo scrivo sul file
            else{
                nw=write(fdw, &chs, nr);
                if(nw!=nr){
                    printf("Errore: il padre non riesce a scrivere sulla file %s\n", argv[argc-1]);
                    exit (6);
                }
                pipeLette+=1;
            }
        }
    }
    
    //il padre aspetta tutti i processi figli
    for(int n=0; n<N; ++n){
         if ((pidFiglio=wait(&status)) < 0){
             printf("Errore wait\n");
             exit(7);
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