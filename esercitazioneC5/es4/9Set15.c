#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(int argc, char ** argv){
    /*---------variabili locali------------*/
int N;          //contiente il nuemro di file da controllare
int Af;         //contiene fd file del padre
int Fi;         //contiene fd file del figlio
char ch;        //contiene il carattere letto dal figlio dal file
char check;     //contiene il carattere letto dal padre
typedef int pipe_t[2];
pipe_t *pipedFp;        //per contenere pipe da figli a padre
pipe_t *pipedPf;        //per contenere pipe da padre a figli
int pidFiglio, status, ritorno;      //per fork e wait
int *pid;           //per pid di tutti i figli, per la kill
int *continuare;    //per sapere se devo rimandare segnale a un figlio oppure no
    /*------------------------------------*/


//controllo: numero di parametri almeno 5
if(argc<4){
    printf("Errore: passati %d parametri, ma ne servono almeno 3\n", argc-1);
    exit (1);
}
N=argc-2;

//il padre crea 2*N pipe 
pipedPf=malloc(N*sizeof(pipe_t));
pipedFp=malloc(N*sizeof(pipe_t));
for(int i=0; i<N; ++i){
    if(pipe(pipedPf[i])<0 || pipe(pipedFp[i])<0){
        printf("Errore: problemi nella creazione della pipe\n");
        exit (2);
    }
}
//il padre alloca spazio per i due array 
continuare= malloc(N*sizeof(int));
//inizializzo continuare, con si
for(int i=0; i<N; ++i){
    continuare[i]=1;
}
pid= malloc(N*sizeof(int));


//il padre crea N figli
for(int i=0; i<N; ++i){
    pidFiglio=fork();
    pid[i]=pidFiglio;

        if(pidFiglio<0){
        printf("Errore: problemi nella fork\n");
        exit (3);
        }
        if(pidFiglio==0){
            //il figlio chiude i lati della pipe che non usa: read e write tranne il suo
            for(int j=0; j<N; ++j){
                close(pipedFp[j][0]); close(pipedPf[j][1]);
                if(i!=j){
                    close(pipedFp[j][1]); close(pipedPf[j][0]);
                }
            }

            //figlio prova ad aprire il file associato
            if((Fi=open(argv[i+1], O_RDONLY))<0){
                printf("Errore: problemi nell'apertura del file %s\n", argv[i+1]);
                exit (-1);
            }

            //figlio legge un carattere alla volta dal file, dopo aver ricevuto il segnale dal padre
            while(read(pipedPf[i][0], &ch, 1)){
                read(Fi, &ch, 1);
                write(pipedFp[i][1], &ch, 1);
            }
            exit (0);
        }

}

//il padre chiude i lati della pipe che non usa: latti in scrittura
for(int j=0; j<N; ++j){
    close(pipedFp[j][1]);
    close(pipedPf[j][0]);
}

//il padre apre il file passato come ultimo parametro
if((Af=open(argv[argc-1], O_RDONLY))<0){
    printf("Errore: problemi nell'apertura del file %s\n", argv[argc-1]);
    exit (3);
}
//per ogni carattere, chiede a ogni figlio
while(read(Af, &check, 1)>0){
    for(int i=0; i<N; ++i){
        if(continuare[i]==1){
            //dico al figlio che può leggere
            write(pipedPf[i][1], "v", 1);
            read(pipedFp[i][0], &ch, 1);
            //se i caratteri non combaciano, imposto che il figlio non mi serve più
            if(ch!=check){
                continuare[i]=0;
            }
        }
    }
}
//uso kill sui figli che sono stati stoppati
for(int i=0; i<N; ++i){
    if(continuare[i]==0){
        if(kill(pid[i], SIGKILL)<0){
            printf("Errore: il figlio %d è già terminato\n", pid[i]);
            exit (4);
        }
    }
}

//il padre aspetta tutti i figli che non sono ancora terminati
for(int i=0; i<N; ++i){
    if(pid[i]==0) continue;
    else{
    if ((pidFiglio=wait(&status)) < 0)
        {
                printf("Errore wait\n");
                exit(5);
        }
        if ((status & 0xFF) != 0)
                printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
        else
        {       int j;
                for(j=0; j<0; ++j){
                    if(pid[j]==pidFiglio) break;
                }
                ritorno=(int)((status >> 8) & 0xFF);
                printf("Il figlio con pid=%d che lavorava sul file %s ha ritornato %d (se 255 problemi!)\n", pidFiglio, argv[i+1], ritorno);
        }
    }
}

exit (0);
}