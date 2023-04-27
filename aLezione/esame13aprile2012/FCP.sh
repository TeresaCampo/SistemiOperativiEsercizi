#!/bin/sh

#un solo parametro
if test $# -ne 1
then 
echo Errore: il numero di parametri deve essere 1
fi

#il parametro deve essere il nome assouluto di una directory traversabile
case $1 in
	/*) 
	if test ! -d $1 -o ! -x 
	then echo Errore: il parametro deve essere una directory traversabile; exit 1; fi;;
	*)
	echo Errore: il parametro deve essere un nome assoluto; exit 1;;
esac

#variabili per il file FCR.sh
numLivello=1
fase=A

#imposto PATH, creo il file tmp e chiamo il file ricorsivo
PATH=`pwd`:$PATH
export PATH

echo  0 > /tmp/lMax$$
FCR.sh $1 $numLivello /tmp/lMax$$ $fase

read nLivelli < /tmp/lMax$$
echo Il numero di livelli totali della gerarchia sono $nLivelli
echo Indica il numero di ivello pari che vuoi visulaizzare \(tra 1 e $nLivelli\)

read risposta
case $risposta in
*[!0-9]*) echo Errore: il parametro passato non rispetta le caratteristiche
rm /tmp/lMax$$
exit 3;;
*)if test $risposta -lt 1 -o $risposta -gt $nLivelli -o `expr $risposta % 2` -ne 0
then echo Errore: parametro passato non rispetta i criteri 
rm /tmp/lMax$$
exit 3
fi;;
esac

fase=B
FCR.sh $1 $numLivello $risposta $fase

