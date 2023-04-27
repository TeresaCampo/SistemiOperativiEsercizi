#!/bin/sh
if test $# -ne 1		#controllo il numero di parametri, se diverso da uno do errore
then 
echo Errore: il numero di parametri deve essere 1
exit 1
fi

case $1 in		#controllo il formato del nome del file
/*) echo Nome assoluto
if test -f $1		#controllo se è un file
then echo $1 è un file
else
	if test -d $1 -a -x $1
	then echo $1 è una directory traversabile
	else echo $1 non è nè file nè directory
	fi
fi;;

*/*) echo Nome relativo;;
*) echo Nome relativo semplice;;
esac


