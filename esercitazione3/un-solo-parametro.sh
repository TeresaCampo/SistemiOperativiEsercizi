#!/bin/sh
if test $# -ne 1		#controllo il numero di parametri, se diverso da uno do errore
then 
echo Errore: il numero di parametri deve essere 1
exit 1
fi

case $1 in		#controllo il formato del nome del file
/*) echo Nome assoluto;;
*/*) echo Nome relativo;;
*) echo Nome relativo semplice;;
esac


