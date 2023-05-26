#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <ctype.h>

int main(int argc, char ** argv){

      /*-------------*variabili locali-------------*/
int Q;                          //contiene il numero di file
int q;                          //indice processi figli
char linea[255];                //contiene linea letta supponendo max lunghezza 255 caratteri
typedef int pipe_t[2];
pipe_t pipedPf0;                //per contenere pipe da padre a figlio0
pipe_t *pipedFf;                //per contenere pipe da un figlio a successivo
int pidFiglio, status, ritorno; //per fork e wait
int Fq;                         //contiene fd del file associato al processo
int pos;                        //posizione dentro la linea che sto leggendo
int numeriTrovati;              //numero numeri trovati nella riga che sto leggendo
int nrFf, nrPf;                 //contiene numero di caratteri letti da pipe del padre o di un altro figlio
char letto;                     //contiene segnale di sincronizzazioen appnea letto
    /*-------------------------------------------*/

//controllo: almeno 2 parametri
if(argc<3){
    printf("Errore: passati %d parametri, ma ne servono almeno 2\n", argc-1);
    exit (1);
}
Q=argc-1;

//il padre alloca memoria per pipedFf
if((pipedFf=malloc(Q*sizeof(pipe_t)))<0){
    printf("Errore: problemi nell'allocazione di memoria per pipedFf\n");
    exit (2);
}

//il padre crea pipedFf e pipedPf0
for(int i=0; i<Q; ++i){
    if(pipe(pipedFf[i])<0){
        printf("Errore: problemi nella creazione della pipeFf\n");
        exit (3);
    }
}
if(pipe(pipedPf0)<0){
        printf("Errore: problemi nella creazione della pipePf0\n");
        exit (4);
}

//il padre crea i Q figli
for(q=0; q<Q; ++q){
    pidFiglio=fork();
        if(pidFiglio<0){
        printf("Errore: problemi nella fork\n");
        exit (3);
        }
        if(pidFiglio==0){
            //il figlio chiude i lati della pipe: tutti quelli di lettura tranne q-1(se q-1==0 allore non chiude Q-1)
            //tutti i lati di scittura tranne q
            for(int j=0; j<Q; ++j){
                if(j!=q-1 && (q-1)>=0){
                    close(pipedFf[j][0]);
                }
                if(j!=(Q-1) && (q-1)==0){
                    close(pipedFf[j][0]);
                }
                if(q!=j) {
                    close(pipedFf[j][1]);
                }
            }
            //il figlio 0 chiude lato di scrittura pipedPf0
            if(q==0){
                close(pipedPf0[1]);
            }

            //il figlio prova ad aprire il file associato
            if((Fq=open(argv[q+1], O_RDONLY ))<0 ){
            printf("Errore: impossibile aprire in lettura il file %s\n", argv[q+1]);
            exit (-1);
            }

            pos=0; numeriTrovati=0;
            while(read(Fq, &linea[pos], 1)>0){
                if(isdigit(linea[pos])!=0){
                    ++numeriTrovati;
                }
                if(linea[pos]=='\n'){
                    if(q==0){
                        nrFf=read(pipedFf[Q-1][0], &letto, 1);
                        nrPf=read(pipedPf0[0], &letto, 1);
                        if(nrFf>0 || nrPf> 0){
                            linea[pos]=0;
                            printf("Il processo %d ha trovato %d caratteri numerici nella linea %s\n", getpid(), numeriTrovati, linea);
                    
                        }
                    }
                    else{
                        nrFf=read(pipedFf[q-1][0], &letto, 1);
                        if(nrFf>0){
                            linea[pos]=0;
                            printf("Il processo %d ha trovato %d caratteri numerici nella linea %s\n", getpid(), numeriTrovati, linea);
                        }

                    }
                    pos=0;   numeriTrovati=0; 
                }
            }
            exit (numeriTrovati);
        }
}

write(pipedPf0[1], "v", 1);

//il padre aspetta tutti i figli
    for(q=0; q<Q; ++q){
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