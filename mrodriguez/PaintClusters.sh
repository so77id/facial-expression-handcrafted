#!/bin/bash
# argv 1 -> archivo microdescritpores
# argv 2 -> k-fold
# argv 3 -> path salida de imagenes
# argv 4 -> path de salida de vocabularios
# argv 5 -> cluster para clustering
# argv 6 -> cantidad de clusters a pintar $6 <= $5 hacer if
# argv 7 -> video_list.txt
cd  build


rm -R $3'class1'
rm -R $3'class2'
rm -R $3'class3'
rm -R $3'class4'
rm -R $3'class5'
rm -R $3'class6'

mkdir $3'class1'
mkdir $3'class2'
mkdir $3'class3'
mkdir $3'class4'
mkdir $3'class5'
mkdir $3'class6'

estado=0
zero=0
while read p; do
    array=()
    i=0
    for word in $p
        do
            array[$i]=$word
            i=$(($i + 1))
    done

    path=${array[0]}
    id=${array[1]}
    class=${array[2]}

    echo 'Ejecutando creacion de imagen para video: '$id
    ./PaintFaceWithClusters $path $id $1 $5 $2 $3'class'$class'/'$id'.png' $estado $4'vocabulario_'$5'.yml' $6

    if [ $estado -eq 0 ]
     then
      estado=1
    fi

done <$7

#clase 1
    #
#clase 2
    #./PaintFaceWithClusters '../data/videos/rigid_test5.avi' '5' $1 $5 $2 $3'Img_clase_2_id_5_k_'$5'_paint_'$6'.png' 1 $4'vocabulario_'$5'.yml' $6
#clase 3
    #./PaintFaceWithClusters '../data/videos/rigid_test1.avi' '1' $1 $5 $2 $3'Img_clase_3_id_1_k_'$5'_paint_'$6'.png' 1 $4'vocabulario_'$5'.yml' $6
#clase 4
   # ./PaintFaceWithClusters '../data/videos/rigid_test6.avi' '6' $1 $5 $2 $3'Img_clase_2_id_5_k_'$5'_paint_'$6'.png' 1 $4'vocabulario_'$5'.yml' $6
#clase 5

#clase 6
