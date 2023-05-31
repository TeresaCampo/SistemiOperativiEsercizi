#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <ctype.h>

int main(int argc, char **argv){
        /*-------------*variabili locali-------------*/
    typedef struct{
        int c1;                     //contiene indice d'ordine dle processo
        int c2;                     //contiene carattere numero Cn trovato nel file(da char->int)
    } struttura;
    bool trovato;                   //contiene indicazione, se ho trovato un carattere o no
    char Cn;                        //contiene carattere numerico
    int N;                          //contiene il numero di file passati come parametri
    int Fj;                         //contiene fd del file associato a ogni figlio
    struttura *vstrutture;          //contiene N strutture
    int *vpid;                      //contiene i pid dei processi figli
    long int somma;                 //contiene la somma dei caratteri Cn ritornati dai figli

    typedef int pipe_t[2];
    pipe_t *pipedFf;                //contiene pipe da figlio->figlio successivo(l'ultimo è tra figlio->padre)
    int nr, nw;                     //per controllare lettura e scrittura pipe
    int pidFiglio, status, ritorno; //per fork e wait
        /*-------------------------------------------*/

    //controllo: almeno 1 parametro
    if(argc<2){
        printf("Errore: passati %d parametri, ma ne servealmeno 1\n", argc-1);
        exit (1);
    }
    N=argc-1;
    //controllo: file apribile in lettura lo farnno i figli

    //alloco spazio per pipe
    pipedFf= (pipe_t*) malloc(N*sizeof(pipe_t));
    if(pipedFf==NULL){
        printf("Errore: problemi nell'allocazione di memoria per le pipe pipedFf\n");
        exit (2);
    }

    //alloco spazio per vstrutture
    vstrutture= (struttura*) malloc(N*sizeof(struttura));
    if(vstrutture==NULL){
        printf("Errore: problemi nell'allocazione di memoria per l'array vstrutture\n");
        exit (3);
    }
    //alloco spazioe per vpid
    vpid= (int*) malloc(N*sizeof(int));
    if(vpid==NULL){
        printf("Errore: problemi nell'allocazione di memoria per l'array vpid\n");
        exit (4);
    }

    //creo N pipe
    for(int i=0; i<N; ++i){
         if(pipe(pipedFf[i])<0){
            printf("Errore: problemi nella creazione della pipe pipedFf\n");
            exit (5);
        }
    }

    //creo N figli
    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli\n", getpid(), N);
    for(int i=0; i<N; ++i){
        pidFiglio=fork();
        vpid[i]=pidFiglio;
        if(pidFiglio<0){
             printf("Errore nella fork\n");
             exit (6);
        }
         if(pidFiglio==0){
            printf("DEBUG-Sono il processo figlio con pid %d\n", getpid());
            //chiudo lati pipe:
            //figlio scrive su pipedFf[i], legge da pipedFf[i-1]
            for(int j=0; j<N; ++j){
                if(j!=i){
                    close(pipedFf[j][1]);
                }
                if(j!=(i-1)){
                    close(pipedFf[j][0]);
                }
            }

            //provo ad aprire in lettura il file associato
            if((Fj=open(argv[i+1], O_RDONLY))<0){
            printf("Errore: impossibile aprire in lettura il file %s\n", argv[i+1]);
            exit (-1);
            }

            //leggo il file finchè trovo il primo carattere numerico
            trovato=false;
            while(read(Fj, &Cn, 1)>0){
                if(isdigit(Cn)!=0){
                    trovato=true;
                    break;
                }
            }

            //se non sono il processo 0, leggo dal processo prima vstrutture
            if(i!=0){
                nr=read(pipedFf[i-1][0], vstrutture, sizeof(struttura)*i);
                if(nr!=(sizeof(struttura*)*i)){
                    printf("Errore: il figlio non riesce a leggere dalla pipe pipedFf\n");
                    exit (-1);
                }
            }
           
            //setta la propria struttura nel vstrutture
            if(trovato==false){
                vstrutture[i].c2=-1;
            }
            else{
                vstrutture[i].c2=atoi(&Cn);
            }
            vstrutture[i].c1=i;

            //comunico la struttura al processo seguente
            nw=write(pipedFf[i][1], vstrutture, (i+1)*sizeof(struttura));
            if(nw!=((i+1)*sizeof(struttura))){
                 printf("Errore: il figlio non riesce a scrivere sulla pipe pipedFf\n");
            exit (-1);
            }
            exit(Cn);
        }
    }

    //codice del processo padre
    //chiude lati pipe: tutti i lati di lettura tranne quello di N-1 e tutti i lati di scrittura
    for(int j=0; j<N; ++j){
        close(pipedFf[j][1]);
        if(j!=(N-1)){
             close(pipedFf[j][0]);
        }
    }

    //il padre riceve vstrutture
    nr=read(pipedFf[N-1][0], vstrutture, sizeof(struttura)*N);
    if(nr!=sizeof(struttura)*N){
        printf("Errore: il padre non riesce a leggere dalla pipe pipedFf\n");
        exit (7);
    }

    somma=0;
    //il padre scrive i dati relativi a ogni struttura e il pid del processo corrispondente
    for(int i=0; i<N; ++i){
        printf("DEBUG-Il figlio %d ha scritto nella propria struttura %d\n", i, vstrutture[i].c2);
        if(vstrutture[i].c2==-1){
            printf("Il figlio di indice d'ordine %d e pid %d non ha letto numeri dal file %s\n", i, vpid[i],argv[i+1]);
        }
        else{
        printf("Il figlio di indice d'ordine %d e pid %d ha letto il numero %d dal file %s\n", i, vpid[i], vstrutture[i].c2,argv[i+1]);
        somma+=vstrutture[i].c2;
        }
    }
    //il padre scrive la somma dei valori riportati dai figli
    printf("La somma dei caratteri numerici Cn trovati dai figli è %ld\n", somma);

    //il padre aspetta tutti i processi figli
    for(int i=0; i<N; ++i){
         if ((pidFiglio=wait(&status)) < 0){
             printf("Errore wait\n");
             exit(8);
        }
         if ((status & 0xFF) != 0){
             printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);}
         else{
             ritorno=(int)((status >> 8) & 0xFF);
             printf("Il figlio con pid=%d ha ritornato %c (%d se 255 problemi!)\n", pidFiglio, ritorno, ritorno);
         }
    }
    exit (0);  
}