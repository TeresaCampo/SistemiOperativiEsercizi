#!/bin/sh

#parametri: dirAss, str1, str2, file tmp
#mi sposto nella dir da studiare
cd $1

for file in *
do
	#controllo se è un file leggibile
	if test -f $file -a -r $file
	then 
	#controllo se termina con str1 o con str2 e nel caso li salvo nel file tmp
		case $file in
		*\.$2) echo $file; echo $file >> $4;;
		*\.$3) echo $file; echo $file >> $4;;
		esac
	fi
done


#parte ricorsiva
for dir in *
do
	if test -d $dir -a -x $dir 
	then $0 `pwd`/$dir $2 $3 $4
	fi
done
