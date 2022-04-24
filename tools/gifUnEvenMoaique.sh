#!/bin/bash
fich=$1
Var=1
mkdir .tmp
for (( x=90; x>0; x-=5 ))
do
    echo $Var
    ./build/Mosaique 1 $fich .tmp/$Var.ppm $x 2
    ffmpeg -i .tmp/$Var.ppm .tmp/$Var.jpg
    Var=$(($Var +1))
done
ffmpeg -f image2 -framerate 1 -i .tmp/%0d.jpg -loop -1 $2
cd .tmp
ls