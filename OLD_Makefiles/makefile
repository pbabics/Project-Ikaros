CC=@g++
Flags=-W -Wall -O2 -ldl -rdynamic -pthread -g
BaseDir=./
BuildDir=./build/
Additions= -ldl -rdynamic $(BaseDir)Shared/Shared.h
OutputFileName= server
Output=-o $(BaseDir)$(OutputFileName)


SourceRootDir= /home/ntx/Plocha/MultiServer
ApplicationInclude = $(SourceRootDir)/Application
DataStructInclude = $(SourceRootDir)/DataArrays
HandlingInclude = $(SourceRootDir)/Handling
SignalInclude = $(SourceRootDir)/Signal
SharedInclude = $(SourceRootDir)/Shared
SimpleLogInclude = $(SourceRootDir)/SimpleLog
SocketsInclude = $(SourceRootDir)/Sockets
ThreadsInclude = $(SourceRootDir)/Threads
ConfigMgrInclude = $(SourceRootDir)/ConfigMgr

IncludeDirs= -I $(SourceRootDir) -I $(ApplicationInclude) -I $(DataStructInclude) -I $(HandlingInclude) -I $(SignalInclude) -I $(SharedInclude) -I $(SimpleLogInclude) -I $(SocketsInclude) -I $(ThreadsInclude) -I $(ConfigMgrInclude)
 
Flags += $(IncludeDirs)

Objs= build/Utils.o build/binData.o build/file.o build/SimpleLog.o -ldl build/Shared.o build/SignalHandler.o\
 build/Socket.o build/SocketMgr.o build/ThreadMgr.o build/ConfigMgr.o build/Handler.o build/Application.o build/main.o

ObjectOut=-o $@


black= tput setaf 0
red= tput setaf 1
green= tput setaf 2
orange= tput setaf 3
blue= tput setaf 4
bold= tput bold
normal= tput sgr0

all: $(Objs)
	@cd $(BuildDir);make;

install: all HTTP SMTP FTP
	@cp ./server ./Server/

HTTP:
	@cd ./Protocol/HTTP/;make;
	@cp ./Protocol/HTTP/*.so ./Server/

SMTP:
	@cd ./Protocol/SMTP/;make;
	@cp ./Protocol/SMTP/*.so ./Server/

FTP:
	@cd ./Protocol/FTP/;make;
	@cp ./Protocol/FTP/*.so ./Server/

clean:
	@$(red);$(bold);echo "[ 1 / 2 ] Cleaning Root Directory";$(normal)
	@rm -rf ./*.o
	@rm -rf ./$(OutputFileName)
	@rm -rf ./*.out
	@cd $(BuildDir);make clean;

$(Objs): $(BaseDir)Includes.h $(BaseDir)Defs.h

build/Utils.o: Utils.cpp  Utils.h
	@$(blue);$(bold);echo "Building Utilities";$(normal)
	$(CC) $(Flags) $(ObjectOut) -c Utils.cpp

build/binData.o: DataArrays/binData.cpp DataArrays/binData.h
	@$(blue);$(bold);echo "Building Binary Data Class";$(normal)
	$(CC) $(Flags) $(ObjectOut) -c DataArrays/binData.cpp

build/file.o: $(BuildDir)binData.o DataArrays/file.cpp DataArrays/file.h
	@$(blue);$(bold);echo "Building File Class";$(normal)
	$(CC) $(Flags) $(ObjectOut) -c DataArrays/file.cpp

build/SimpleLog.o: SimpleLog/SimpleLog.cpp SimpleLog/SimpleLog.h
	@$(blue);$(bold);echo "Building Loging";$(normal)
	$(CC) $(Flags) $(ObjectOut) -pthread -c SimpleLog/SimpleLog.cpp

build/Shared.o: Shared/Shared.cpp Shared/Shared.h
	@$(blue);$(bold);echo "Building Shared";$(normal)
	$(CC) $(Flags) $(ObjectOut) -ldl -rdynamic -c Shared/Shared.cpp

build/SignalHandler.o: Signal/SignalHandler.cpp Signal/SignalHandler.h
	@$(blue);$(bold);echo "Building Signal Handler";$(normal)
	$(CC) $(Flags) $(ObjectOut) -c Signal/SignalHandler.cpp

build/Socket.o: Sockets/Socket.cpp Sockets/Socket.h
	@$(blue);$(bold);echo "Building Sockets";$(normal)
	$(CC) $(Flags) $(ObjectOut) -c Sockets/Socket.cpp

build/SocketMgr.o: $(BuildDir)Socket.o Sockets/SocketMgr.cpp Sockets/SocketMgr.h
	@$(blue);$(bold);echo "Building Socket Manager";$(normal)
	$(CC) $(Flags) $(ObjectOut) -c Sockets/SocketMgr.cpp

build/ThreadMgr.o: Threads/ThreadMgr.cpp Threads/ThreadMgr.h
	@$(blue);$(bold);echo "Building Thread Manager";$(normal)
	$(CC) $(Flags) $(ObjectOut) -pthread -c Threads/ThreadMgr.cpp

build/Handler.o: Handling/Handler.cpp Handling/Handler.h
	@$(blue);$(bold);echo "Building Data Handler";$(normal)
	$(CC) $(Flags) $(ObjectOut) -c Handling/Handler.cpp

build/Application.o: Application/Application.cpp  Application/Application.h
	@$(blue);$(bold);echo "Building Application";$(normal)
	$(CC) $(Flags) $(ObjectOut) -c Application/Application.cpp

build/ConfigMgr.o: ConfigMgr/ConfigMgr.cpp  ConfigMgr/ConfigMgr.h
	@$(blue);$(bold);echo "Building Config Manager";$(normal)
	$(CC) $(Flags) $(ObjectOut) -c ConfigMgr/ConfigMgr.cpp

build/main.o: main.cpp
	@$(blue);$(bold);echo "Building Main Function";$(normal)
	$(CC) $(Flags) $(ObjectOut) -c main.cpp
