#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char ** argv){
        /*-------------*variabili locali-------------*/
    int N;      //contiene numero di file passati
    int Cx;     //contiene singolo carattere(ultimo parametro)
    int FK;     //contiene fd file associato a ogni processo figlio
    char chLetto;   //contiene carattere letto dal file associato
    long int pos;   //contiene posizione in cui trovo carattere nel file associato. LA PRIMA POSIZIONE E' 1   
    char indicazione;   //contiene indicazione data dal padre: carattere con cui sostituire oppure 0=non fare nulla
    int sostituito;     //contiene il numero di sostituzioni fatte da un figlio (si suppone minore di 255)
    int ricevute;     //contiene il numero di pipe da cui il padre ha letto un valore

    typedef int pipe_t[2];
    pipe_t *pipedFp;                //per comunicazione figlio->padre
    pipe_t *pipedPf;                //per comunicazione padre->figlio
    int nr, nw;                     //per controllare lettura e scrittura pipe
    int pidFiglio, status, ritorno; //per fork e wait
        /*-------------------------------------------*/
    
    //controllo: lamneo 2 parametri
    if(argc<3){
    printf("Errore: passati %d parametri, ma ne servono almeno 2\n", argc-1);
    exit (1);
    }
    N=argc-2;
    //controllo: ultimo parametro deve essere un singolo carattere
    if(strlen(argv[argc-1])!=1){
        printf("Errore: l'ultimo parametro %s non è un singolo carattere\n", argv[argc-1]);
        exit (2);
    }
    Cx=*argv[argc-1];
    //la lunghezza di tutti i file è uguale(da non controllare)

    //il padre alloca spazio per le 2*N pipe
    pipedFp= (pipe_t*) malloc(N*sizeof(pipe_t));
    if(pipedFp==NULL){
         printf("Errore: problemi nell'allocazione di memoria per le pipe pipedFp\n");
        exit (3);
    }
    pipedPf= (pipe_t*) malloc(N*sizeof(pipe_t));
    if(pipedPf==NULL){
         printf("Errore: problemi nell'allocazione di memoria per le pipe pipedPf\n");
        exit (4);
    }

    //il padre crea le 2*N pipe
    for(int i=0; i<N; ++i){
         if(pipe(pipedFp[i])<0){
            printf("Errore: problemi nella creazione della pipe pipedFp\n");
            exit (5);
        }
    }
    for(int i=0; i<N; ++i){
         if(pipe(pipedPf[i])<0){
            printf("Errore: problemi nella creazione della pipe pipedPf\n");
            exit (6);
        }
    }

    //il padre crea gli N figli
    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli\n", getpid(), N);
    for(int i=0; i<N; ++i){
        pidFiglio=fork();
        if(pidFiglio<0){
             printf("Errore nella fork\n");
             exit (7);
        }
         if(pidFiglio==0){
            printf("DEBUG-Sono il processo figlio con pid %d\n", getpid());
            //chiudo pipePf: tutti i lati di scrittura, tutti i lati di lettura tranne il mio
            //chiudo pipeFp: tutti i lati di scrittura tranne il mio, tutti i lati di lettura
            for(int j=0; j<N; ++j){
                close(pipedFp[j][0]); close(pipedPf[j][1]);
                if(j!=i){
                    close(pipedFp[j][1]);  close(pipedPf[j][0]);
                }
            }

            //provo ad aprire il file associato
            if((FK=open(argv[i+1],  O_RDWR))<0){
            printf("Errore: impossibile aprire in lettura e scrittura il file %s\n", argv[i+1]);
            exit (-1);
            }

            pos=1;  sostituito=0;
            while(read(FK, &chLetto, 1)>0){
                //se trova il carattere Cx
                if(chLetto==Cx){
                    //comunica al padre la posizione in cui lo ha trovato
                    printf("DEBUG-Trovato Cx alla posizione %ld di %s\n", pos, argv[i+1]);
                    nw=write(pipedFp[i][1], &pos, sizeof(pos));
                    if(nw!=sizeof(pos)){
                        printf("Errore: il figlio non riesce a scrivere sulla pipe pipedFp\n");
                        exit (-1);
                    }

                    //legge dal padre l'indicazione per sapere se lasciarlo così o se sostituirlo
                    nr=read(pipedPf[i][0], &indicazione, sizeof(indicazione));
                    if(nr!=sizeof(indicazione)){
                         printf("Errore: il figlio non riesce a leggere dalla pipe pipedPf\n");
                    exit (-1);
                    }
                    //se il padre tornza zero-> lasciare così
                    if(indicazione=='\n'){
                        ++pos;
                        continue;
                    }
                    //altrimenti sostituire con il valore tornato dal padre
                    else{
                        lseek(FK, -1, SEEK_CUR);
                        nw=write(FK, &indicazione, sizeof(indicazione));
                        if(nw!=sizeof(indicazione)){
                            printf("Errore: il figlio non riesce a scrivere sul file %s\n", argv[i+1]);
                            exit (-1);
                        }
                        ++sostituito;
                        ++pos;
                    }
                }
                //se non è il carattere Cx, continuo a leggere
                else{
                    ++pos;
                }
            }
            exit (sostituito);
        }
    }
    //codice del padre

    //il padre chiude le pipe
    //pipedFp: tutti i lati di scrittura, pipedPf: tutti i lati di lettura
    for(int i=0; i<N; ++i){
        close(pipedFp[i][1]); close(pipedPf[i][0]);
    }

    ricevute=1; //così controlla la prima volta
    while(ricevute>0){
        ricevute=0;
        //legge posizione ricevuta da ogni figlio
        for(int i=0; i<N; ++i){
            nr=read(pipedFp[i][0], &pos, sizeof(pos));
            
            if(nr==0) continue;
            //se ha ricevuto qualcosa dal figlio, lo mostra all'utente
            else{
                ++ricevute;
                printf("Il figlio %d ha trovato nel file %s il carattere %c nella posizione %ld, con che carattere desidera sostituirlo? (se non si vuole sostituire non scrivere nulla)\n", i, argv[i+1], Cx, pos);
                //leggo da standard output
                read(1, &indicazione, 1);

                //invio al figlio l'indicazione
                nw=write(pipedPf[i][1], &indicazione, sizeof(indicazione));
                if(nw!=sizeof(indicazione)){
                     printf("Errore: il padre non riesce a scrivere sulla pipe pipedPf\n");
                    exit (8);
                }
                //leggo da standard output il carattere a capo
                read(1, &indicazione, 1);
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