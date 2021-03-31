all: main.exe

main.exe: main.cpp monitor.cpp method.cpp tools.cpp
	g++ -std=c++11 -o main.exe main.cpp monitor.cpp method.cpp tools.cpp

clean:
	del *.exe