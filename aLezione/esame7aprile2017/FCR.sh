#!/bin/sh

#primo parametro G, secondo F, terzo file tmp
#mi sposto nellla dir giusta
cd $1

for i in `ls -A`
do
if test -f $i -a -r $i
then 
	case $i in
	$2) sort -f $i > sorted
	echo `pwd`/sorted >> $3;;
	esac
fi
done


#parte ricorsiva
for i in *
do
	if test -d $i -a -x $i
	then $0 `pwd`/$i $2 $3 
	fi
done
