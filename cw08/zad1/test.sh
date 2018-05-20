#!/bin/bash


for x in filters/* 
do
    ./run_main 1 images/mountain.ascii.pgm "$x" results/out.pgm
    ./run_main 2 images/mountain.ascii.pgm "$x" results/out.pgm
    ./run_main 4 images/mountain.ascii.pgm "$x" results/out.pgm
    ./run_main 8 images/mountain.ascii.pgm "$x" results/out.pgm
    ./run_main 16 images/mountain.ascii.pgm "$x" results/out.pgm
done    
