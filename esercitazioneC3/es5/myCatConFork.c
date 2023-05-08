#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char ** argv){
//pid processo figlio, status e ritorno da controllare dopo aver fatto la wait
int pid, status, ritorno;

//Controllo sui parametri: 1 solo parametro
if(argc!=2){
printf("Errore: numero di parametri deve essere 1, non %d\n", argc-1);
exit (1);
}

//Sto simulando una shell: creo processo figlio che eseguirà il comando cat
if( (pid=fork())<0){
	printf("Errore: problemi nella fork\n");
	exit (2);
}
if(pid==0){
	//ridireziono lo Std Input, lo sostituisco con il file passato come parametro
	close(0);
	if( (open(argv[1], O_RDONLY))<0){
	printf("Errore: problemi nell'apertura del file %s\n", argv[1]);
	exit (-1);
	}
	//execlp
	printf("Provo a eseguire cat del file %s\n", argv[1]);
	execlp("mycat", "mycat", (char *) 0);

	//se fallisce exec, lo dico e segnalo problema al processo padre
	printf("Errore: problemi nell'execlp\n");
	exit (-1);
}

pid=wait(&status);
if(pid <0){
	printf("Errore: problemi nella wait\n");
	exit (3);
}
if( (status & 0xFF) != 0){
	printf("Il processo figlio con pid %d è terminato in modo anomalo\n", pid);
}
else{	ritorno= (int) ((status >> 8) & 0xFF);
	printf("Il figlio con pid %d ha ritornato %d (se 255= problema con la execlp)\n", pid, ritorno);
}

exit (0);
}
