#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(){
int ret;

printf("Si vuole fare una chiamta ricorsiva di questo programma? (No=0, Si=tutti gli altri numeri)\n");
scanf("%d", &ret);

if(ret!=0){
	execlp("./prova", "prova", (char *) 0);

	printf("Errore in execlp\n");
	exit (1);
}
else{	exit (0);}

}
