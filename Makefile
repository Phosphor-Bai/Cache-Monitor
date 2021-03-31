all: main.exe

main.exe: main.cpp monitor.cpp
	g++ -std=c++11 -o main.exe main.cpp monitor.cpp

clean:
	del *.exe