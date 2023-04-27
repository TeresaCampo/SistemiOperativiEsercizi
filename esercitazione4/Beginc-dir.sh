#!/bin/sh

#d serve per memorizzare la directory in cui cercare(corrente o primo parametro)
#dDaCercare serve per memorizzare nome relativo di directory da cercare(secondo parametro)
d=
dDaCercare=

#controllo il numero di parametri
case $# in
1)
d=`pwd`
dDaCercare=$1;;
2)
d=$1
dDaCercare=$2;;
*)
echo Bisogna passare o 1 o 2 parametri
exit 1;;
esac

#controllo il formato in cui sono dati i parametri
#controllo che il file da cecare sia una directory
if test ! -d $dDaCercare
then 
echo Bisogna passare come parametro una directory da cercare
exit 2
fi

#controllo che la directory da cercare si a data in forma relativa semplice
case $dDaCercare in
*/*)
echo Errore: $dDaCercare deve essere fornito con nome relativo semplice
exit 2;;
esac

#controllo che la directory in cui cercare sia un nome assoluto
case $d in
/*) ;;
*)
echo Errore: la directory in cui cercare deve essere passata in nome assoulto
exit 3;;
esac

#cambio il PATH
PATH=`pwd`:$PATH
export PATH

#ora invoco il file ricorsivo
echo Inizio la ricerca
Cercadir.sh $d $dDaCercare
echo Finito






