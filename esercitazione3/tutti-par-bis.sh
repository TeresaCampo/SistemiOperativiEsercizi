#!/bin/sh

if test $# -lt 1	#il numero di parametri deve essere maggiore o uguale a 1
then 
echo Errore: bisogna passare almeno un parametro
exit 1
fi

echo La lista di tutti i parametri è: $*
echo L\'elenco di tutti i parametri è

counter=1		#serve per indicare il nome dei parametri
for i in $*
do
echo Parametro \$$counter= $i
counter=`expr $counter + 1`
done

