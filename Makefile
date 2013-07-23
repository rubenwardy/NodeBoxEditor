# :mode=makefile:noTabs=false:

nbgen: main.o Editor.o Node.o NBPropertiesDialog.o CodeDialog.cpp CommonFunctions.o
g++ -o nbgen main.o Editor.o Node.o NBPropertiesDialog.o CodeDialog.cpp CommonFunctions.o -lIrrlicht

%.o: %.cpp
g++ -I/usr/include/irrlicht -o $@ -c $<