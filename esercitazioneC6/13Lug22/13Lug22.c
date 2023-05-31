#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char **argv){
        /*-------------*variabili locali-------------*/
    int Q;                          //contiene numero di processi figli=caratteri da controllare in ogni linea
    int L;                          //contiene numero linee del file F
    int F;                          //contiene fd del file passato come primo parametro
    char Cq;                        //contiene carattere associato al rispettivo processo
    char chLetto;                   //contiene il carattere appena letto dal file associato
    int occorrenze;                 //contiene occorrenze del carattere Cq nella linea corrente
    char indicazione;               //contiene ok del processo precedente al successivo

    typedef int pipe_t[2];
    pipe_t pipedPf0;                //1 pipe tra padre->figlio0
    pipe_t *pipedFf;                //Q pipe tra fuglio->successivo/padre
    pipe_t pipedFp;                 //1 pipe tra tutti i figli->padre
    int nr, nw;                     //per controllare lettura e scrittura pipe
    int *vpid;                      //contiene pid dei processi creati
    int pidFiglio, status, ritorno; //per fork e wait
        /*-------------------------------------------*/ 

    //controllo: almeno 4 parametri
    if(argc<5){
        printf("Errore: passati %d parametri, ma dovrebbero essere almeno 4\n", argc-1);
        exit (1);
    }
    Q=argc-3;

    //controllo: F file apribil ein lettura lo controlleranno i figli
    //controllo: L strettamente positivo
    L=atoi(argv[2]);
    if(L<=0){
        printf("Errore: la lunghezza in linee del file passata come parametro è %d, ma deve essere strettamente positivo\n", L);
        exit (2);
    }
    //controllo: ultimi Q parametri devono essere singoli caratteri
    for(int q=0; q<Q; ++q){
        if(strlen(argv[3+q])!=1){
            printf("Errore: il parametro %s dovrebbe essere un singolo carattere\n", argv[3+q]);
        }
    }

    //sincronizzazione a ring: Q pipe tra un process figlio e il successivo/padre. processo scrive su q, legge su q-1
    //1 pipe tra padre e processo figlio0
    //una pipe tra tutti i figli e padre

    //allco spazio per vpid
    vpid= (int*) malloc(Q*sizeof(int));
    if(vpid==NULL){
        printf("Errore: problemi nell'allocazione di memoria per l'array vpid\n");
        exit (3);
    }

    //alloco spazio per Q pipe
    pipedFf= (pipe_t*) malloc(Q*sizeof(pipe_t));
    if(pipedFf==NULL){
        printf("Errore: problemi nell'allocazione di memoria per le pipe pipedFf\n");
        exit (4);
    }
    //creo le pipe pipedFf
    for(int q=0; q<Q; ++q){
         if(pipe(pipedFf[q])<0){
            printf("Errore: problemi nella creazione della pipe pipedFf\n");
            exit (5);
        }
    }
    //creo le altre 2 pipe
    if(pipe(pipedFp)<0){
        printf("Errore: problemi nella creazione della pipe pipedFp\n");
        exit (6);
    }
    if(pipe(pipedPf0)<0){
        printf("Errore: problemi nella creazione della pipe pipedPf0\n");
        exit (7);
    }

    //il processo padre cre Q processi figli
    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli\n", getpid(), Q);
    for(int q=0; q<Q; ++q){
        pidFiglio=fork();
        vpid[q]=pidFiglio;

        if(pidFiglio<0){
             printf("Errore nella fork\n");
             exit (8);
        }
         if(pidFiglio==0){
            printf("DEBUG-Sono il processo figlio con pid %d\n", getpid());
            //il figlio chiude le pipe: pipedFf chiude tutto in lettura tranne pipedFf[q-1], chiude tutto in scrittura tranne pipedFf[q]
            //pipedPf0: chiude tutto tranne processo 0 che lascia aperta lettura
            //pipedFp chiude in lettura
            for(int j=0; j<Q; ++j){
                close(pipedFp[0]);  close(pipedPf0[1]);
                if(j!=q){
                    close(pipedFf[j][1]);
                }
                if(j!=(q-1)){
                    close(pipedFf[j][0]);
                }
                if(q!=0){
                    close(pipedPf0[0]);
                }
            }
            Cq=*argv[3+q];

            //provo ad aprire il file associato
            if((F=open(argv[1], O_RDONLY))<0){
            printf("Errore: impossibile aprire in lettura e scrittura il file %s\n", argv[1]);
            exit (-1);
            }

            occorrenze=0;
            while(read(F, &chLetto, 1)>0){
                if(chLetto==Cq){
                ++occorrenze;
                }
                //arrivato a fine linea, aspetta che il processo precedente gli dia l'ok, poi scrive al padre, poi da l'ok al processo seguente
                if(chLetto=='\n'){
                    //tutti i processi(tranne il primo) leggono dal figlio precedente
                    if(q!=0){
                        nr=read(pipedFf[q-1][0], &indicazione, sizeof(indicazione));
                        if(nr!=sizeof(indicazione)){
                            printf("Errore: il figlio %d non riesce a leggere dalla pipe pipedFf\n", q);
                            exit (-1);
                        }
                    }
                    //il primo processo legge dapipedPf0
                    if(q==0){
                        nr=read(pipedPf0[0], &indicazione, sizeof(indicazione));
                        if(nr!=sizeof(indicazione)){
                            printf("Errore: il figlio %i non riesce a leggere dalla pipe pipedPf0\n", q);
                            exit (-1);
                        }
                    }

                    //il processo scrive al padre su pipedFp
                    nw=write(pipedFp[1], &occorrenze, sizeof(occorrenze));
                    if(nw!=sizeof(occorrenze)){
                         printf("Errore: il figlio %i non riesce a scrivere sulla pipe pipedFp\n", q);
                    exit (-1);
                    }

                    //il processo da l'ok al processo dopo
                    nw=write(pipedFf[q][1], "v", 1);
                    if(nw!=1){
                         printf("Errore: il figlio non riesce a scrivere sulla pipe pipedFf\n");
                    exit (-1);
                    }
                    ritorno=occorrenze;
                    occorrenze=0;
                }
            }

            exit(ritorno);
        }
    }

    //codice del padre
    //il padre chiude le pipe: pipedFf tutto tranne lettura di Q-1
    //pipedPf0 chiude lato lettura
    //pipedFp chiude lato di scrittura
    for(int q=0; q<Q; ++q){
        close(pipedFf[q][1]);
        if(q!=Q-1){
             close(pipedFf[q][0]);
        }
    }
    close(pipedPf0[0]);
    close(pipedFp[1]);
    

    //per ogni riga del file: manda segnale su pipedPf0, per ogni figlio legge su pipedFp e scrive il rispettivo valore su St Out
    for(int l=0; l<L; ++l){
        printf("Linea %d del file %s\n", l+1, argv[1]);
        //il padre da il segnale al figlio 0
        nw=write(pipedPf0[1], "v", 1);
        if(nw!=1){
             printf("Errore: il padre non riesce a scrivere sulla pipe pipedPf0");
        exit (9);
        }

        for(int q=0; q<Q; ++q){
            nr=read(pipedFp[0], &occorrenze, sizeof(occorrenze));
            if(nr!=sizeof(occorrenze)){
                printf("Errore: il padre non riesce a leggere dalla pipe pipedFp\n");
                exit (10);
            }
            printf("Figlio con indice %d e pid %d ha letto %d caratteri %c nella linea corrente\n", q, vpid[q], occorrenze, *argv[3+q]);
        }
    }
    
    //il padre aspetta tutti i processi figli
    for(int q=0; q<Q; ++q){
         if ((pidFiglio=wait(&status)) < 0){
             printf("Errore wait\n");
             exit(11);
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