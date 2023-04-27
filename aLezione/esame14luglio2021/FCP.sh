#!/bin/sh

#numero parametri: almeno 4
if test $# -lt 4
then echo Errore: il numero di parametri deve essere almeno 4; exit 1
fi

#i primi parametri devono essere nomi assoluti di directory traversabili, G
#gli ultimi due devono essere numeri interi positivi, H e M
ultimiPar=`expr $# - 1`
c=1
for i 
do
	if test $c -lt $ultimiPar
	then 
		case $i in
		/*) if test ! -d $i -o ! -x $i
		then echo Errore: i primi parametri devono essere directory traversabili; exit 2; fi;;
		*) echo Errore: i primi parametri devono essere nomi assoluti; exit 2;;
		esac
	fi

	if test $c -ge $ultimiPar
	then
		case $i in
		*[!0-9]*) echo Errore: gli ultimi 2 parametri devono essere numeri strettamente positivi; exit 3;;
		*) if test $i -eq 0
		then echo Errore: gli ultimi 2 parametri devono essere numeri strettamente positivi; exit 3; fi;;
		esac
			if test $c -eq $ultimiPar
			then H=$i
			fi

			if test $c -eq $#
			then M=$i
			fi
		fi
	c=`expr $c + 1`
done

#imposto il PATH 
PATH=`pwd`:$PATH
export PATH

c=1
for i 
do
	if test $c -lt $ultimiPar
	then 
	G=$i

	FCR.sh $G $H $M
	fi
	c=`expr $c + 1`
done

echo FINITO
