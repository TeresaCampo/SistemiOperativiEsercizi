#!/bin/sh

#1 solo parametro
if test $# -ne 1
then echo Errore: il numero di parametri deve essere 1; exit 1
fi

#il parametro deve essere il nome assoluto di una dir traversabile
G=$1

case $G in
/*) if test ! -d $G -o ! -x $G
then echo Errore: il parametro deve essere il nome assoluto di una dir traversabile; exit 2
fi;;
*) echo Errore: il parametro deve essere il nome assoluto di una dir traversabile; exit 2;;
esac

#creo variabile fase, file tmp e imposto il PATH
fase=A
> /tmp/maxLiv
echo 1 >> /tmp/maxLiv	#imposto il livello radice

PATH=`pwd`:$PATH
export PATH

#chiamo il file FCR per la fase A
FCR.sh $G $fase 1 /tmp/maxLiv

numLiv=`cat < /tmp/maxLiv`
rm /tmp/maxLiv
echo La gerarchia ha $numLiv livelli

#inizia la fase B
fase=B
echo Scegliere un numero pari tra 1 e $numLiv
read answer

case $answer in
*[!0-9]*) echo Errore: è stato inserito un valore errato; exit 3;;
*) if test $answer -lt 1 -o $answer -gt $numLiv
then echo Errore: è stato inserito un valore errato; exit 3
fi

resto=`expr $answer % 2`
if test $resto -ne 0 
then echo Errore: è stato inserito un livello dispari; exit 3
fi;;
esac

#creo file tmp in cui scrivere i nomi assoluti dei direttori in quel livello
> /tmp/dirLiv

#chiamo il file FCR per la fase B
FCR.sh $G $fase 1 /tmp/dirLiv $answer

#stampo tutte le informazioni di tutti gli elementi(anche file nascosti)
for i in `cat /tmp/dirLiv`
do
	echo Nella directory: $i
	ls -lA $i
done
rm /tmp/dirLiv

echo FINE
