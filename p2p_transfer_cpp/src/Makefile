CXXFLAGS =-fPIC -O2 -g -Wall -fmessage-length=0 

OBJS =exception.o common.o endpoint.o packet.o node.o server.o peer.o group_pkt.o group.o

LIBS =-lpthread

TARGET =libpt.so libpt.a

all:$(TARGET)

libpt.so:$(OBJS)
	$(CXX) -shared $^ -o $@  $(LIBS)
libpt.a:$(OBJS)
	ar -rcs $@ $^

clean:
	rm -f $(OBJS) $(TARGET)