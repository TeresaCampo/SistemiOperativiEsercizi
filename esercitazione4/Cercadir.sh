#!/bin/sh

#mi sposto nella cartella in cui devo eseguire la ricerca
cd $1
if test -d $2
then 
echo File $2 trovato, si trova nella posizione `pwd`
fi

#parte ricorsiva
for i in *
do

if test -d $i -a -x $i
then 
echo DEBUG-Ora entro nella direcory `pwd`/$i
Cercadir.sh `pwd`/$i $2
fi
done
