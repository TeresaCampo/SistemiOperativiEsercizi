#!/bin/sh

if test $# -ne 2	#controllo in modo stretto il numero di parametri, se diverso da 2 do errore
then 
echo Errore: sono stati passati $# parametri ma ne servono 2.
exit 1

fi

for i in $*		#entrambi i parametri devono essere nomi relativi semplici
do 
	case $i in
	*/*) 			#non è un nomer relarivo semplice, do errore
	echo Errore: il parametro $i non è un nome relativo semplice
	exit 1;;
	*);;
	esac
done
if test ! -f $1 -o ! -r	#il primo parametro deve esistere ed essere leggibile
then 
echo Errore: $1 o non esiste o non è un file leggibile
exit 1
fi

if test -f $2 -a ! -w	#il secondo file se esiste deve essere scrivibile
then 
echo Errore: $2 deve essere scrivibile
exit 1
fi

if test ! -f $2 -a ! -w .	#se il secondo file non esiste la directory corrente deve essere scrivibile
then 
echo Errore: la directory corrente deve essere scrivibile
exit 1
fi

echo DEBUG-Ora eseguo append del contenuto di $1 in $2
cat <$1 >>$2






