#! /bin/bash 
for fich in `ls colorDatabase`
do
    echo $fich
    ffmpeg -i "colorDatabase/"$fich "ppmColorDatabase/""${fich%.jpg}.ppm"
done
