#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

//funzione random
int mia_random(int n){
    int casuale;
    casuale=rand()%n;
    casuale++;
    return casuale;
}

int main(int argc, char ** argv){
        /*-------------*variabili locali-------------*/
    typedef struct{
        int c1;                     //contiene pid nipote
        int c2;                      //contiene numero linea
        char c3[250];               //contiene linea letta(lunga al massimo 250)
    } s;
    
    int N;                          //contiene numero di file=numero di numeri
    int Fh;                         //contiene fd del file associato a ogni figlio
    int Xh;                         //contiene numero associato a ogni figlio
    int lineaScelta;                //contiene linea fino alla quale leggere il file(generata con la random)
    char comandoHead[5];            //- e poi al massimo 3 cifre che rappresentano lineaScelta (e poi terminatore di linea)
    char linea[250];                //contiene linea appena letta dal figlio (si suppone lunga al max 250)
    s struttura;                    //contiene struttura che il figlio passa al padre
    int *vpidNipote;                 //contiene pid del nipote
    int pipeLette;                  //contiene numero di pipe lette

    typedef int pipe_t[2];
    pipe_t *pipedFp;                //contiene pipe figlio->nipote
    pipe_t *pipedNf;                //contiene pipe nipote->figlio
    int nr, nw;                     //per controllare lettura e scrittura pipe
    int pidFiglio, pidNipote, status, ritorno; //per fork e wait
        /*-------------------------------------------*/     
    
    //controllo: almeno 2 parametri, numero pari
    N=argc-1;
    if(N<2 || (N%2)!=0){
        printf("Errore: passati %d parametri, ma serve un numero pari maggiore o uguale a 2\n", argc-1);
        exit (1);
    }
    N=N/2;
    //controllo: parametri passati sono numeri strettamente positivi
    for(int i=1; i<=N; ++i){
        Xh=atoi(argv[i*2]);
        if(Xh<=0){
            printf("Errore: il parametro %d non è un numero strettamente positov\n", Xh);
            exit (2);
        }
    }
    //controllo: file apribili in lettura verrà fatto dai processi nipoti

    //alloco spazio per array di pipe
    pipedFp= (pipe_t*) malloc(N*sizeof(pipe_t));
    if(pipedFp==NULL){
        printf("Errore: problemi nell'allocazione di memoria per le pipe pipedFp\n");
        exit (3);
    }
    pipedNf= (pipe_t*) malloc(N*sizeof(pipe_t));
    if(pipedNf==NULL){
        printf("Errore: problemi nell'allocazione di memoria per le pipe pipedNf\n");
        exit (4);
    }
    //alloco spazio per vpidNipote
    vpidNipote= (int*) malloc(N*sizeof(int));
    if(vpidNipote==NULL){
        printf("Errore: problemi nell'allocazione di memoria per l'array vpidNipote'\n");
        exit (5);
    }
    //creo le pipe
    for(int i=0; i<N; ++i){
         if(pipe(pipedFp[i])<0){
            printf("Errore: problemi nella creazione della pipe pipedFp\n");
            exit (6);
        }
    }
    for(int i=0; i<N; ++i){
         if(pipe(pipedNf[i])<0){
            printf("Errore: problemi nella creazione della pipe pipedNf\n");
            exit (7);
        }
    }

    //creo N processi figli
    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli\n", getpid(), N);
    for(int i=0; i<N; ++i){
        pidFiglio=fork();
        if(pidFiglio<0){
             printf("Errore nella fork\n");
             exit (8);
        }
         if(pidFiglio==0){
            printf("DEBUG-Sono il processo figlio con pid %d\n", getpid());
            //chiudo pipe figlio->padre (non serviranno nemmeno ai nipoti)
            //chiudo tutti lati di lettura, tutti lati di scrittura tranne il proprio
            for(int j=0; j<N; ++j){
                close(pipedFp[j][0]);
                if(j!=i){
                    close(pipedFp[j][1]);
                }
            }

            //creo processo nipote
            printf("DEBUG-Sono il processo figlio con pid %d e sto per creare 1 processo nipote\n", getpid());
            pidNipote=fork();
            vpidNipote[i]=pidNipote;
            if(pidNipote<0){
                printf("Errore nella fork\n");
                exit (-1);
            }
            if(pidNipote==0){
                printf("DEBUG-Sono il processo nipote con pid %d\n", getpid());
                //chiudo lati pipe: chiudo lato scrittura pipedFp[i][1]
                //chiudo lati di lettura, tutti i lati di scrittura tranne il mio
                for(int j=0; j<N; ++j){
                    close(pipedNf[j][0]);
                    if(j!=i){
                        close(pipedNf[j][1]);
                    }
                }
                close(pipedFp[i][1]);

                //inizializzo seme
                srand(time(NULL));
                //lunghezza in linee corrispondente al file su cui lavoro
                Xh=atoi(argv[2+(i*2)]);
                //uso funzione random
                lineaScelta=mia_random(Xh);

                //creo stringa per comando head
                sprintf(comandoHead, "-%d",lineaScelta);
                printf("DEBUG-%s\n", comandoHead);
                printf("DEBUG-Apro il file %s\n", argv[1+(i*2)]);
                //ridirezione: output=pipedNf
                close(1);
                dup(pipedNf[i][1]);
                close(pipedNf[i][1]);
                
                //provo a eseguire exec per comando head
                execlp("head", "head", comandoHead, argv[1+(i*2)],(char *)0);

                //nel caso in cui non esegua l'exec, c'è stato un probelma
                exit(-1);
            }
        
        //codice del figlio
        //chiude lati pipe di pipedNf: tutti i lati di scrittura, tutti lati di lettura tranne il proprio
        for(int j=0; j<N; ++j){
            close(pipedNf[j][1]);
            if(j!=i){
                close(pipedNf[j][0]);
            }
        }

        //finchè posso, leggo da pipedNf
        lineaScelta=1;
        int j=0;
        while(read(pipedNf[i][0], &struttura.c3[j], 1)>0){
            struttura.c1=vpidNipote[i];
            struttura.c2=lineaScelta;
            if(struttura.c3[j]=='\n'){
                struttura.c3[j]=0;
                nw=write(pipedFp[i][1], &struttura, sizeof(struttura));
                if(nw!=sizeof(struttura)){
                    printf("Errore: il figlio non riesce a scrivere sulla pipe pipedFp\n");
                    exit (-1);
                }
                ++lineaScelta;
                j=0;
            }
            else{
            ++j;
            }
        
        }
        //il figlio aspetta il nipote
        if ((pidNipote=wait(&status)) < 0){
            printf("Errore wait\n");
            exit(-1);
        }
        if ((status & 0xFF) != 0){
                 printf("Nipote con pid %d terminato in modo anomalo\n", pidFiglio);
        }
        else{
                 ritorno=(int)((status >> 8) & 0xFF);
                 printf("Il nipote con pid=%d ha ritornato %d (se 255 problemi!)\n", pidFiglio, ritorno);
        }
        
        exit (ritorno);
         }
    }
    //codice del padre
    //chiude lati pipe: tutti lati di scrittura
    for(int j=0; j<N; ++j){
        close(pipedFp[j][1]);
    }

    //finchè riesce a leggere qualcosa, per ogni figlio legge 1 struttura
    pipeLette=0;
    for(int i=0; i<N; ++i){
        nr=read(pipedFp[i][0], &struttura, sizeof(struttura));
        if(nr!=0){
            printf("Il nipote di pid %d ha letto la linea %d: %s\n", struttura.c1, struttura.c2, struttura.c3);
            pipeLette+=1;
        }
    }
    while(pipeLette>0){
        pipeLette=0;
        for(int i=0; i<N; ++i){
            nr=read(pipedFp[i][0], &struttura, sizeof(struttura));
            if(nr!=0){
                printf("Il nipote di pid %d ha letto la linea %d: %s\n", struttura.c1, struttura.c2, struttura.c3);
                pipeLette+=1;
            }
        }
    }

    //il padre aspetta tutti i processi figli
    for(int i=0; i<N; ++i){
         if ((pidFiglio=wait(&status)) < 0){
             printf("Errore wait\n");
             exit(9);
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


