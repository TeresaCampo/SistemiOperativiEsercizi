#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdbool.h>
typedef struct{
    char v1;        //contiene il carattere cercato
    long int v2;    //contiene il nuero di occorrenze
} struttura;
struttura vstruttura[26];   //contiene 26 strutture, dei figli
int vpid[26];                   //contiene pid figli


void scambia(struttura *v1, int *indexv1, struttura *v2, int *indexv2){
    struttura tmp=*v1;
    *v1 =*v2;
    *v2=tmp;
    
    int indextmp=*indexv1;
    *indexv1=*indexv2;
    *indexv2=indextmp;
}

void bubbleSort(struttura v[], int dim){ 
    int i; bool ordinato = false;
    while (dim>1 && !ordinato){ 
    ordinato = true; /* hp: è ordinato */
        for (i=0; i<dim-1; i++){
            if (v[i].v2> v[i+1].v2){
            scambia(&v[i], &vpid[i],&v[i+1], &vpid[i+1]);
            ordinato = false;
            }
        }
        dim--;
    }
}

int main(int argc, char **argv){

    /*-------------*variabili locali-------------*/

typedef int pipe_t[2];
pipe_t piped[26];        //per contenere pipe da figlio a figlio, l'ultima da figlio a padre
                        //processo legge su piped[i-1] e scrive su piped[i]
int fd;                 //fd file
int pidFiglio, status, ritorno; //per fork e wait
char chCheck;           //contiene carattere che deve cercare figlio corrente
char chLetto;           //contiene il carattere appena letto nel file associato
long int occorrenze;    //contiene numero di occorrenze trovate
int nr, nw;                 //per controllare lettura pipe
    /*-------------------------------------------*/

//controllo: 1 solo parametro
if(argc!=2){
    printf("Errore: passati %d parametri, ma ne serve solo 1\n", argc-1);
    exit (1);
}

//controllo: il parametro è un file apribile in lettura
//poi lo chiudo
if((fd=open(argv[1], O_RDONLY ))<0 ){
    printf("Errore: impossibile aprire in lettura il file %s\n", argv[1]);
    exit (2);
}
close(fd);

//il padre crea le 26 pipe
for(int i=0; i<26; ++i){
    if(pipe(piped[i])<0){
        printf("Errore: problemi nella creazione della pipe\n");
        exit (3);
    }
}

chCheck='a';
//il padre crea i 26 figli
for(int i=0; i<26; ++i){
    pidFiglio=fork();
    vpid[i]=pidFiglio;
        if(pidFiglio<0){
        printf("Errore: problemi nella fork\n");
        exit (4);
        }
        if(pidFiglio==0){
            //il figlio chiude le pipe: tutti i lati di lettura tranne (i-1), tutti i lati di scrittura tranne i
            //il figlio i-1 non legge da nessuno
            for(int j=0; j<26; ++j){
                if(j!=(i-1)){
                    close(piped[j][0]);
                }
                if(j!=i){
                    close(piped[j][1]);
                }
            }

            //il figlio porva ad aprire il file
            if((fd=open(argv[1], O_RDONLY ))<0 ){
                printf("Errore: impossibile aprire in lettura il file %s\n", argv[1]);
                exit (-1);
            }

            occorrenze=0;
            //il figlio cerca il valore in tutto il file
            while(read(fd, &chLetto, 1)>0){
                if(chLetto==chCheck){
                    ++occorrenze;
                }
            } 

            //tutti i figli diversi dal primo aspettano di leggere l'array di struct dal figlio precedente
            if(i!=0){
                nr=read(piped[i-1][0], vstruttura, sizeof(vstruttura));
                if(nr!=sizeof(vstruttura)){
                    printf("Errore: il figlio di pid %d non riesce a leggere dalla pipe %i\n", getpid(), i);
                    exit (-1);
                }
            }
            vstruttura[i].v1=chCheck;
            vstruttura[i].v2=occorrenze;
            nw=write(piped[i][1], &vstruttura, sizeof(vstruttura));
            if(nw!=sizeof(vstruttura)){
                printf("Errore: il figlio di pid %d non riesce a leggere dalla pipe %i\n", getpid(), i);
                exit (-1);
            }
            exit (chLetto);
        }
        ++chCheck;
}

//il padre chiude i lati della pipe: tutti i lati di scrittura, tutti i lati di lettura tranne 25
for(int i=0; i<26; ++i){
    close(piped[i][1]);
    if(i!=25){
        close(piped[i][0]);
    }
}

//il padre legge dall'ultimo figlio l'array completato
nr=read(piped[25][0], vstruttura, sizeof(vstruttura));
    if(nr!=sizeof(vstruttura)){
        printf("Errore: il padre di pid %d non riesce a leggere dalla pipe 25\n", getpid());
        exit (4);
    }
//il padre ordina vstrutture in modo crescente in base a v2
bubbleSort(vstruttura, 26);

//il padre scrive su stdoutput tutti i dati
for(int i=0; i<26; ++i){
    printf("Il figlio di pid %d ha trovato %ld occorrenze del carattere %c\n", vpid[i], vstruttura[i].v2, vstruttura[i].v1);
}

//il padre aspetta tutti i figli
for(int i=0; i<26; ++i){
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
                printf("Il figlio con pid=%d ha ritornato %c (%dse 255 problemi!)\n", pidFiglio, ritorno, ritorno);
        }
    }
    exit (0);   
}