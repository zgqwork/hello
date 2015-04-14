CXXFLAGS = -O2 -g -Wall -fmessage-length=0 -I../../src

LIBS = -lpthread -L../../src -lpt

TARGET = test1 test2 test3 test4 test5

all:${TARGET}

test1: test1.o
	$(CXX) $^ -o $@ $(LIBS)
test2: test2.o
	$(CXX) $^ -o $@ $(LIBS)
test3: test3.o
	$(CXX) $^ -o $@ $(LIBS)
test4: test4.o
	$(CXX) $^ -o $@ $(LIBS)
test5: test5.o
	$(CXX) $^ -o $@ $(LIBS)

clean:
	rm -f *.o ${TARGET}