#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
//il numero di parametri deve essere almeno 1(quindi argc almeno 2)
if (argc < 2) {
	printf("Errore nel numero di parametri, argc= %d e dovrebbe essere almeno 2\n", argc);
	exit(1);
	}
//numero di parametri
int N=argc-1;

//mostro il nome dell'eseguibile e il numero di parametri
printf("Il nome dell'eseguibile è %s. Il numero di parametri è %d.\n", argv[0], N);

//mostro il valore dei singoli parametri
for(int i=0; i<N; i++){
int indice=i+1;
printf("Parametro %d) %s\n",indice, argv[i+1]); 
}

exit(0);
}
