#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <ctype.h>
#define PERMESSI 0644

int main(int argc, char ** argv){

    /*-------------*variabili locali-------------*/
int Q;              //contiene il numero di file
int q;              //indice processi figli
char linea[250];    //contiene linea letta supponendo max lunghezza 250 caratteri
int fdCamilla;      //contiene fd del file creato
int Ff;             //contiene fd del file associato a ogni figlio
typedef int pipe_t[2];
pipe_t *pipedFp;        //per contenere pipe da figli a padre
int pidFiglio, status, ritorno; //per fork e wait
int pos;            //posizione nella linea
int lineeInviate;   //numero di linee inviate al padre da un figlio
    /*-------------------------------------------*/

//controllo: almeno 2 parametri
if(argc<3){
    printf("Errore: passati %d parametri, ma ne servono almeno 2\n", argc-1);
    exit (1);
}
Q=argc-1;

//il padre crea file Camilla nella dir corrente
fdCamilla=creat("Camilla", PERMESSI);
if(fdCamilla<0){
    printf("Errore: problemi nella creazione del file Camilla\n");
    exit (2);
}

//il padre crea N pipe
pipedFp=malloc(Q*sizeof(pipe_t));
for(int i=0; i<Q; ++i){
    if(pipe(pipedFp[i])<0){
        printf("Errore: problemi nella creazione della pipe\n");
        exit (2);
    }
}

//il padre crea N figli
for(q=0; q<Q; ++q){
    pidFiglio=fork();
        if(pidFiglio<0){
        printf("Errore: problemi nella fork\n");
        exit (3);
        }
        if(pidFiglio==0){
            //figlio chiude i lati della pipe: tutti in lettura, tutti tranne il suo in scrittura
            for(int j=0; j<Q; ++j){
                close(pipedFp[j][0]);
                if(q!=j) {
                    close(pipedFp[j][1]);
                }
            }

            //figlio prova ad aprire il file associato
            if((Ff=open(argv[q+1], O_RDONLY ))<0 ){
            printf("Errore: impossibile aprire in lettura il file %s\n", argv[q+1]);
            exit (-1);
            }

            pos=0; lineeInviate=0;
            while(read(Ff, &linea[pos], 1)>0){
                if(linea[pos]=='\n'){
                    //se il primo carattere della linea è un numero E pos <9
                    if(isdigit(linea[0])!=0 && pos<9){
                        write(pipedFp[q][1], linea, sizeof(linea));
                        write(fdCamilla, linea, pos+1);
                        ++lineeInviate;
                    }
                    pos=0;
                }
                else{
                    ++pos;
                }
            }

            exit(lineeInviate);
        }
}
//padre chiude lati della pipe: tutti i lati di scrittura
for(q=0; q<Q; ++q){
    close(pipedFp[q][1]);
}

//per ogni file, riceve tutte le linee dai figli
for(q=0; q<Q; ++q){
    while(read(pipedFp[q][0], linea, sizeof(linea))){
        //rendo la linea una stringa
        for(pos=0; pos<250; ++pos){
            if(linea[pos]=='\n'){
                linea[pos]=0;
            }
        }
        printf("Il figlio di indice %d lavora sul file %s e ha mandato la linea: %s\n", q, argv[q+1], linea);
    }

}

//il padre aspetta tutti i figli
    for(q=0; q<Q; ++q){
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