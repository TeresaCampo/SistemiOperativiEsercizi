#!/bin/sh

#deve avere almeno 3 parametri
if test $# -lt 3
then echo Errore: devono essere passati almeno 3 parametri; exit 1
fi

#i primi n parametri devono essere nomi assoluti di directory traversabili
#l'ultimo parametro deve essere un numero intero strettamente positivo

c=1

for i
do
	if test $c -lt $#
	then 

	case $i in 
	/*) if test ! -d $i -o ! -x $i
	then echo Errore: i primi parametri devono essere nomi assoluti di dir traversabili; exit 2
	fi;;
	*) echo Errore: i primi parametri devono essere nomi assoluti; exit 2;;
	esac

	fi

	if test $c -eq $#
	then 
		if test $i -le 0
		then echo Errore: l\'ultimo parametro deve essere un numero strettamente positivo; exit 2
		fi
		X=$i
	fi 
c=`expr $c + 1`
done

#imposto il PATH e creo il file tmp
PATH=`pwd`:$PATH
export PATH

> /tmp/found$$

c=1
for i 
do
	if test $c -lt $#
	then 
	FCR.sh $i $X /tmp/found$$	
	fi
	c=`expr $c + 1`
done


echo Ho trovato `wc -l < /tmp/found$$` dir
echo Li stampo

for i in `cat /tmp/found$$`
do
	echo Nome dir:$i
	for j in `ls $i`
	do
		echo Nome file:$i/$j
		echo X-esima riga dal basso:
		tail -$X $i/$j| head -1
		echo =====================================
	done
done

rm /tmp/found$$
