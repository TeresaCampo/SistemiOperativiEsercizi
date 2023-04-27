#!/bin/sh
if test $# -lt 1	#deve avere almeno 1 parametro
then 
echo Bisogna passare almeno un carattere
exit 1
fi

for i in $*
do
	#controllo che il primo parametro sia un file e sia leggibile
	if test ! -f $i -o ! -r $i
	then
	echo $i o non è un file o non è leggibile
	exit 2
	fi

	#se il primo parametro è un file leggibile chiedo all'utente se lo vuole visualizzare
	echo Vuoi visualizzare il file $i? "(S/N)"
	read risposta

	case $risposta in
	S) 
	echo DEBUG-Ora mostro le info sul file $i
	ls -la $i
	cat $i;;
	*) 
	echo Allora abbiamo già finito;;
	esac
done

