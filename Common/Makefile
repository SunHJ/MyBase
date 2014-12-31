lib_dir=../Lib/
bin_dir=../Bin/
obj_dir=../Obj/

RM=rm -rf
CC=g++
AR=ar rc

#创建目录
$(shell mkdir -p $(lib_dir))
$(shell mkdir -p $(bin_dir))
$(shell mkdir -p $(obj_dir))

obj=Thread.o CriticalSection.o Semaphore.o Event.o File.o \
   	Buffer.o
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

#$(shell cp $(obj) $(obj_dir))
#$(shell cp $(tag) $(lib_dir))

clean:
	rm $(tag) $(obj)
