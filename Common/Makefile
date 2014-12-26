ccx=g++
obj=Thread.o
tag=libcommon.a
lib=../Lib/

$(tag):$(obj)
	ar cqs $(tag) $(obj)
	cp -f $(tag) $(tpath)

thread.o:Thread.cpp
	$(ccx) -c Thread.cpp

clean:
	rm $(tag) $(obj)
