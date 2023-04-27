#!/bin/sh
case $# in		#controllo il numero di parametri
1)
echo E\' stato passate 1 paramentro
echo DEBUG-Ora scrivo in append il contenuto di StInput sul file $1.
cat >>$1;;
2)
echo Sono stati passati 2 parametri
echo DEBUG-Ora scrivo in append il contenuto di $1 su $2
cat <$1 >>$2;;
*)			#numero di prametri sbagliato e do errore
echo Sono stati passati $# parametri. Devono essere passati 1 o 2 parametri.
exit 1;;
esac

