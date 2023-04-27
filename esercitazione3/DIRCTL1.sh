#!/bin/sh
if test $# -ne 1	#controllo il numero di parametri, se non è uguale a 1 do un errore
then 
echo Errore: il numero di parametri deve essere 1
exit 1

fi
echo SONO DIRCTL.sh
echo 'Il valore di $0===>' $0
echo 'Il valore di $1===>' $1

if test -f $1		#controllo se è un file
then
echo $1 è un file
echo "DEBUG-Ora eseguo ls -l $1"
ls -l $1

else
if test -d $1		#controllo se è una directory
then
echo $1 è una directory
echo "DEBUG-Ora eseguo ls -ld $1"
ls -ld $1

else			#altrimenti non esiste
echo Non è nè un file nè una directory
fi

fi
