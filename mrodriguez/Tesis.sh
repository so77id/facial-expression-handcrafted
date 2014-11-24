#!/bin/bash
# argv 1 -> nombre de la carpeta de testing ../testing/test_1/
# argv 2 -> video_list.txt
echo "Entrando a la carpeta build"
cd build/
rm -R $1
mkdir $1

kFoldPath=$1'k-fold/'
MicroNNPath=$1'microdescriptors_nn/'
MicroNPath=$1'microdescriptors_n/'
MacroPath=$1'macrodescriptors/'
SVMResults=$1'svmResults/'
videoList=$2

mkdir $kFoldPath
mkdir $MicroNNPath
mkdir $MicroNPath
mkdir $MacroPath
mkdir $SVMResults

echo "Creando el kfold"
./kFoldCreatorFiles $videoList $kFoldPath 5 50 1

echo "Ejecutando la extraccion de microdescriptores"
./TesisMain -RS 1 1 $videoList $MicroNNPath 3 7

echo "Ejecutando la normalizacion de microdescriptores"
./TesisMain -N $MicroNNPath'Config_microdescriptors_no_norm.txt' $MicroNPath 60 60 10

echo "Ejecutando la creacion de macrodescriptores"
./TesisMain -K $MicroNPath'Config_microdescriptors_norm.txt' $MacroPath $kFoldPath 3 3 2

echo "Ejecutando el SVM"
./TesisMain -SVM $MacroPath'Config_macrodescriptors.txt' $SVMResults $videoList $kFoldPath