#!/bin/sh

#mi sposto nella cartella da contorllare
cd $1

#controllo se si tratta di un file, quante linee contiene e se contiene Cx
#se corrisponde aggiorno il contatore e salvo il nome dei file nella variabile
numeroFile=0
nomeFile=

for i in *
do
	if test -f $i
	then
	numLinee=`wc -l < $i`
		if test $numLinee -eq $3
		then 
		grep $4 < $i > /dev/null 2>&1
			if test $? -eq 0
			then numeroFile=`expr $numeroFile + 1` 
			nomeFile="$nomeFile $i"
			fi
		fi
	fi
done

#se ho trovato la dir giusta stampo il suo nome e i parametri da usare per invocare la parte in C
if test $numeroFile -eq $2
then 
echo Directory che soddisfa i criteri: `pwd`
echo Per chiamare la parte in C biogna passare i prametri $nomeFile
fi

for i in *
do
	if test -d $i -a -x $i
	then
	$0 `pwd`/$i $2 $3 $4
	fi
done




