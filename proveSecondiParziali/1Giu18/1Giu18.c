#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

int main(int argc, char ** argv){
/*---------variabili locali--------------*/
int N;      //contiene il nuero di file passati
int K;      //contiene lunghezza media in linee del file
double checkK;  //per contenere il numerok e controllare se numero non intero
int X;          //per contenere numero scritto dall'utente
int Fa;         //contiene fd del file associato a ogni figlio
typedef int pipe_t[2];
pipe_t *pipedFp;        //per contenere pipe da figli a padre
int pidFiglio, status, ritorno; //per fork e wait
int *vpid;              //vettore che contiene i pid dei figli
char linea[255];        //si suppone len max di una riga 255 caratteri compreso il terminatore
int numLinea, pos;      //contiene linea appena letta dal file associato e la poszione nella liena che sto leggendo dal file associato
int L;                      //contiene la lunghezza della linea
/*---------------------------------------*/

//controllo: almeno 2 parametri
if(argc<4){
    printf("Errore: passati %d parametri, ma ne servono almeno 3\n", argc-1);
    exit (1);
}
N=argc-2;
K=atoi(argv[argc-1]);
checkK=atof(argv[argc-1]);

//controllo: K numero intero strettamente positivo
if(K<=0 || (checkK-K)>0){
    printf("Errore: il parametro %d non è un numero strettamente positivo o non è un numero intero\n", K);
    exit (2);
}

//il padre chiede all'utente di inserire un numero positivo e minore o ugualr a K
printf("Scrivere un numero strettamete positivo e minore o uguale a %d\n", K);
scanf("%d", &X);

if(X<=0 || X>K){
    printf("Errore: scrivere un numero che rispetti i parametri, %d non è compreso tra 1 e %d\n", X, K);
    exit(3);
}

printf("Sono il processo padre e sto per creare %d pipe\n", N);
//il padre crea N pipe figlio-padre
if((vpid=malloc(N*sizeof(int)))==NULL){
   printf("Errore: problemi allocazione vpid\n"); 
   exit (4);
}
if((pipedFp=malloc(N*sizeof(pipe_t)))==NULL){
    printf("Errore: problemi allocazione di pipedFp\n"); 
    exit(5);
}


for(int i=0; i<N; ++i){
    if(pipe(pipedFp[i])<0){
        printf("Errore: problemi nella creazione della pipe\n");
        exit (6);
    }
}

//il padre crea N figli
for(int i=0; i<N; ++i){
    pidFiglio=fork();
    vpid[i]=pidFiglio;
        if(pidFiglio<0){
        printf("Errore: problemi nella fork\n");
        exit (7);
        }
        if(pidFiglio==0){
            printf("Sono il processo figlio di pid %d e sto per cercare la linea %d nel file %s\n", getpid(), X, argv[i+1]);
            //il figlio chiude lati lettura e tutti-1 lati scrittura
            for(int j=0; j<N; ++j){
                close(pipedFp[j][0]);
                if(i!=j) {
                    close(pipedFp[j][1]);
                }
            }

            //figlio prova ad aprire il file associato
            if((Fa=open(argv[i+1], O_RDONLY ))<0 ){
            printf("Errore: impossibile aprire in lettura il file %s\n", argv[i+1]);
            exit (-1);
            }

            //il figlio legge il file Fa fino alla linea X, se esiste
            pos=0; numLinea=1;
            while(read(Fa, &linea[pos], 1)>0){
                if(numLinea>X) {
                    L=0; linea[0]=0;
                    write(pipedFp[i][1], &L, sizeof(int));
                    write(pipedFp[i][1], linea, 255);
                    break;
                }
                if(linea[pos]=='\n'){
                    if(numLinea==X){
                        //manda la lunghezza dell alinea(compreso terminatore) e la linea
                        L=pos+1;
                        write(pipedFp[i][1], &L, sizeof(int));
                        write(pipedFp[i][1], linea, 255);
                        break;
                    }
                    ++numLinea; //numero linea che sto per leggere
                    pos=0;
                }
                else{
                    ++pos;
                }
            }
            exit(L);
        }
}
//il padre chiude le pipe in scrittura
for(int i=0; i<N; ++i){
    close(pipedFp[i][1]);
}

//il padre legge la linea del primo file(primo figlio), poi quella del secondo...
for(int i=0; i<N; ++i){
    read(pipedFp[i][0], &L, sizeof(int));
    read(pipedFp[i][0], linea, L);
    linea[L-1]=0;

    if(L!=0){
    printf("Il figlio di pid %d ha selezionato dal file %s la linea %d: %s\n", vpid[i], argv[i+1], X, linea);
    }
}

//il padre aspetta i figli
//il padre aspetta tutti i figli
    for(int i=0; i<N; ++i){
        if ((pidFiglio=wait(&status)) < 0)
        {
                printf("Errore wait\n");
                exit(8);
        }
        if ((status & 0xFF) != 0)
                printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
        else
        {
                ritorno=(int)((status >> 8) & 0xFF);
                printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi!)\n", pidFiglio, ritorno);
        }
    }
    exit (0);   
}