#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>

int main(int argc, char ** argv){
    /*-------------*variabili locali-------------*/
int N;                          //contiene numero di file passari;
int Fk;                         //contiene fd del file associato a ogni figlio
int lenByte;                    //conteine lunghezza in byte di ogni file
int divisore;                   //contiene divisore scelto dall'utente per ogni file
char ch;                        //contiene carattere appena letto dal file associato
int pos;                        //contiene indice carattere nel file
int inviato;                    //conteien nummero di caratteri inviati al padre
int pipeLette=0;                //contiene numero di pipe lette dal padre(quando sero, fine)
int giro;                       //per tenere contro di quante volte ha letto da una pipe
int *vDivisore;                 //per contenere i divisorei associati ai vari file

typedef int pipe_t[2];
pipe_t *pipedPf;                //contiene pipe padre->figlio
pipe_t *pipedFp;                //contiene pipe figlio->padre
int nr, nw;                     //per controllare lettura e scrittura pipe
int pidFiglio, status, ritorno; //per fork e wait
    /*-------------------------------------------*/

//controllo: almeno 2 parametri e pari
N=argc-1;
if(N<2 || (N%2)!=0){
    printf("Errore: passati %d parametri, ma devono essere almeno 2 e devono essere un numero pari\n", N);
    exit (1);
}
N=N/2;

//alloco vDivisori
vDivisore= (int*) malloc(N*sizeof(int));
if(vDivisore==NULL){
    printf("Errore: problemi nell'allocazione di memoria per l'array vDivisore\n");
    exit (2);
}

//creo 2*N pipe
//alloco spazio per array pipe
pipedPf= (pipe_t*) malloc(N*sizeof(pipe_t));
if(pipedPf==NULL){
    printf("Errore: problemi nell'allocazione di memoria per le pipe pipedPf\n");
    exit (3);
}
pipedFp= (pipe_t*) malloc(N*sizeof(pipe_t));
if(pipedFp==NULL){
    printf("Errore: problemi nell'allocazione di memoria per le pipe pipedFp\n");
    exit (4);
}

//creo pipe
for(int i=0; i<N; ++i){
     if(pipe(pipedPf[i])<0){
        printf("Errore: problemi nella creazione della pipe pipedPf\n");
        exit (5);
    }
}
for(int i=0; i<N; ++i){
     if(pipe(pipedFp[i])<0){
        printf("Errore: problemi nella creazione della pipe pipedFp\n");
        exit (6);
    }
}

//creo N figli
printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli\n", getpid(), N);
for(int i=0; i<N; ++i){
    pidFiglio=fork();
    if(pidFiglio<0){
         printf("Errore nella fork\n");
         exit (7);
    }
     if(pidFiglio==0){
        printf("DEBUG-Sono il processo figlio con pid %d\n", getpid());
        //chiudo lati pipe:
        for(int j=0; j<N; ++j){
            close(pipedPf[i][1]);   close(pipedFp[i][0]);
            if(j!=i){
                close(pipedPf[j][0]);   close(pipedFp[j][1]);
            }
        }

        //leggo il multiplo dal padre
        nr=read(pipedPf[i][0], &divisore, sizeof(divisore));
        if(nr!=sizeof(divisore)){
            printf("Errore: il figlio non riesce a leggere dalla pipe pipedPf\n");
            exit (-1);
        }
        printf("DEBUG-Il figlio legge dal padre il divisore %d\n", divisore);

        //provo ad aprire il file associato
        if((Fk=open(argv[1+(i*2)], O_RDONLY))<0){
        printf("Errore: impossibile aprire in lettura il file %s\n", argv[1+(i*2)]);
        exit (-1);
        }

        //leggo il file, scrivo il multiplo al padre
        pos=1;  inviato=0;
        while(read(Fk, &ch, 1)>0){
            if((pos%divisore)==0){
                //printf("Trovato il carattere %c nel file %s\n", ch, argv[1+(i*2)]);
                nw=write(pipedFp[i][1], &ch, sizeof(ch));
                if(nw!=sizeof(ch)){
                     printf("Errore: il figlio non riesce a scrivere sulla pipe pipedFp\n");
                    exit (-1);
                }
                ++inviato;
            }
            ++pos;
        }
        exit (inviato);
    }
}

//codice del padre
//chiudo le pipe:
for(int i=0; i<N; ++i){
    close(pipedPf[i][0]);  close(pipedFp[i][1]);    
}

//per ogni figlio, chiedo indice all'utente e faccio il primo giro
pipeLette=0;
for(int i=0; i<N; ++i){
    lenByte=atoi(argv[2+(i*2)]);
    printf("Scrivere un divisore di %d, lunghezza in byte del file %s\n", lenByte, argv[1+(i*2)]);
    scanf("%d", &divisore);
    vDivisore[i]=divisore;

    if((divisore<=0)||(lenByte%divisore)!=0){
        printf("Il numero inserito(%d) non è un divisore di %d\n", divisore, lenByte);
        exit (8);
    }
    nw=write(pipedPf[i][1], &divisore, sizeof(divisore));
    if(nw!=sizeof(divisore)){
         printf("Errore: il padre non riesce a scrivere sulla pipe pipedPf\n");
    exit (9);
    }

    nr=read(pipedFp[i][0], &ch, sizeof(ch));
    if(nr!=0){
        printf("Il figlio con indice d'ordine %d ha letto il carattere %c che si trova nella posizione %d del file %s\n", i, ch, divisore, argv[1+(i*2)]);
        ++pipeLette;
    }
}
giro=2;
while(pipeLette>0){
    pipeLette=0;
    for(int i=0; i<N; ++i){
        nr=read(pipedFp[i][0], &ch, sizeof(ch));
        if(nr!=0){
            printf("Il figlio con indice d'ordine %d ha letto il carattere %c che si trova nella posizione %d del file %s\n", i, ch, (giro*vDivisore[i]), argv[1+(i*2)]);
            ++pipeLette;
        }
    }
    ++giro;
}

//il padre aspetta tutti i processi figli
for(int i=0; i<N; ++i){
     if ((pidFiglio=wait(&status)) < 0){
         printf("Errore wait\n");
         exit(10);
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