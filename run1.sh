#!/bin/bash

cc sequentialWithRectangularPoints.c -o a -lpthread -lm
./a
cc parallelWithRectangularPoints_#2.c -o a -lpthread -lm
./a
cc parallelWithRectangularPoints_#3.c -o a -lpthread -lm
./a
cc parallelWithRectangularPoints_#5.c -o a -lpthread -lm
./a
cc parallelWithRectangularPoints_#8.c -o a -lpthread -lm
./a
cc parallelWithRectangularPoints_#10.c -o a -lpthread -lm
./a
cc parallelWithRectangularPoints_#15.c -o a -lpthread -lm
./a
cc parallelWithRectangularPoints_#25.c -o a -lpthread -lm
./a
