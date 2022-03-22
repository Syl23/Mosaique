#!/bin/bash
for fich in `ls promo`
do
    Var=1
    for x in 1 2 3 4 5 6 7
    do
        echo $Var
        ./exe promo/$fich results/promo/${fich%.ppm}$x.ppm $Var
        ffmpeg -i results/promo/${fich%.ppm}$x.ppm results/promo/${fich%.ppm}$x.jpg
        Var=$(($Var * 2))
    done
    ffmpeg -f image2 -framerate 1 -i results/promo/${fich%.ppm}%0d.jpg -loop -1 results/gif/${fich%.ppm}.gif
done
