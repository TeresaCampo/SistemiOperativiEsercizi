#!/bin/sh
if test $# -ne 1
then 
echo "Il numero di parametri deve essere 1"
exit 1

else
echo SONO DIRCTL.sh
echo 'Il valore di $0===>' $0
echo 'Il valore di $1===>' $1
echo "DEBUG-Ora eseguo ls -l $1"
ls -l $1

fi
