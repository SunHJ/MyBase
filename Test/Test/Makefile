lib_dir=../../Lib/
bin_dir=../../Bin/
obj_dir=../../Obj/

RM=rm -rf
CC=g++
#AR=ar rc

I_COMMON=-I../../Common/ -std=c++11
L_COMMON=-lcommon -L $(lib_dir)
#创建目录
#$(shell mkdir -p $(lib_dir))
$(shell mkdir -p $(bin_dir))
$(shell mkdir -p $(obj_dir))

obj=main.o UnitTest.o
tag=test


$(tag):$(obj)
	$(CC) -o $(tag) $(obj) $(L_COMMON) -lpthread
	cp $(obj) $(obj_dir)
	cp $(tag) $(bin_dir)

main.o:main.cpp
	$(CC) -c main.cpp $(I_COMMON)

UnitTest.o:UnitTest.cpp
	$(CC) -c UnitTest.cpp $(I_COMMON)

clean:
	$(RM) $(tag) $(obj)
