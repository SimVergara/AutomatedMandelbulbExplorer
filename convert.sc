#!/bin/bash
shopt -s nullglob nocaseglob
for i in *.bmp
do
	echo convert $i
	newname=${i%.*}.jpg
	convert "$i" "$newname"
	rm -rf $i
done