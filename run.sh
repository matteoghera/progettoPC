#!/bin/bash

cc sequentialWithCirclePoints.c -o a -lpthread -lm
./a.out
cc parallelWithCirclePoints_#2.c -o a -lpthread -lm
./a.out
cc parallelWithCirclePoints_#3.c -o a -lpthread -lm
./a.out
cc parallelWithCirclePoints_#5.c -o a -lpthread -lm
./a.out
cc parallelWithCirclePoints_#10.c -o a -lpthread -lm
./a.out
cc parallelWithCirclePoints_#15.c -o a -lpthread -lm
./a.out
cc sequentialWith2CirclePoints.c -o a -lpthread -lm
./a.out
cc parallelWith2CirclePoints_#2.c -o a -lpthread -lm
./a.out
cc parallelWith2CirclePoints_#3.c -o a -lpthread -lm
./a.out
cc parallelWith2CirclePoints_#5.c -o a -lpthread -lm
./a.out
cc parallelWith2CirclePoints_#10.c -o a -lpthread -lm
./a.out
cc parallelWith2CirclePoints_#15.c -o a -lpthread -lm
./a.out
cc sequentialWithRectangularPoints.c -o a -lpthread -lm
./a.out
cc parallelWithRectangularPoints_#2.c -o a -lpthread -lm
./a.out
cc parallelWithRectangularPoints_#3.c -o a -lpthread -lm
./a.out
cc parallelWithRectangularPoints_#5.c -o a -lpthread -lm
./a.out
cc parallelWithRectangularPoints_#10.c -o a -lpthread -lm
./a.out
cc parallelWithRectangularPoints_#15.c -o a -lpthread -lm
./a.out
