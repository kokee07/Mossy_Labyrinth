CC=g++
#SRC=src/enemigo.cpp src/maths_funcs.cpp src/gl_utils.cpp src/main.cpp
SRC=src/*.cpp
LIBS=-lGL -lGLEW -lglfw -lassimp -lLinearMath -lBulletDynamics -lBulletCollision
INCLUDE=-I/usr/include/bullet
EXEC=bin/prog

all: 
	${CC} ${SRC} ${INCLUDE} ${LIBS} -o ${EXEC} 


clear:
	rm bin/*
