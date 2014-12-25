ccx=g++
obj=thread.o
tag=libcommon.a

$(tag):$(obj)
	ar cqs $(tag) $(obj)

thread.o:Thread.cpp
	$(ccx) -c Thread.cpp

clean:
	rm $(tag) $(obj)
