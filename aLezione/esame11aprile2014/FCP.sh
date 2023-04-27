#!/bin/sh

#il numero di parametri deve essere maggiore o uguale a 2
if test $# -lt 2
then 
echo Errore: il numero di parametri deve essere almeno 2
exit 1
fi

#controllo che i parametri siano directory traversabili
for i
do
	case $i in 
	/*)
		if test ! -d $i -o ! -x $i
		then 
		echo Errore: i parametri devono essere nomi assoluti di directory traversabili
		exit 2
		fi;;
	*)
	echo Errore: i parametri devono essere nomi assoluti
	exit 2;;
	esac
done

#creo un file temporaneo per ognuna delle N gerarchie e nel mentre chiamo N volte il file FCR.sh
#ma prima sistemo il PATH
PATH=`pwd`:$PATH
export PATH

n=1

for i in $*
do
	> /tmp/lista$n		#creo il file temporaneo per questa gerarchia
	FCR.sh $i /tmp/lista$n	#chiamo il file ricorsivo
	n=`expr $n + 1`		#aggiorno N per la prossima chiamata
done


#dopo aver eseguito le N fasi, per ogni gerarchia faccio echo del nome e del numero di file trovati
#faccio echo del nome assouluto del primo file
#poi chiedo all'utente valore X delle prime X linee da riportare di questo file
#così per ogni file

n=1
for i in $*
do
	echo Nella gerarchia $i ho trovato `wc -l < /tmp/lista$n` file 
		for j in `cat /tmp/lista$n`
		do
		echo File: $j
		echo Digitare il numero delle prime X linee da visualizzare \(deve essere un numero compreso tra 1 e `wc -l < $j`\)
	
		read risposta
		#controllo che sia stato immesso un numero e che sia corretto
		case $risposta in
		*[!0-9]*) echo Errore: deve essere immesso un numero, positivo. Programma interrotto; exit 3;;
		*) if test $risposta -gt `wc -l < $j` -o $risposta -eq 0 
	   		then  echo Errore: bisogna inserire un numero che rispetti le caratteristiche
			rm /tmp/lista$n 
			exit 3
			fi;;
		esac

		#faccio echo delle prime X righe
		echo `head -$risposta $j`
		done
	
	#incremento n per poter usare il prossimo file tmp e elimino quello attuale
	rm /tmp/lista$n	
	n=`expr $n + 1`
done

echo DEBUG-Ho finito
