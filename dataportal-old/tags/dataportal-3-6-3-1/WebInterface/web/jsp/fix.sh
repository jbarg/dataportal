#!/usr/bin/bash
for f in `ls *.jsp`
do
	sed "s/
	mv $f.n $f
done