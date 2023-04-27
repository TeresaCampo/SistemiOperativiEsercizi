#!/bin/sh

#parametri passati: G gerarchia, H numero max file, M numero linee
#mi sposto nella dir
cd $1
cont=0
file=

for i in *
do
	if test -f $i
	then 
		case $i in
		??) nLinee=`wc -l < $i`
			if test $nLinee -eq $3
			then 
			file="$file $i"
			cont=`expr $cont + 1`
			fi;;
		*);;
		esac
	fi
done

if test $cont -ge 2 -a $cont -lt $2
then 
echo Directory trovata: `pwd`
echo Chiamo il programma C con parametri $file $M
fi

#parte ricorsiva
for i in *
do
	if test -d $i -a -x $i
	then $0 `pwd`/$i $2 $3
	fi
done
