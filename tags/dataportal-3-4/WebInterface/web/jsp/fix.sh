#!/usr/bin/bash
for f in `ls *.jsp`
do
	sed "s//\n/g" $f > $f.n
	mv $f.n $f
done
