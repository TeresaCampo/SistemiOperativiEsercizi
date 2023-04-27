#!/bin/sh

#mi sposto nella dir in cui cercare
cd $1

if test $4 = "A"
then
	nLivello=$2
	read nMax < $3
		#se sono arrivato a un livello più profondo del nMax, aggiorno nMax sul file tmp
		if test $nLivello -gt $nMax
		then 
		echo $nLivello > $3
		fi
fi

for i in *
do
	if test -d $i -a -x $i
	then 
	nLivello=`expr $2 + 1`
	FCR.sh `pwd`/$i $nLivello $3 $4
	nLivello=$2
	fi
done

#ho terminato la fase A
if test $4 = "B" 
then 

	if test $2 -eq $3
	then 
	echo Directory `pwd`
	ls -l `pwd`

	for i in *
	do
	ls -l $i
	done
	fi
fi



