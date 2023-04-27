#!/bin/sh
for i in p*
do
#mostro l'elemento che ho trovato
echo $i

#se si tratta di una direcotry lo scrivo
if test -d $i
then echo si tratta di una directory

#se si tratta di un file stampo il contenuto
else	if test -f $i
	then 
	cat $i
	fi
fi
done
