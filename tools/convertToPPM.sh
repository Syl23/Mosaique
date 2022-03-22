#! /bin/bash
for fich in `ls selectionCroped`
do
    echo $fich
    ffmpeg -i "selectionCroped/"$fich "promo/""${fich%.jpg}.ppm"
done
