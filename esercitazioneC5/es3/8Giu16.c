#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

#define PERMESSI 0644

int mia_random(int n){
int casuale;
casuale=rand()%n;
return casuale;
}

int main(int argc, char ** argv){
    /*--------------variabili locali-------------*/
int N;                  //numero di file
int H;                  //numero linee di ogni file(da non controllare)
typedef int pipe_t[2];
pipe_t *pipedPf;        //per contenere pipe da padre a figli
pipe_t *pipedFp;        //per contenere pipe da figli a padre
int Fcreato;            //fd del file tmp
int Fk1, scritti;                //fd del file associato a ogni figlio
char rigaLetta[255];    //contiene caratteri della riga appena letta, supponiamo lunghezza max riga 255(terminatore compreso)
int lenScelta, lenLetta,figlioScelto, pos;          //contiene la lunghezza scelta dal padre, quella letta da ogni pipe
int pidFiglio, status, ritorno; //per fork e wait
    /*------------------------------------------*/

//controllo: numero di parametri almeno 5
if(argc<6){
    printf("Errore: passati %d parametri, ma ne servono almeno 5\n", argc-1);
    exit (1);
}
N=argc-2;
H=atoi(argv[argc-1]);

//controllo: H deve essere positivo e minore di 255
if(H<=0 || H>=255){
    printf("Errore: H passato è %d, ma deve essere compreso tra 1 e 254\n", H);
    exit (2);
}

//inizializzo il seme della random
srand(time(NULL));

//creo il file temporaneo /tmp/creato
Fcreato=creat("/tmp/creato", PERMESSI);
if(Fcreato<0){
    printf("Errore: problemi nella creazione del file tmp\n");
    exit (3);
}

//creo N*2 pipe
pipedPf=malloc(N*sizeof(pipe_t));
pipedFp=malloc(N*sizeof(pipe_t));
for(int i=0; i<N; ++i){
    if(pipe(pipedPf[i])<0 || pipe(pipedFp[i])<0){
        printf("Errore: problemi nella creazione della pipe\n");
        exit (2);
    }
}

//creo N processi figlio
for(int i=0; i<N; ++i){
    pidFiglio=fork();
        if(pidFiglio<0){
        printf("Errore: problemi nella fork\n");
        exit (3);
        }
        if(pidFiglio==0){
            //chiudo i lati della pipe
            for(int j=0; j<N; ++j){
                close(pipedFp[j][0]);   close(pipedPf[j][1]);
                if(i!=j) {
                    close(pipedFp[j][1]);
                    close(pipedPf[j][0]);
                }
            }

            //controllo che il parametro sia un file
            if((Fk1=open(argv[i+1], O_RDONLY ))<0 ){
            printf("Errore: impossibile aprire in lettura il file %s\n", argv[i+1]);
            exit (-1);
            }

            //leggo una riga alla volta e ne riporto la lunghezza sulla pipe
            int len=1;  int j=0;    scritti=0;
            while(read(Fk1, &rigaLetta[j], 1)){
                if(rigaLetta[j]=='\n'){
                    write(pipedFp[i][1], &len, sizeof(int));
                    read(pipedPf[i][0], &lenScelta, sizeof(int));

                    if(lenScelta<len){
                        write(Fcreato, &rigaLetta[lenScelta], 1);
                        ++scritti;
                    }
                    len=1;  j=0;
                }
                else{
                    ++len;  ++j;
                }
            }

            exit(scritti);
       } 
    }

    //il padre chiude i lati delle pipe che non usa
    for(int i=0; i<N; ++i){
        close(pipedFp[i][1]);   close(pipedPf[i][0]);
    }

    //il padre per ogni linea dei file deve:
        //leggere la i-esima riga di tutti i file, generare un numero random e selezionare 1 di valori letti
        //generare un numero random con il valore selezionato e scriverlo sulla pipe pipedPf
    for(int i=0; i<H; ++i){
        //il padre genera un numero random tra 0 e N-1, cioè uno dei figli
        figlioScelto=mia_random(N);
        printf("scelgo linea %d\n", figlioScelto);
        for(int j=0; j<N; ++j){
            read(pipedFp[j][0], &lenLetta, sizeof(int));
            printf("lunghezza linea %d del figlio %i\n", lenLetta, j);
            if(j==figlioScelto){
                pos=mia_random(lenLetta);
                printf("scelgo posizione %d\n", pos);
                for(int k=0; k<N; ++k){
                    write(pipedPf[k][1], &pos, sizeof(int));
                }
            }
        }
    }

    //il padre aspetta tutti i figli
    for(int i=0; i<N; ++i){
        if ((pidFiglio=wait(&status)) < 0)
        {
                printf("Errore wait\n");
                exit(5);
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