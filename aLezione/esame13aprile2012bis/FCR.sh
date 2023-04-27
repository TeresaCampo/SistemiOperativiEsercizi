#!/bin/sh

#parametri: G gerarchia, fase, livello, file tmp, answer
#mi sposto nella dir G
cd $1

#controllo se sono nella fase A
if test $2 = A
then 

	#se sono arrivato a livello più profondo aggiorno il maxLiv
	maxLiv=`cat $4`
	if test $3 -gt $maxLiv
	then echo $3 > $4
	fi
fi

if test $2 = B
then 
	#se sono al livello giusto, salvo il nome della dir in cui mi trovo nel file tmp
	if test $3 -eq $5
	then echo `pwd` >> $4
	fi
fi

#parte ricorsiva
for i in *
do
	if test -d $i -a -x $i
	then
	nextLiv=`expr $3 + 1`
	$0 `pwd`/$i $2 $nextLiv $4 $5	#chiamo FCR per la dir trovata
	fi
done
