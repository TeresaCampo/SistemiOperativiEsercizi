#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#define PERM 0644

int main(int argc, char ** argv){
    /*-------------*variabili locali-------------*/
int N;                          //contiene numero di processi figli
int nroLinee;                    //contiene numero linee di ogni file
typedef char l[250];            //linea(si suppone max 250 caratteri compreso il temrinatore))
l linea;                        //contiene linea appena letta da ogni processo
l *tutteLinee;                  //contiene N linee
int pos;                        //contiene pos della line ain cui scrivere carattere appena letto dal file
int fcreato;                    //contiene fd file creato CAMPO
int Fn;                         //contiene fd file associato a ogni processo

typedef int pipe_t[2];
pipe_t *pipedFf;                //contiene pipe figlio(n)->figlio(n+1)/ padre
int nr, nw;                     //per controllare lettura e scrittura pipe
int pidFiglio, status, ritorno; //per fork e wait
    /*-------------------------------------------*/         

    //controllo: almeno 2 parametri
    if(argc<3){
        printf("Errore: passati %d parametri, ma ne servono almeno 2\n", argc-1);
        exit (1);
    }
    N=argc-2;
    //controllo: nroLinee strettamente positivo
    nroLinee=atoi(argv[argc-1]);
    if(nroLinee<=0){
        printf("Errore: il parametro %d dovrebbe essere strettamente poositivo\n", nroLinee);
        exit (2);
    }

    //il padre crea il file "CAMPO"
    fcreato=creat("CAMPO", PERM);
    if(fcreato<0){
    printf("Errore: il file %s non è stato creato correttamente\n", "CAMPO");
    exit(3);
    }

    //alloco N pipe
    pipedFf= (pipe_t*) malloc(N*sizeof(pipe_t));
    if(pipedFf==NULL){
        printf("Errore: problemi nell'allocazione di memoria per le pipe pipedFf\n");
        exit (4);
    }
    //alloco tutteLinee
    tutteLinee= (l*) malloc(N*sizeof(l));
    if(tutteLinee==NULL){
        printf("Errore: problemi nell'allocazione di memoria per l'array tutteLinee\n");
        exit (5);
    }

    //creo N pipe
    for(int n=0; n<N; ++n){
         if(pipe(pipedFf[n])<0){
            printf("Errore: problemi nella creazione della pipe pipedFf\n");
            exit (6);
        }
    }

    //creo N figli
    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli\n", getpid(), N);
    for(int n=0; n<N; ++n){
        pidFiglio=fork();
        if(pidFiglio<0){
             printf("Errore nella fork\n");
             exit (7);
        }
        if(pidFiglio==0){
            printf("DEBUG-Sono il processo figlio con pid %d\n", getpid());
            //chiudo lati pipe: scrivo su pipedFf(i), leggo su pipedFf(i-1)
            for(int j=0; j<N; ++j){
                if(j!=n){
                    close(pipedFf[j][1]);
                }
                if(j!=(n-1) ){
                    close(pipedFf[j][0]);
                }
            }

            //apro file associato
            if((Fn=open(argv[1+n], O_RDONLY))<0){
            printf("Errore: impossibile aprire in lettura il file %s\n", argv[1+n]);
            exit (-1);
            }

            //leggo riga per riga
            pos=0;
            while(read(Fn, &linea[pos], 1)>0){
                if(linea[pos]=='\n'){
                    //metto terminatore di linea
                    linea[pos+1]=0;

                    //i processi diversi dal primo aspettano tutteLinee dal processo prima
                    if(n!=0){
                        nr=read(pipedFf[n-1][0], tutteLinee, N*sizeof(l));
                        if(nr!=N*sizeof(l)){
                            printf("Errore: il figlio %d non riesce a leggere dalla pipe pipedFf\n", n);
                            exit (-1);
                        }
                        printf("Sono il processo figlio %d e ho letto dalla pipe %d e tutteLinee[%d] è %s\n", n, n-1, n-1, tutteLinee[n-1]);
                    }

                    //copio la riga appena letta nella posizione i dell'array
                    for(int i=0; i<250; ++i){
                        tutteLinee[n][i]=linea[i];
                    }

                    //passo l'array al prossimo figlio
                    nw=write(pipedFf[n][1], tutteLinee, N*sizeof(l));
                    if(nw!=N*sizeof(l)){
                         printf("Errore: il figlio non riesce a scrivere sulla pipe pipedFf\n");
                        exit (-1);
                    }
                    printf("Sono il processo figlio %d e ho scritto sulla pipe %d\n", n, n);

                    //incomincio a leggere la nuova riga
                    pos=0;
                }
                else{
                    ++pos;
                }
            }
            exit(pos+1);
        }
    }

    //coidce del padre
    //chiudo lati pipe: legge da pipedFf[N-1]
    for(int j=0; j<N; ++j){
        close(pipedFf[j][1]);
        if(j!=(N-1)){
            close(pipedFf[j][0]);
        }
    }

    //per ognuna delle nroLinee legge ciò  gli manda l'ultimo figlio
    for(int n=0; n<nroLinee; ++n){
        nr=read(pipedFf[N-1][0], tutteLinee, N*sizeof(l));
        if(nr!=N*sizeof(l)){
            printf("Errore: il padre non riesce a leggere dalla pipe pipedFf\n");
            exit (8);
        }
        //scrive le linee sul file "CAMPO"
        for(int i=0; i<N; ++i){
            for(int k=0; k<250; ++k){
                if(tutteLinee[i][k]==0) break;
                nw=write(fcreato, &tutteLinee[i][k], 1);
                if(nw!=1){
                    printf("Errore: il padre non riesce a scrivere sul file CAMPO\n");
                exit (9);
                }
            }
        }
    }

    //il padre aspetta tutti i processi figli
    for(int n=0; n<N; ++n){
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
