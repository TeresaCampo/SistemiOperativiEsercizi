#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <ctype.h>

int main(int argc, char ** argv){
        /*-------------*variabili locali-------------*/
    typedef struct{
        char chr;                   //contiene carattere AM
        long int occ;               //contiene max tra Nocc e Pocc
        char proc;                  //contiene tipo(F/N) processo che lo ha trovato
        int pid;                    //contiene pid processo che lo ha trovato
    } struttura;
    int Npar;                       //contiene il numero di parametri passati
    int N;                          //contiene il numero di processi figli
    int Fj;                         //contiene fd del file corrispondente al processo
    int *vpidF;                     //contiene pid degli N figli
    int *vpidN;                     //contiene pid degli N nipoti
    int *vordine;                    //contiene ordine in cui sono stati creati i figli
    int counter;                    //serve per ordine di creazione
    char AM;                        //contiene carattere alfabetico maiuscolo
    char ch;                        //contiene carattere appena letto dal file corrisponendente
    long int Pocc, Nocc;            //contiene il numero di occorrenze del carattere AM nel file corrispondente al processo
    struttura s;                    //contiene info figlio->padre
    

    typedef int pipe_t[2];
    pipe_t *pipedFn;                //contiene pipe figlio->nipote
    pipe_t *pipedNf;                //contiene pipe nipote->figlio
    pipe_t *pipedFp;                //contiene pipe figlio->padre
    int nr, nw;                     //per controllare lettura e scrittura pipe
    int pidFiglio, pidNipote, status, ritorno; //per fork e wait
        /*-------------------------------------------*/
    
    //controllo: numero parametri pari e almeno 4
    Npar=argc-1;
    if(Npar<4 || (Npar%2)!=0){
        printf("Errore: passati %d parametri, ma ne servono almeno 4 e devono essere in numero pari\n", Npar);
        exit (1);
    }
    N=Npar/2;
    //controllo: file apribili in lettura lo faranno poi figli e nipoti associati

    //alloco array pipe
    pipedFn= (pipe_t*) malloc(N*sizeof(pipe_t));
    if(pipedFn==NULL){
        printf("Errore: problemi nell'allocazione di memoria per le pipe pipedFn\n");
        exit (2);
    }
    pipedNf= (pipe_t*) malloc(N*sizeof(pipe_t));
    if(pipedNf==NULL){
        printf("Errore: problemi nell'allocazione di memoria per le pipe pipedNf\n");
        exit (3);
    }
    pipedFp= (pipe_t*) malloc(N*sizeof(pipe_t));
    if(pipedFp==NULL){
        printf("Errore: problemi nell'allocazione di memoria per le pipe pipedFp\n");
        exit (4);
    }
    //creo pipe
    for(int i=0; i<N; ++i){
         if(pipe(pipedFn[i])<0){
            printf("Errore: problemi nella creazione della pipe pipedFn\n");
            exit (5);
        }
    }
    for(int i=0; i<N; ++i){
         if(pipe(pipedNf[i])<0){
            printf("Errore: problemi nella creazione della pipe pipedNf\n");
            exit (6);
        }
    }
    for(int i=0; i<N; ++i){
         if(pipe(pipedFp[i])<0){
            printf("Errore: problemi nella creazione della pipe pipedFp\n");
            exit (7);
        }
    }
    //alloco vpid
    vpidF= (int*) malloc(N*sizeof(int));
    if(vpidF==NULL){
        printf("Errore: problemi nell'allocazione di memoria per l'array vpidF\n");
        exit (8);
    }
    vpidN= (int*) malloc(N*sizeof(int));
    if(vpidN==NULL){
        printf("Errore: problemi nell'allocazione di memoria per l'array vpidN\n");
        exit (9);
    }
    //alloco vordine
    vordine= (int*) malloc(N*sizeof(int));
    if(vordine==NULL){
        printf("Errore: problemi nell'allocazione di memoria per l'array vordine\n");
        exit (10);
    }

    counter=0;
    //padre crea N figli
    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli\n", getpid(), Npar);
    for(int i=0; i<N; ++i){
        pidFiglio=fork();

        if(pidFiglio<0){
             printf("Errore nella fork\n");
             exit (11);
        }
         if(pidFiglio==0){
            vpidF[i]=getpid();
            printf("DEBUG-Sono il processo figlio con pid %d\n", getpid());
            //salvo ordine di creazione
            //chiudo lati pipe
            for(int j=0; j<N; ++j){
                close(pipedFp[j][0]);
                if(j!=i){
                    close(pipedFp[j][1]);
                }
            }
            //creo processo nipote
            printf("DEBUG-Sono il processo figlio con pid %d e sto per creare  1 nipote\n", getpid());
            pidNipote=fork();
            vpidN[i]=pidNipote;
            if(pidNipote<0){
                printf("Errore nella fork\n");
                exit (-1);
            }
             if(pidNipote==0){
                printf("DEBUG-Sono il processo nipote con pid %d\n", getpid());
                //chiudo lati pipe
                close(pipedFp[i][1]);
                for(int j=0; j<N; ++j){
                    close(pipedNf[j][0]);   close(pipedFn[j][1]);
                    if(j!=i){
                        close(pipedNf[j][1]);   close(pipedFn[j][0]);
                    }
                }
                //apre il file associato
                if((Fj=open(argv[1+i+N], O_RDONLY))<0){
                printf("Errore: impossibile aprire in lettura il file %s\n", argv[1+i+N]);
                exit (-1);
                }

                //legge carattere AM dal figlio
                nr=read(pipedFn[i][0], &AM, sizeof(AM));
                if(nr!=sizeof(AM)){
                    printf("Errore: il nipote non riesce a leggere dalla pipe pipedFn\n");
                    exit (-1);
                }

                //inizia a leggere il file associato in cerca delle occorrenze di AM
                Nocc=0;
                while(read(Fj, &ch, 1)>0){
                    if(ch==AM){
                        ++Nocc;
                    }
                }
                 
                //comunica Pocc trovate al figlio
                nw=write(pipedNf[i][1], &Nocc, sizeof(Nocc));
                if(nw!=sizeof(Nocc)){
                    printf("Errore: il nipote non riesce a scrivere sulla pipe pipedNf\n");
                    exit (-1);
                }
                exit(ch);       //ritorno ultimo carattere letto
            }
            //codice del figlio
            //chiude lati pipe
            for(int j=0; j<N; ++j){
                close(pipedNf[j][1]);   close(pipedFn[j][0]);
                if(j!=i){
                    close(pipedNf[j][0]);   close(pipedFn[j][1]);
                }
            }
            //apro file associato
            if((Fj=open(argv[1+i], O_RDONLY))<0){
            printf("Errore: impossibile aprire in lettura il file %s\n", argv[1+i]);
            exit (-1);
            }
            //inizializzo AM con una letta non maiuscola
            AM='a';
            //leggo il file
            Pocc=0;
            while(read(Fj, &ch, 1)>0){
                if(isupper(ch)!=0){
                    //se è il primo carattere maiuscolo che trovo
                    if(AM=='a'){
                        AM=ch;
                        ++Pocc;

                        //scrivo AM al nipote
                        nw=write(pipedFn[i][1], &AM, sizeof(AM));
                        if(nw!=sizeof(AM)){
                            printf("Errore: il figlio non riesce a scrivere sulla pipe pipedFn\n");
                            exit (-1);
                        }
                    }
                    //se non è il primo carattere maiuscolo che trovo
                    else{
                        if(ch==AM){
                            ++Pocc;
                        }
                    }
                }
            }
            //scrivo AM==a al nipote se non ho trovato nemmeno un carattere maiuscolo
            if(Pocc==0){
                nw=write(pipedFn[i][1], &AM, sizeof(AM));
                if(nw!=sizeof(AM)){
                    printf("Errore: il figlio non riesce a scrivere sulla pipe pipedFn\n");
                    exit (-1);
                }
            }

            //aspetta Nocc
            nr=read(pipedNf[i][0], &Nocc, sizeof(Nocc));
            if(nr!=sizeof(Nocc)){
                printf("Errore: il figlio non riesce a leggere dalla pipe pipedNf\n");
                exit (-1);
            }
            //printf("DEBUG-Il nipote ha trovato %ld occorrente di %c nel file %s\n", Nocc, AM, argv[1+i+N]);
            //printf("DEBUG-Il figlio ha trovato %ld occorrente di %c nel file %s\n", Pocc, AM, argv[1+i]);
           s.chr=AM;
           if(Nocc>Pocc){
            s.occ=Nocc;
            s.proc='N';
            s.pid=vpidN[i];
           }
           else{
            s.occ=Pocc;
            s.proc='F';
            s.pid=vpidF[i];
           }
           //se non ho trovato nemmeno 1 carattere maiuscolo scrivo 0
            if(Pocc==0){
                s.chr='a';
                s.pid=vpidF[i];
            }
           //comunica al padre la struttura
           nw=write(pipedFp[i][1], &s, sizeof(s));
           if(nw!=sizeof(s)){
            printf("Errore: il figlio non riesce a scrivere sulla pipe pipedFp\n");
            exit (-1);
           }
           exit(ch);       //ritorno ultimo carattere letto
        }
        vordine[i]=counter;
        ++counter;
    }
    //codice del padre
    //chiudo lati pipe
    for(int j=0; j<N; ++j){
        close(pipedNf[j][1]);   close(pipedNf[j][0]);
        close(pipedFn[j][1]);   close(pipedFn[j][0]);  
        close(pipedFp[j][1]);  
    }
    
    //padre legge le strutture mandate da ogni figlio
    for(int i=0; i<N; ++i){
        //printf("Il processo creato per %d è quello di indice d'ordine %d\n", vordine[i], i);
        nr=read(pipedFp[i][0], &s, sizeof(s));
        if(nr!=sizeof(s)){
            printf("Errore: il padre non riesce a leggere dalla pipe pipedFp\n");
            exit (11);
        }
        if(s.chr=='a'){
             printf("Il processo figlio di pid %d non ha trovato caratteri maiuscoli nel file %s\n", s.pid, argv[1+i]);
             continue;

        }
        if(s.proc=='F'){
        printf("Il processo figlio di pid %d ha trovato %ld occorrenze del carattere %c nel file %s\n", s.pid, s.occ, s.chr, argv[1+i]);
        }
        else{
        printf("Il processo nipote di pid %d ha trovato %ld occorrenze del carattere %c nel file %s\n", s.pid, s.occ, s.chr, argv[1+N+i]);
        }

    }

    //il padre aspetta tutti i processi figli
    for(int i=0; i<N; ++i){
         if ((pidFiglio=wait(&status)) < 0){
             printf("Errore wait\n");
             exit(12);
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