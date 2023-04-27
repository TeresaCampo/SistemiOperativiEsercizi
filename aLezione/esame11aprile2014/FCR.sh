#!/bin/sh

#mi sposto nella direcottory in cui cercare
cd $1

for i in *
do
	if test -f $i -a -r $i
	then 
	nl=`wc -l < $i`
	nlCona=`grep '^X' $i | wc -l`
		if test $nlCona -eq $nl -a $nl -ne 0	#ho trovato un file adeguato
		then echo "`pwd`/$i" >> $2
		fi
	fi
done

#parte ricorsiva
for i in *
do
	if test -d $i -a -x $i
	then 
	$0 `pwd`/$i $2
	fi
done
