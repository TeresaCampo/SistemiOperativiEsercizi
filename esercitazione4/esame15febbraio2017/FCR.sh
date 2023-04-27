#!/bin/sh

#mi sposto nella directory giusta
cd $1
numeroFile=0
nomeFile=

#controllo se i file rispettano i criteri
for i in *
do
	if test -f $i -a -r $i
	then nl=`wc -l < $i` 
		if test $nl -eq $2
		then nlConNumeri=`grep [0-9] < $i | wc -l `
			if test $nlConNumeri -eq $2 
			then
			numeroFile=`expr $numeroFile + 1`
			nomeFile="$nomeFile $i"
			fi
		fi
	fi
done

#se il numeroFile è maggiore o uguale a 1 faccio echo del nome assoulto della dir e i file che corrispondono ai criteri 
if test $numeroFile -ge 1
then
echo `pwd`
echo $nomeFile
fi

#parte ricorsiva
for i in *
do
	if test -d $i -a -x $i
	then 
	$0 `pwd`/$i $2
	fi
done
