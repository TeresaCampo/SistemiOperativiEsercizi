                                                                                #include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>

typedef int pipe_t[2];         //pipe in cui figlio passa parametri al padre

int main(int argc, char ** argv){
/*----------variabili------------------*/
int N;                      //per contenere il numero di parametri passati
int pidFiglio, pidNipote, ritorno, status;  //per fare fork e wait
pipe_t *pipedFiglio;        //per contenere le N pipe tra figli e padri
pipe_t pipedNipote;        //per contenere la pipe tra nipote e figlio
int fdFile;                 //per contenere fd del parametro, per controllare se un file leggibile
struct {
    int c1;
    int c2;
    char c3[250];
}strutturaDati;             //per contenere i dati passati dal nipote
/*------------------------------------*/

//numero di parametri: almeno 3
//devono essere nomi di file: lo controllo nei figli
if(argc<4){
    printf("Errore: devono essere passati almeno 2 parametri\n");
    exit (1);
}
N=argc-1;
pipedFiglio= malloc(N*sizeof(pipe_t));

//il padre crea N pipe
for(int i=0; i<N; ++i){
    if(pipe(pipedFiglio[i])<0){
        printf("Errore: problemi nella creazione della pipe\n");
        exit (2);
    }
}

//il padre crea N processi figli
printf("Sono il processo padre con pid %d e sto per creare %d figli\n", getpid(), N);
for(int i=0; i<N; ++i){
    pidFiglio=fork();
    if(pidFiglio<0){
        printf("Errore nella fork\n");
        exit (-1);
    }
    if(pidFiglio==0){
        //chiude pipe: tutti i lati di lettura e tutti i lati di scrittura(tranne il suo)
        for(int j=0; j<N; ++j){
            close(pipedFiglio[j][0]); 
            if(j!=i) close(pipedFiglio[j][1]);
        }

        //controllo che il parametro sia un file apribile in lettura
        if((fdFile=open(argv[i+1], O_RDONLY ))<0){
            printf("Errore: impossibile aprire in lettura il file %s\n", argv[i+1]);
            exit (-1);
       } 
       close(fdFile);

       //creo la pipe per il processo nipote
       if(pipe(pipedNipote)<0){
        printf("Errore: problemi nella creazione della pipe\n");
        exit (3);
        }
        
        //creo il processo nipote
       printf("Sono il processo figlio di pid %d e sto per creare il mio processo nipote\n", getpid());
       pidNipote=fork();
            if(pidNipote<0){
            printf("Errore nella fork\n");
            exit (-1);        
            }
        
            if(pidNipote==0){
                //il processo nipote chiude il lato di lettura
                close(pipedNipote[0]);
                //ridireziono output sulla pipe e lo standard errore su /dev/null
                close(1);   dup(pipedNipote[1]);    close(pipedNipote[1]);
                close(2);   open("/dev/null", O_WRONLY);

                //apro il file da riordinare
                if((fdFile=open(argv[i+1], O_RDONLY ))<0){
                printf("Errore: impossibile aprire in lettura il file %s\n", argv[i+1]);
                exit (-1);
                } 
                                
                //faccio exec e ordino il file
                //printf("Sono il processo nipote di pid %d e provo a eseguire sort\n", getpid());
                execlp("sort", "sort", "-f", argv[i+1], (char *)0);

                printf("Errore: exec fallita\n");
                exit(-1);                
            }
        
        
        if ((pidNipote=wait(&status)) < 0){
                printf("Errore wait\n");
                exit(5);
        }
        if ((status & 0xFF) != 0){
                //compilo il campo c1 
                strutturaDati.c1=pidNipote;
                printf("Nipote con pid %d terminato in modo anomalo\n", pidNipote);
        }
        else
        {       //compilo il campo c1 
                strutturaDati.c1=pidNipote;

                ritorno=(int)((status >> 8) & 0xFF);
                printf("Il nipote con pid=%d ha ritornato %c (%d se 255 problemi!)\n", pidNipote, ritorno, ritorno);
        }
        //chiudo il lato di scrittura di pipedNipote
        close(pipedNipote[1]);

        //compilo il campo c3
        int j=0;
        while(read(pipedNipote[0], &strutturaDati.c3[j], 1)){
            if(strutturaDati.c3[j]=='\n'){
                ++j;
                break;
            }
            ++j;
        }

        //compilo il campo c2
        strutturaDati.c2=j;

        //passo la struct al padre
        write(pipedFiglio[i][1], &strutturaDati, sizeof(strutturaDati));

        //exit e ritorno la lunghezza della linea, terminatore di linea non compreso
        --j; exit(j);  
    }
}

    //il padre legge le N struct e le scrive in output
    for(int i=0; i<N; ++i){
        close(pipedFiglio[i][1]);
        read(pipedFiglio[i][0], &strutturaDati, sizeof(strutturaDati));
        printf("Struttura dati del file %s:\n", argv[i+1]);
        printf("PID nipote: %d, lunghezza linea(compreso il terminatore di linea): %d\n", strutturaDati.c1, strutturaDati.c2);
        strutturaDati.c3[strutturaDati.c2-1]=0;
        printf("Prima stringa del file: %s\n---------------------------\n", strutturaDati.c3);
    }
    
    //il padre aspetta tutti i figli
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
                printf("Il figlio con pid=%d ha ritornato %c (%d se 255 problemi!)\n", pidFiglio, ritorno, ritorno);
        }
    }

    exit (0);    
}