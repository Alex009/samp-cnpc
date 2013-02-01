OUTFILE = "Release/CNPC.so"
 
COMPILE_FLAGS=-m32 -c -O3 -w -DLINUX -I./plugin/SDK/amx/

all:
	gcc $(COMPILE_FLAGS) plugin/SDK/amx/*.c
	g++ $(COMPILE_FLAGS) plugin/SDK/*.cpp
	g++ $(COMPILE_FLAGS) plugin/utils/*.cpp
	g++ $(COMPILE_FLAGS) plugin/*.cpp
	g++ -m32 -O -pthread -unistd -fshort-wchar -shared -o $(OUTFILE) *.o
	rm *.o
