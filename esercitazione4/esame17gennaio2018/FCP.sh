#!/bin/sh

#controllo il numero di parametri passati, deve essere 4
case $# in
4) ;;
*)
echo Errore: devono essere passati 4 parametri
exit 1;;
esac

#creo variabili per contenere i parametri
#G contiene la directory da cui inizio a cercare
#N contiene il numero di file che devono avere certe caratteristiche
#H contiene il numero di linee che devono contenere i file
#Cx contiene il carattere che deve essere presente almeno una volta nei file
G=$1
N=$2
H=$3
Cx=$4

#controllo che G sia una directory in nome assoulto
if test -d $G
then 
	case $G in
	/*);;
	*)
	echo Errore: la gerarchia in cui cercare deve essere un nome assoluto
	exit 2;;
	esac
fi


#controllo che N e H siano numeri interi positivi
for i in $2 $3
do
	if test $i -le 0
	then
	echo Errore: N e G devono essere strettamente positivi
	exit 3
	fi
done


#controllo che Cx sia un singolo carattere
case $Cx in
?);;
*)
echo Errore: Cx deve essere un singolo carattere
exit 4;;
esac

#imposto PATH
PATH=`pwd`:$PATH
export PATH
echo Inizio la ricerca
FCR.sh $1 $2 $3 $4
echo Ho finito
