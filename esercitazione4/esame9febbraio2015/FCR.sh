#!/bin/sh

#cambio direcotry 
cd $1

#variabile in cui insierisco il nome assoluto dei file che rispettano i prametri
trovato=
for i in *
do
	if test -f $i -a -r $i			#è un file leggibile
	then 
	nl=`wc -l < $i`
		if test $nl -eq $2		#ha il giusto numero di linee
		then 
		trovato=true
		echo `pwd`/$i >> $3
		fi
	fi
done

#se ho trovato almeno un file faccio echo del nome assouluto della dir e salvo su un file tmp i file trovati
if test $trovato=true
then 
echo Directory: `pwd`
fi

#parte ricorsiva
for i in *
do
	if test -d $i -a -x $i
	then 
	$0 `pwd`/$i $2 $3
	fi
done




