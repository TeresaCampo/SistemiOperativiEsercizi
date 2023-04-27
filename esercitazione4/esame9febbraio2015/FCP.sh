#!/bin/sh

#controllo in modo stretto il numero dei parametri
case $# in
2);;
*)
echo Errore: devono essere passati 2 parametri
exit 1;;
esac

#controllo la forma dei 2 parametri
G=$1
K=$2

#il primo parametro, G, deve essere il nome assoulto di una directory
case $G in
/*)
if test ! -d $G -o ! -x $G
then
echo Errore: il primo parametro deve essere una directory traversabile
exit 2
fi;;
*)
echo Errore: il primo parametro deve essere un nome assoulto
exit 3;;
esac

#il secondo carattere deve essere un numero intero strettamente positivo
case $K in
*[!0-9]*)
echo Errore: il secondo parametro deve essere un numero positivo
exit 4;;
*) 
if test $K -eq 0
then 
echo Errore: il secondo parametro deve essere un numero positivo
exit 4
fi;;
esac

#imposto il PATH
PATH=`pwd`:$PATH
export PATH

#creo il file tmp e invoco la ricerca ricorsiva
> /tmp/tmp$$

echo Debug: inizio la ricerca ricorsiva
FCR.sh $1 $2 /tmp/tmp$$
echo Debug: ricerca terminata

#faccio echo dei parametri con cui invocare i parametri della parte in C
parametri=

for i in `cat /tmp/tmp$$`
do
echo Inserire un numero compreso tra 1 e $K
read risposta
case $risposta in
*[!0-9]) 
echo Errore: il valore insierito non è un numero o non ha le giuste caratteristiche
echo Programma interrotto

rm /tmp/tmp$$		#rimuovo il file temporaneo
exit 5;;
*)
if test $risposta -eq 0 -o $risposta -gt $K
then 
echo Errore: il valore inserito non è un numero o non ha le giuste caratteristiche
echo Programma interrotto

rm /tmp/tmp$$
exit 5
fi;;
esac

parametri="$parametri $i $risposta"
done

rm /tmp/tmp$$
echo I parametri da passare alla chiamata C sono $parametri


