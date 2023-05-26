#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>


int main(int argc, char ** argv){
    /*-------------*variabili locali-------------*/
char CZ;        //contiene il primo parametro
int N;          //contiene il numero di file passati
int Ff;         //contiene fd file associati a ogni processo(i+2)
typedef int pipe_t[2];
pipe_t *pipedFp;        //per contenere pipe da figli a padre
pipe_t *pipedPf;        //per contenere pipe da padre a figli
int nw, nr;                 //per controllo scrittura sulla pipe
int pidFiglio, status, ritorno; //per fork e wait
long int pos;           //per contenere posizione dentro al file che sto leggendo. PRIMA POSIZIONE=1
int trovate;            //per contenere numero pCZ trovati nel file associato
char ch;                //per contenere carattere appena letto da file associato
long int letto, max;    //per contenere valore ritornato da un figlio, per settare il max
int indexMax;           //contiene indice del figlio che ha trovato il max
int numeroPipeLette;    //contiene il numero di pipe da cui ho letto
int *vfinito;           //contiene figli temrinati
    /*-------------------------------------------*/

//controllo: almeno 3 parametri
if(argc<4){
    printf("Errore: passati %d parametri, ma ne servono almeno 3\n", argc-1);
    exit (1);
}
N=argc-2;
//controllo: il primo parametro deve essere 1 singolo carattere
if(strlen(argv[1])!=1){
    printf("Errore: il primo parametro %s non è un singolo carattere\n", argv[1]);
    exit (2);
}
CZ=*argv[1];

//alloco spazio per le 2 pipe e per vfinito
vfinito=(int *) malloc(N*sizeof(int));
pipedPf=(pipe_t*) malloc(N*sizeof(pipe_t));
pipedFp=(pipe_t*) malloc(N*sizeof(pipe_t));
if(pipedFp==NULL || pipedPf==NULL || vfinito==NULL){
    printf("Errore: problemi nell'allocazione di memoria per le pipe padre-figli oppure per vfinito\n");
    exit (3);
}
//il pare crea N*2 pipe
for(int i=0; i<N; ++i){
    if(pipe(pipedFp[i])<0 || pipe(pipedPf[i])<0){
        printf("Errore: problemi nella creazione delle pipe\n");
        exit (4);
    }
}



//il padre crea gli N figli
for(int i=0; i<N; ++i){
    pidFiglio=fork();
    vfinito[i]=1;
        if(pidFiglio<0){
        printf("Errore: problemi nella fork\n");
        exit (3);
        }
        if(pidFiglio==0){
            //il figlio chiude i lati dell pipe:
            //su pipedFp chiude tutti i lati di scrittura tranne il suo, tutti i lati di lettura
            //su pipedPf chiude tutti i lati di scrittura, tutti i lati di lettura tranne il suo
            for(int j=0; j<N; ++j){
                close(pipedFp[j][0]); close(pipedPf[j][1]);
                if(j!=i) {
                    close(pipedFp[j][1]); close(pipedPf[j][0]);
                }
            }

            //il figlio prova ad aprire il file associato
            if((Ff=open(argv[i+2], O_RDONLY ))<0 ){
                printf("Errore: impossibile aprire in lettura il file %s\n", argv[i+2]);
                exit (-1);
            }
            
            //il figlio inizia a leggere il file e cerca occorrenze
            pos=1;  trovate=0;
            while(read(Ff, &ch, 1)>0){
                if(ch==CZ){
                    ++trovate;
                    //printf("DEBUG-in %s trovato in pos %ld\n", argv[i+2], pos);
                    nw=write(pipedFp[i][1], &pos, sizeof(pos));
                    if(nw!=sizeof(pos)){
                        printf("Il pfiglio di pid %d non è riuscito a scrivere sulla pipe\n", getpid());
                        exit (-1);
                    }
                    nr=read(pipedPf[i][0], &ch, 1);
                    if(nr!=sizeof(ch)){
                        printf("Il pfiglio di pid %d non è riuscito a leggere dalla pipe\n", getpid());
                        exit (-1);
                    }
                    if(ch=='v'){
                        printf("Il processo di indice d'ordine %d e pid %d ha trovato il carattere %c alla posizione %ld nel file %s\n", i, getpid(), CZ, pos, argv[i+2]);
                    }
                }
                ++pos;
            }
            printf("DEBUG-fine figlio del file %s\n", argv[i+2]);
            exit (trovate);
        } 
}
//printf("Il padre fa cose\n");
//il padre chiude i lati della pipe
for(int i=0; i<N; ++i){
    close(pipedFp[i][1]); close(pipedPf[i][0]);
}

//finchè qualcuno scrive sulla pipe, il padre legge i gruppi
        //MI SEGNO PIPE CHE HANNO FINITO E LEGGO MAX
        max=0; numeroPipeLette=0; 
        for(int i=0; i<N; ++i){
            if(vfinito[i]!=1) continue;
            
            nr=read(pipedFp[i][0], &letto, sizeof(letto));
            if(nr!=sizeof(letto)){
                printf("DEBUG-la pipe %d è stata chiusa\n", i);
                vfinito[i]=-1;
            }
            numeroPipeLette+=1;
            if(letto>max){
                max=letto;
                indexMax=i;
            }
        }
while(numeroPipeLette>0){
    for(int i=0; i<N; ++i){
        if(vfinito[i]==-1) continue;

        if(i==indexMax){
            nw=write(pipedPf[i][1], "v", 1);
        }
        else{
        nw=write(pipedPf[i][1], "n", 1);
        }
        if(nw!=1){
            printf("Il padre di pid %d non è riuscito a scrivere sulla pipe\n", getpid());
            exit (5);
        }
    }
            //MI SEGNO PIPE CHE HANNO FINITO E LEGGO MAX
            max=0; numeroPipeLette=0; 
            for(int i=0; i<N; ++i){
                if(vfinito[i]!=1) continue;
                
                nr=read(pipedFp[i][0], &letto, sizeof(letto));
                if(nr!=sizeof(letto)){
                    printf("DEBUG-la pipe %d è stata chiusa\n", i);
                    vfinito[i]=-1;
                }
                numeroPipeLette+=1;
                if(letto>max){
                    max=letto;
                    indexMax=i;
                }
            }
}

//il padre aspetta i figli
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