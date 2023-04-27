#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char ** argv){
int status;

printf("Il pid del processo corrente è %d\n", getpid());
int pid=wait(&status);
if(pid<0){
printf("Errore: nessun processo figlio da aspettare\n");
}
}
