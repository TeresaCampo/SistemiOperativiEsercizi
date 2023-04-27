#!/bin/sh

#parametri passati: G gerarchia, C carattere, tmp file temporaneo
#mi sposto nella gerarchia G
cd $1

#controllo se la directory contiene directory con nome richiesto
for i in *
do
	if test -d $i 
	then 
		case $i in
		$2?$2) echo `pwd`/$i >> $3;;
		*);;
		esac
	fi
done

#parte ricorsiva
for i in *
do
	if test -d $i -a -x $i
	then $0 `pwd`/$i $2 $3
	fi
done

