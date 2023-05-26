#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <ctype.h>

int main(int argc, char ** argv){
    /*-------------*variabili locali-------------*/
int N;                          //numero di file passati
typedef int pipe_t[2];
pipe_t *pipedFp;                //per contenere pipe figlio-padre
pipe_t *pipedNp;                //per contenere pipe nipote-padre
int pidFiglio, pidNipote, status, ritorno; //per fork e wait
int nr, nw;                     //per controllare lettura pipe
int Ff;                         //per contenere fd del file associato al processo
char chLetto;                   //per contenere carattere appena letto
long int modificato;            //per conenere il numero di caratteri modificati da figli/ nipoti
    /*-------------------------------------------*/

//controllo: almeno 2 parametri
if(argc<3){
    printf("Errore: passati %d parametri, ma ne servono almeno 2\n", argc-1);
    exit (1);
}
N=argc-1;

//alloco spazio per le 2 pipe
pipedFp= (pipe_t*) malloc(N*sizeof(pipe_t));
pipedNp= (pipe_t*) malloc(N*sizeof(pipe_t));
if(pipedFp==NULL || pipedNp==NULL){
    printf("Errore: problemi nell'allocazione di memoria per le pipe\n");
    exit (2);
}

//creo le pipe
for(int i=0; i<N; ++i){
    if(pipe(pipedFp[i])<0 || pipe(pipedNp[i])<0){
        printf("Errore: problemi nella creazione della pipe\n");
        exit (3);
    }
}

//il padre crea N figli
printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli\n", getpid(), N);
for(int i=0; i<N; ++i){
    pidFiglio=fork();
    if(pidFiglio<0){
        printf("Errore nella fork\n");
        exit (-1);
    }
    if(pidFiglio==0){
        //il processo figlio crea 1 processo nipote
        pidNipote=fork();
        if(pidNipote<0){
            printf("Errore nella fork\n");
            exit (-1);
            }
        if(pidNipote==0){
            //il processo nipote chiude i lati della pipe
            //pipedFp chiude tutto
            //pipedNp chiude tutti i lati di lettura, tutti i lati di scrittura tranne il suo
                for(int j=0; j<N; ++j){
                  close(pipedNp[j][0]); close(pipedFp[j][0]); close(pipedFp[j][1]);
                    if(j!=i){
                        close(pipedNp[j][1]);  
                    }
                }

                //il nipote prova ad aprire il file
                if((Ff=open(argv[i+1], O_RDWR))<0){
                printf("Errore: impossibile aprire in lettura e scrittura il file %s\n", argv[i+1]);
                exit (-1);
                } 

                modificato=0;
                while(read(Ff, &chLetto, 1)>0){
                    if(isalpha(chLetto)!=0 && islower(chLetto)!=0){
                        lseek(Ff, -1, SEEK_CUR);
                        chLetto+=('A'-'a');
                        nw=write(Ff, &chLetto, 1);
                        if(nw!=1){
                            printf("Errore: il nipote non riesce a scrivere nel file %s\n", argv[i+1]);
                            exit (-1);
                        }
                            ++modificato;
                    }
                }
                //il nipote comunica al padre il numero di caratteri modificati
                nw=write(pipedNp[i][1], &modificato, sizeof(modificato));
                if(nw!=sizeof(modificato)){
                    printf("Errore: il figlio non riesce a scrivere nella pipe %s\n", argv[i+1]);
                    exit (-1);
                }

                //il nipote termina
                long int max=1; int c;
                for(c=1; ; ++c){
                    max=c*(256);
                    if(modificato<max){
                        --c;
                        break;
                    }
                }
                exit (c);
        }

        //il processo figlio chiude i lati della pipe: 
        //pipedFp chiude tutti i lati di lettura, tutti i lati di scrittura tranne il suo
        for(int j=0; j<N; ++j){
            close(pipedFp[j][0]);   close(pipedNp[j][0]);   close(pipedNp[j][1]); 
            if(j!=i){
                close(pipedFp[j][1]); 
            }
        }

        //il figlio prova ad aprire il file
        if((Ff=open(argv[i+1], O_RDWR))<0){
            printf("Errore: impossibile aprire in lettura e scrittura il file %s\n", argv[i+1]);
            exit (-1);
        } 

        modificato=0;
        while(read(Ff, &chLetto, 1)>0){
            if(isdigit(chLetto)!=0){
                lseek(Ff, -1, SEEK_CUR);
                chLetto=' ';
                nw=write(Ff, &chLetto, 1);
                if(nw!=1){
                    printf("Errore: il figlio non riesce a scrivere nel file %s\n", argv[i+1]);
                    exit (-1);
                    }
                ++modificato;
                }
        }
        //il figlio comunica al padre il numero di caratteri modificati
        nw=write(pipedFp[i][1], &modificato, sizeof(modificato));
        if(nw!=sizeof(modificato)){
            printf("Errore: il figlio non riesce a scrivere nella pipe %s\n", argv[i+1]);
            exit (-1);
        }
        
        //il figlio termina
        long int max=1; int c;
        for(c=1; ; ++c){
            max=c*(256);
            if(modificato<max){
                --c;
                break;
            }
        }
        exit (c);
    }
}

//il padre chiude i lati di lettura delle pipe: tutti i lati in scrittura
for(int j=0; j<N; ++j){
    close(pipedFp[j][1]); close(pipedNp[j][1]); 
}

//il padre legge dal 1 figlio e 1 nipote, 2,3, ...
for(int i=0; i<N; ++i){
    nr=read(pipedFp[i][0], &modificato, sizeof(modificato));
    if(nr!=sizeof(modificato)){
        printf("Errore: il padre non riesce a leggere dalla pipe\n");
        exit (-1);
    }
    printf("Il processo figlio %d ha lavorato sul file %s. Ha sostituito %ld caratteri numerici con spazi\n", i, argv[i+1], modificato);
    nr=read(pipedNp[i][0], &modificato, sizeof(modificato));
    if(nr!=sizeof(modificato)){
        printf("Errore: il padre non riesce a leggere dalla pipe\n");
        exit (-1);
    }
    printf("Il processo nipote %d ha lavorato sul file %s. Ha sostituito %ld caratteri minuscoli con i rispettivi maiuscoli\n", i, argv[i+1], modificato); 
}

//il padre aspetta tutti i processi figli
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