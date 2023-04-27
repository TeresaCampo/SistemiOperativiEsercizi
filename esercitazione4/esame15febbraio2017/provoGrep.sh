#!/bin/sh

#numero linee di un file
nl=`wc -l < $1`
nlConCarattere=`grep [0-9] $1 | wc -l  `

echo $nl
echo $nlConCarattere

