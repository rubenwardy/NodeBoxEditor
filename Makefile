# :mode=makefile:noTabs=false:

nbgen: main.o cEditor.o cNodes.o CommonFunctions.o
g++ -o nbgen main.o cEditor.o cNodes.o CommonFunctions.o -lIrrlicht

%.o: %.cpp
g++ -I/usr/include/irrlicht -o $@ -c $<