#include <time.h>
#include <stdlib.h>
#define PERMESSI 666

int mia_random(int n){
int casuale;
casuale=rand()%n;
return casuale;
}

int main(int argc, char ** argv){
    /*--------------variabili locali-------------*/
int N;               //numero di file
int H;               //numero linee di ogni file(da non controllare)
typedef int pipe_t[2];
pipe_t *pipedPf;     //per contenere pipe da padre a figli
pipe_t *pipedFp;     //per contenere pipe da figli a padre
int Fcreato;        //fd del file tmp
int Fk1, Fk2;             //fd del file associato a ogni figlio
char ch1, ch2;            //contiene carattere letto dal file Fk
int nr, posPassataDaP;
int pidFiglio, status, ritorno; //per fork e wait
    /*------------------------------------------*/

//controllo: numero di parametri almeno 5
if(argc<6){
    printf("Errore: passati %d parametri, ma ne servono almeno 5\n");
    exit (1);
}
N=argc-2;
H=argv[argc-1];

//controllo: H deve essere positivo e minore di 255
if(H<=0 || H>=255){
    printf("Errore: H passato è %d, ma deve essere compreso tra 1 e 254\n");
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
    if(pipe(pipedPf[i])<0 || pipe(pipedFp[i]<0)){
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
            if((Fk1=open(argv[i+1], O_RDONLY ))<0 || (Fk2=open(argv[i+1], O_RDONLY ))<0){
            printf("Errore: impossibile aprire in lettura il file %s\n", argv[i+1]);
            exit (-1);
            }

            //leggo una riga alla volta e ne riporto la lunghezza sulla pipe
            int len=1;
            while(read(Fk1), &ch1, 1){
                if(ch1=='\n'){
                    write(pipedFp[i][1], &len, sizeof(int));
                    len=1;
                }
                else{
                    ++len;
                }
            }

            //se ricevo un valore sulla pipe pipedPf, cerco quel valore nella riga i
            int pos=0;
            nr=read(pipedPf[i][0], &ch2, sizeof(int));
            while(nr>0){
                while(read(Fk2, &ch2, 1)>0){
                    if(pos==posPassataDaP){
                        write(Fcreato, &ch2, 1);    //stampo nel file tmp
                        ++pos;
                    }
                    else if(ch2=='\n'){
                        pos=0;
                        break;
                    }
                    else{
                    ++pos;
                    }
                }
                nr=read(pipedPf[i][0], &ch2, sizeof(int));
            }

            exit(0);
       } 



    }

}