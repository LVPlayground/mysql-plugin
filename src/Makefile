# Compile the LVP MySQL project on Linux systems. Just run "make"

$(shell [ -d "out" ] || mkdir -p out)

OUTFILE=mysql-plugin.so
COMPILE_FLAGS=-c -m32 -fPIC -O3 -std=gnu++0x -w -DLINUX -I/usr/include/mysql -I.
LINK_FLAGS=-O2 -m32 -fshort-wchar -Wl,--export-dynamic -shared -static -lpthread
CC=g++

all:
	$(CC) $(COMPILE_FLAGS) sdk/amxplugin.cpp -o out/amxplugin.o
	$(CC) $(COMPILE_FLAGS) Mutex.cpp -o out/Mutex.o
	$(CC) $(COMPILE_FLAGS) Thread.cpp -o out/Thread.o
	$(CC) $(COMPILE_FLAGS) ConnectionClient.cpp -o out/ConnectionClient.o
	$(CC) $(COMPILE_FLAGS) ConnectionController.cpp -o out/ConnectionController.o
	$(CC) $(COMPILE_FLAGS) ConnectionHost.cpp -o out/ConnectionHost.o
	$(CC) $(COMPILE_FLAGS) DebugPrinter.cpp -o out/DebugPrinter.o
	$(CC) $(COMPILE_FLAGS) plugin.cpp -o out/plugin.o
	$(CC) $(COMPILE_FLAGS) QueryBuilder.cpp -o out/QueryBuilder.o
	$(CC) $(COMPILE_FLAGS) ResultController.cpp -o out/ResultController.o
	$(CC) $(COMPILE_FLAGS) Script.cpp -o out/Script.o
	$(CC) $(COMPILE_FLAGS) StatementRegistry.cpp -o out/StatementRegistry.o
	$(CC) $(LINK_FLAGS) -o out/$(OUTFILE) out/*.o -lmysqlclient
