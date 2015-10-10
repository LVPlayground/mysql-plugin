# Compile the LVP MySQL project on Linux systems. Just run "make"

$(shell [ -d "out" ] || mkdir -p out)

OUTFILE="lvp_MySQL.so"
COMPILE_FLAGS=-c -m32 -fPIC -O3 -std=gnu++0x -w -DLINUX -I. -Ilibmysql/include
LINK_FLAGS=-O2 -m32 -fshort-wchar -shared -static -lpthread

all:
	g++ $(COMPILE_FLAGS) sdk/amxplugin.cpp -o out/amxplugin.o
	g++ $(COMPILE_FLAGS) Mutex.cpp -o out/Mutex.o
	g++ $(COMPILE_FLAGS) Thread.cpp -o out/Thread.o
	g++ $(COMPILE_FLAGS) ConnectionClient.cpp -o out/ConnectionClient.o
	g++ $(COMPILE_FLAGS) ConnectionController.cpp -o out/ConnectionController.o
	g++ $(COMPILE_FLAGS) ConnectionHost.cpp -o out/ConnectionHost.o
	g++ $(COMPILE_FLAGS) DebugPrinter.cpp -o out/DebugPrinter.o
	g++ $(COMPILE_FLAGS) plugin.cpp -o out/plugin.o
	g++ $(COMPILE_FLAGS) QueryBuilder.cpp -o out/QueryBuilder.o
	g++ $(COMPILE_FLAGS) ResultController.cpp -o out/ResultController.o
	g++ $(COMPILE_FLAGS) Script.cpp -o out/Script.o
	g++ $(COMPILE_FLAGS) StatementRegistry.cpp -o out/StatementRegistry.o
	g++ $(LINK_FLAGS) -o $(OUTFILE) out/*.o libmysql/libmysqlclient.a
	strip $(OUTFILE)
