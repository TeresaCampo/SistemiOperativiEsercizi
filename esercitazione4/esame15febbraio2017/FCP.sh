#!/bin/sh

#controllo il numero di parametri
case $# in
2);;
*)
echo Errore: bisogna passare 2 parametri
exit 1;;
esac

#controllo la forma dei caratteri
#il primo deve essere il nome assoluto di una directory
case $1 in
/*);;
*)
echo Errore: il primo parametro deve essere un nome assoluto
exit 2;;
esac

if test ! -d 
then 
echo Errore: il primo parametro deve essere una directory
exit 3
fi

#il secondo carattere deve essere un numero intero positivo
case $2 in
*[!0-9]*)
echo Errore: il secondo parametro deve essere un numero intero positivo
exit 4;;
esac

#se sono arrivato fino a qui i parametri vanno bene e li assegno alle variabili
G=$1
X=$2

#imposto il PATH
PATH=`pwd`:$PATH
export PATH

#chiamo il file ricorsivo
echo Inizio la ricerca
FCR.sh $1 $2
echo Ho finito


