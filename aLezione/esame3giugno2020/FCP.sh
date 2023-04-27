#!/bin/sh

#deve avere almeno 3 parametri
if test $# -lt 3
then echo Errore: devono essere passati almeno 3 parametri; exit 1
fi

#il primo parametro dece essere un singolo carattere, C
C=$1

case $C in
?);;
*) echo Errore: il primo parametro deve essere un singolo carattere; exit 2;;
esac

#gli altri parametri devono essere nomi assoluti di directory traversabili
shift
G=$*

for i
do
	case $i in
	/*) if test ! -d $i -o ! -x $i
	    then echo Errore: devono essere passati nomi assoluti di directory traversabili; exit 3; fi;;
	*) echo Errore: devono essere passati nomi assoluti di directory; exit 3;;
	esac
done

#imposto il PATH e creo il file tmp
PATH=`pwd`:$PATH
export PATH

> /tmp/nomiAssoluti

#eseguo le N fasi
for i
do
	FCR.sh $i $C /tmp/nomiAssoluti
done

echo Trovate `wc -l < /tmp/nomiAssoluti` directory
echo Mostro tutte le directory

for i in `cat /tmp/nomiAssoluti`
do
	echo Directory: $i
	echo Elena, vuole visualizzare il contenuto di questa directory? \(si/no\)
	read answer
	case $answer in
	si) ls -A $i;;
	*);;
	esac
done

rm /tmp/nomiAssoluti
