#!/bin/sh

#parametri: gerarchia, numero linee X e file tmp
#mi sposto nella dir corretta
cd $1

righe=1
dir=0
for i in *
do
	if test -d $i
	then
	dir=1
	fi
	
	if test -f $i 
	then 
	nl=`wc -l < $i`
		if test $nl -le $2
		then 
		righe=0
		fi
	fi
done

if test $righe -eq 1 -a $dir -eq 0
then 
echo `pwd` >> $3
fi

#parte ricorsiva
for i in *
do
	if test -d $i -a -x $i
	then $0 `pwd`/$i $2 $3
	fi
done
