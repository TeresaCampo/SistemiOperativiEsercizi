#!/bin/sh

#Numero parametri: almeno 4
if test $# -lt 4
then echo Errore: devono essere passati almeno 4 parametri; exit 1
fi

str1=$1
str2=$2

#controllo che str1 sia una semplice stringa
case $str1 in
*/*) echo Errore: str1 non è una semplice stringa; exit 2;;
esac


#controllo che la str2 sia una semplice stringa
case $str2 in
*/*)echo Errore: str2 non è una semplice stringa; exit 2;;
esac

#faccio in modo di avere solo le Z gerarchie come parametri
shift
shift

#controllodi avere nomi assoluti di directory traversabili
for dirAss 
do
	case $dirAss in
	/*) if test ! -d $dirAss -o ! -x $dirAss
	then echo Errore:gli ultimi parametri non sono nomi assoluti di directory traversabili; exit 3
	fi;;
	esac
done

#arrivati qui i parametri sono corretti
#imposto il PATH e creo il file tmp
PATH=`pwd`:$PATH
export PATH

> /tmp/absoluteDirs

#eseguo le Z fasi
for dirAss
do
	FCR.sh $dirAss $str1 $str2 /tmp/absoluteDirs
done

#arrivati qui nel file tmp ci sono i nomi assoluti di tutti i file trovati
SOMMA1=`grep "$str1$" < /tmp/absoluteDirs | wc -l` 
SOMMA2=`grep "$str2$" < /tmp/absoluteDirs | wc -l`

echo SOMMA1=	$SOMMA1
echo SOMMA2=  $SOMMA2
#se SOMMA2 è maggiore di SOMMA1 oppure se è uguale a 0 non ho nulla da stampare, ho finito
#se SOMMA2 è minore  SOMMA1 chiedo all'utente un numero, controllo che la risposta rispetti i parametri e poi stampo il nome dell N-esimo file con terminazione str1 e con terminazione str2
if test $SOMMA2 -gt $SOMMA1 -o $SOMMA2 -eq 0
then echo SOMMA2 è maggiore di SOMMA1 oppure SOMMA2 è uguale a 0, quindi abbiamo FINITO
fi

if test $SOMMA2 -le $SOMMA1 -a $SOMMA2 -gt 0
then 

echo Teresa, scegli un numero intero compreso tra 1 e $SOMMA2
read N

#controllo che la risposta sia un numero positivo compreso tra 1 e SOMMA2
case $N in
	*[!0-9]*) echo Errore: la risposta non rispetta le richieste; rm /tmp/absoluteDirs; exit 4;;
	*) if test $N -lt 1 -o $N -gt $SOMMA2
	then echo Errore: la risposta non rispetta le richieste; rm /tmp/absoluteDirs; exit 4
	fi;;
esac
 
#stampo il file con estensione str1
echo Il file numero $N con estensione $str1 è `grep "$str1$" < /tmp/absoluteDirs | head -$N | tail -1`
echo Il file numero $N con estensione $str2 è `grep "$str2$" < /tmp/absoluteDirs | head -$N | tail -1`
fi

#rimuovo il file tmp
rm /tmp/absoluteDirs

echo FINITO
