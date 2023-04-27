#!/bin/sh
if test $# -ne 1	#controllo il numero di parametri, se diverso da 1 do errore
then 
echo Errore: numero di parametri deve essere 1.
exit 1
fi

case $1 in		#controllo il formato del parametro
*/*) 
echo Nome assoluto oppure nome relativo. Mi serve un nome relativo semplice. 
exit 1;;
*) echo Nome relativo semplice;;
esac

if test -f $1		#controllo se il parametro è un file e si trova nella dir corrente
then 
echo $1 è un file della directory corrente.
echo DEBUG-Ora stampo il nome assoluto del file $1
echo `pwd`/$1
else
echo $1 non è un file o non si trova nella direcotry corrente.
fi


