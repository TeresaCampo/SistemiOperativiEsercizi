#!/bin/sh
case $# in		#controllo il numero di parametri passati
0)			#se numero di parametri è 0 mostro informazioni di tutti file e directory della dir corrente
echo Il numero di parametri è 0
echo DEBUG-Ora eseguo 'ls -l' paginato nella directory corrente 
ls -l | more
exit 0;;

1) 			#se il numero di parametri è 1, tutto come prima
echo Il numero di parametri è 1;;
*)			#se il numero di parametri è maggiore di 2 do errore
echo Errore: numero di parametri deve essere 0 o 1;;
esac 

echo SONO DIRCTL.sh
echo 'Il valore di $0===>' $0
echo 'Il valore di $1===>' $1

if test -f $1		#controllo se è un file
then
echo $1 è un file
echo "DEBUG-Ora eseguo ls -l $1"
ls -l $1
else 
	if test -d $1	#controllo se è una directory
	then
	echo $1 è una directory
	echo DEBUG-Ora eseguo 'ls -ld'
	ls -ld $1
	else 
	echo $1 non è nè un file nè una directory
	fi
fi
