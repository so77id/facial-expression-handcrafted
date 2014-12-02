#!/bin/bash
# argv 1 -> nombre de la carpeta de testing ../testing/test_1/
# argv 2 -> video_list.txt
cd  build

./VisualRaysTracker $1 '3' '7' '6' '3' '../testing/VisualTrackerOut_LBP/rs3ws7/out.avi' '../testing/VisualTrackerOut_LBP/rs3ws7/XT/' '../testing/VisualTrackerOut_LBP/rs3ws7/YT/' '1' < ../coordenadas.dat &
./VisualRaysTracker $1 '3' '9' '6' '3' '../testing/VisualTrackerOut_LBP/rs3ws9/out.avi' '../testing/VisualTrackerOut_LBP/rs3ws9/XT/' '../testing/VisualTrackerOut_LBP/rs3ws9/YT/' '1' < ../coordenadas.dat &
./VisualRaysTracker $1 '3' '11' '6' '3' '../testing/VisualTrackerOut_LBP/rs3ws11/out.avi' '../testing/VisualTrackerOut_LBP/rs3ws11/XT/' '../testing/VisualTrackerOut_LBP/rs3ws11/YT/' '1' < ../coordenadas.dat &

./VisualRaysTracker $1 '5' '9' '6' '3' '../testing/VisualTrackerOut_LBP/rs5ws9/out.avi' '../testing/VisualTrackerOut_LBP/rs5ws9/XT/' '../testing/VisualTrackerOut_LBP/rs5ws9/YT/' '1' < ../coordenadas.dat &
./VisualRaysTracker $1 '5' '11' '6' '3' '../testing/VisualTrackerOut_LBP/rs5ws11/out.avi' '../testing/VisualTrackerOut_LBP/rs5ws11/XT/' '../testing/VisualTrackerOut_LBP/rs5ws11/YT/' '1' < ../coordenadas.dat &
./VisualRaysTracker $1 '5' '13' '6' '3' '../testing/VisualTrackerOut_LBP/rs5ws13/out.avi' '../testing/VisualTrackerOut_LBP/rs5ws13/XT/' '../testing/VisualTrackerOut_LBP/rs5ws13/YT/' '1' < ../coordenadas.dat &

./VisualRaysTracker $1 '7' '13' '6' '3' '../testing/VisualTrackerOut_LBP/rs7ws13/out.avi' '../testing/VisualTrackerOut_LBP/rs7ws13/XT/' '../testing/VisualTrackerOut_LBP/rs7ws13/YT/' '1' < ../coordenadas.dat &
./VisualRaysTracker $1 '7' '15' '6' '3' '../testing/VisualTrackerOut_LBP/rs7ws15/out.avi' '../testing/VisualTrackerOut_LBP/rs7ws15/XT/' '../testing/VisualTrackerOut_LBP/rs7ws15/YT/' '1' < ../coordenadas.dat &
./VisualRaysTracker $1 '7' '17' '6' '3' '../testing/VisualTrackerOut_LBP/rs7ws17/out.avi' '../testing/VisualTrackerOut_LBP/rs7ws17/XT/' '../testing/VisualTrackerOut_LBP/rs7ws17/YT/' '1' < ../coordenadas.dat &