#!/bin/sh

#il numero di parametri deve essere maggiore o uguale a 4
if test $# -lt 4
then echo Errore: il numero di parametri deve essere 4; exit 1
fi


#il primo parametro deve essere un numero intero strettamente positivo
W=$1
case $W in
*[!0-9])echo Errore: il numero deve essere un numero strettaenete positivo; exit 2;;
*) if test $W -eq 0
then echo Errore: il numero deve essere strettamente poitivo; exit 2
fi;;

#il secondo parametro deve essere una stringa quindi non deve contenere /
S=$2

case $S in
*/*) echo Errore: la stringa non deve contenere /; exit 3;;
*);;
esac

#controllo gli ultimi Q parametri, devono essere nomi assoluti di directory traversabili
shift
shift
Q=$*

for i 
do
	case $i in
	/*) if test ! -d $i -o ! -x
	then echo Errore: gli ultimi parametri devono essere nomi assoluti di directory traversabili; exit 4
	fi;;
	*) echo Errore; gli ultimi parametri devono essere nomi assoluti di directory traversabili; exit 4;;
	esac
done

#imposto il PATH
PATH=`pwd`:$PATH
export PATH

#creo il file tmp
> /tmp/listaDir$$

for i 
do
	FCR.sh $i $S /tmp/listaDir$$
done


