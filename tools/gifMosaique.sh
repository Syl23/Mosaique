#!/bin/bash
fich=$1
Var=1
mkdir .tmp
for x in 1 2 3 4 5 6 7 8 9
do
    echo $Var
    ./build/Mosaique 0 $fich .tmp/$x.ppm $Var 2
    ffmpeg -i .tmp/$x.ppm .tmp/$x.jpg
    Var=$(($Var * 2))
done
ffmpeg -f image2 -framerate 1 -i .tmp/%0d.jpg -loop -1 $2
cd .tmp
ls