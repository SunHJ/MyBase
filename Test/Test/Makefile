ccx=g++
obj=main.o UnitTest.o

hpath=-I../../Common
lib=-L../../Lib -lcommon

test:$(obj)
	$(ccx) $(hpath) $(lib) $(obj)
main.o:main.cpp
	$(ccx) -c main.cpp
UnitTest.o:UnitTest.cpp
	$(ccx) -c UnitTest.cpp

clean:
	rm -rf $(tag) $(obj)
