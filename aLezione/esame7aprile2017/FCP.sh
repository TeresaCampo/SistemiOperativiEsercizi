#!/bin/sh

#i parametri devono essere almeno 3
if test $# -lt 3
then echo Errore: il numero di parametri deve essere almeno 3; exit 1
fi

#il primo parametro deve essere un nomer relativo semplice di file
F=$1

case $F in
*/*) echo Errore: il file deve essere un nome relativo semplice; exit 2;;
esac

#gli altri parametri devono essere nomi assoluti di directori traversabili
shift
G=$*

for i 
do	case $i in
	/*)
	if test ! -d $i -o ! -x $i
	then echo Errore: i parametri dal secondo in poi devono essere nomi di directory traversabili; exit 3
	fi;;
	*) echo Errore: i parametri dal secondo in poi devono essere nomi assoluti; exit 3;;
	esac
done

#imposto PATH
PATH=`pwd`:$PATH
export PATH

#creo file tmp
> /tmp/lista$$

for i
do
	FCR.sh $i $F /tmp/lista$$
done

echo Sono stati creati `wc -l < /tmp/lista$$` file
echo Stampo l\'elenco dei file creati

for i in `cat /tmp/lista$$`
do
	echo File: $i
	echo Prima linea: 
	head -1 $i
	echo Ultima linea: 
	tail -1 $i
done


rm /tmp/lista$$
echo FINITO
