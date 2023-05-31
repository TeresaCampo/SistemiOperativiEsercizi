#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#define MSGSIZE 6   //lunghezza di ogni linea(compreso terminatore di linea)

int main(int argc, char ** argv){
    /*-------------*variabili locali-------------*/
int N;                          //contiene numero di file
char linea[MSGSIZE];            //contiene linea letta dal file associato
char buffer[MSGSIZE];           //contiene linea passata dal figlio P0
int Fn;                         //contiene fd file associato al processo

typedef int pipe_t[2];
pipe_t *pipedF0f;               //pipe da figlio P0->gilio P(n>0)
int nr, nw;                     //per controllare lettura e scrittura pipe
int pidFiglio, pidNipote, status, ritorno; //per fork e wait
    /*-------------------------------------------*/

    //controllo: almeno 3 parametri
    if(argc<4){
        printf("Errore: passati %d parametri, ma ne servono almeno 3\n", argc-1);
        exit (1);
    }
    N=argc-1;
    //controllo: file apribili in lettura lo controllano i nipoti

    //alloco spazio pipe
    pipedF0f= (pipe_t*) malloc(N*sizeof(pipe_t));
    if(pipedF0f==NULL){
        printf("Errore: problemi nell'allocazione di memoria per le pipe pipedF0f\n");
        exit (3);
    }
    //creo pipe figlio P0->figlio P(n>0)
    for(int n=0; n<N; ++n){
         if(pipe(pipedF0f[n])<0){
            printf("Errore: problemi nella creazione della pipe pipedF0f\n");
            exit (4);
        }
    }

    //creo N figli
    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli\n", getpid(), N);
    for(int n=0; n<N; ++n){
        pidFiglio=fork();
        if(pidFiglio<0){
             printf("Errore nella fork\n");
             exit (5);
        }
         if(pidFiglio==0){
            printf("DEBUG-Sono il processo figlio con pid %d\n", getpid());
            //chiudo lati pipe:
            close(pipedF0f[0][0]);
            close(pipedF0f[0][n]);
                
            //cosa fa processo 0
            if(n==0){
                //chiudo pipe
                for(int j=1; j<N; ++j){
                    close(pipedF0f[j][0]);
                }
                
                //provo ad aprire file associato
                if((Fn=open(argv[1+n], O_RDONLY))<0){
                printf("Errore: impossibile aprire in lettura il file %s\n", argv[1+n]);
                exit (-1);
                }

                //leggo 1 riga alla volta
                while(1){
                    nr=read(Fn, linea, MSGSIZE);
                    if(nr!=MSGSIZE) break;

                    //trasformo linea in stringa
                    linea[MSGSIZE-1]=0;
                    printf("IL processo 0 ha letto %s\n", linea);
                    //mando la linea a tutti i fratelli
                    for(int j=1; j<N; ++j){
                        nw=write(pipedF0f[j][1], linea, MSGSIZE);
                        if(nw!=sizeof(linea)){
                            printf("Errore: il figlio 0 non riesce a scrivere sulla pipe pipedF0f\n");
                            exit (-1);
                        }
                    }
                }
                exit(n);
            }
            //ogni processo diverso dal primo
            else{
                //chiudo le pipe
                for(int j=1; j<N; ++j){
                    close(pipedF0f[j][1]);
                    if(j!=n){
                        close(pipedF0f[j][0]);
                    }
                }
                //apro il file associato
                    if((Fn=open(argv[1+n], O_RDONLY))<0){
                    printf("Errore: impossibile aprire in lettura il file %s\n", argv[1+n]);
                    exit (-1);
                }

                //finchè legge dalla pipe del figlio 0, confronta con ogni linea del proprio file
                nr=read(pipedF0f[n][0], buffer, MSGSIZE);
                if(nr!=MSGSIZE){
                    printf("Errore: il figlio %d non riesce a leggere dalla pipe pipedF0f\n", n);
                    exit (-1);
                }
                while(nr==MSGSIZE){
                    //mi sposto all'inizio del file
                    lseek(Fn, 0, 0);
                    //leggo ogni riga del file 
                    while(1){
                        nr=read(Fn, linea, MSGSIZE);
                        if(nr!=MSGSIZE) break;

                        //trasformo linea in stringa
                        linea[MSGSIZE-1]=0;
                        //per ogni coppia creo un processo nipote
                        printf("DEBUG-Processo %d controllo la coppia %s e %s\n",n, buffer, linea);
                        //creo processo nipote
                        pidNipote=fork();
                        if(pidNipote<0){
                            printf("Errore nella fork\n");
                            exit (-1);
                        }
                        if(pidNipote==0){
                            printf("DEBUG-Sono il processo nipote con pid %d e provo a eseguire exec\n", getpid());
                            //eseguo exec
                            //ridireziono standard output e standard error
                            close(1);
                            open("dev/null", O_WRONLY);
                            close(2);
                            open("dev/null", O_WRONLY);
                            execlp("diff", "diff", linea, buffer, (char*)0);

                            printf("Errore nell'exec del nipote di ordine %d\n", n);
                        }
                        //codice del figlio
                        //aspetta il nipote
                        //il padre aspetta tutti i processi figli
                        
                        if ((pidNipote=wait(&status)) < 0){
                             printf("Errore wait\n");
                             exit(-1);
                        }
                        if ((status & 0xFF) != 0){
                            printf("Nipote con pid %d terminato in modo anomalo\n", pidFiglio);}
                        else{
                            ritorno=(int)((status >> 8) & 0xFF);
                            if(ritorno==0){
                                printf("Il file %s e %s sono uguali\n", buffer, linea);
                            }
                            /*else{
                                printf("DEBUG-Il file %s e %s sono diversi\n", buffer, linea);
                            }
                            */
                        }
                    }
                    nr=read(pipedF0f[n][0], buffer, MSGSIZE);
                    printf("Il processo %d ha letto %s cioè %d caratteri\n", n, buffer, nr);
                }
            exit (n);
            }
        }
    }

    //codice del padre
    //chiudo lati pipe
    for(int j=0; j<N; ++j){
        close(pipedF0f[j][1]);
        close(pipedF0f[j][0]);
    }

    //il padre aspetta tutti i processi figli
    for(int n=0; n<N; ++n){
         if ((pidFiglio=wait(&status)) < 0){
             printf("Errore wait\n");
             exit(6);
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