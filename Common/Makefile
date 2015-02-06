lib_dir=../Lib/
bin_dir=../Bin/
obj_dir=../Obj/

RM=rm -rf
CC=g++ -g
AR=ar rc

#创建目录
$(shell mkdir -p $(lib_dir))
$(shell mkdir -p $(bin_dir))
$(shell mkdir -p $(obj_dir))

obj=Thread.o CriticalSection.o Semaphore.o Event.o File.o \
   	Buffer.o SocketWrapper.o SocketStream.o SocketEvent.o \
	SocketAcceptor.o SelecterServer.o SocketStreamQueue.o \
	SuperServer.o SocketStreamBuffer.o SocketStreamEpoll.o
tag=libcommon.a


$(tag):$(obj)
	$(AR) $(tag) $(obj)
	cp $(obj) $(obj_dir)
	cp $(tag) $(lib_dir)

Thread.o:Thread.cpp
	$(CC) -c Thread.cpp

CriticalSection.o:CriticalSection.cpp
	$(CC) -c CriticalSection.cpp

Event.o:Event.cpp
	$(CC) -c Event.cpp

Semaphore.o:Semaphore.cpp
	$(CC) -c Semaphore.cpp

File.o:File.cpp
	$(CC) -c File.cpp

Buffer.o:Buffer.cpp
	$(CC) -c Buffer.cpp -std=c++11

SocketWrapper.o:SocketWrapper.cpp
	$(CC) -c SocketWrapper.cpp

SocketStream.o:SocketStream.cpp
	$(CC) -c SocketStream.cpp -std=c++11

SocketStreamEpoll.o:SocketStreamEpoll.cpp
	$(CC) -c SocketStreamEpoll.cpp -std=c++11

SocketEvent.o:SocketEvent.cpp
	$(CC) -c SocketEvent.cpp -std=c++11

SocketStreamQueue.o:SocketStreamQueue.cpp
	$(CC) -c SocketStreamQueue.cpp -std=c++11

SocketStreamBuffer.o:SocketStreamBuffer.cpp
	$(CC) -c SocketStreamBuffer.cpp -std=c++11

SocketAcceptor.o:SocketAcceptor.cpp
	$(CC) -c SocketAcceptor.cpp -std=c++11

SelecterServer.o:SelecterServer.cpp
	$(CC) -c SelecterServer.cpp -std=c++11

SuperServer.o:SuperServer.cpp
	$(CC) -c SuperServer.cpp -std=c++11

#$(shell cp $(obj) $(obj_dir))
#$(shell cp $(tag) $(lib_dir))

clean:
	$(RM) $(tag) $(obj)
