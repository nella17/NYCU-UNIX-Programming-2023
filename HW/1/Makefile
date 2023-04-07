CXX		= g++
CXXFLAGS= -Wall -g -D_GNU_SOURCE -fPIC
LDFLAGS = 
PROGS	= sandbox.so

.PHONY: all
all: $(PROGS)

%.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS)

sandbox.so: sandbox.o
	$(CXX) -o $@ -shared $^ $(LDFLAGS)

.PHONY: test
test: $(PROGS)
	./test.sh

.PHONY: clean
clean:
	rm -f *.o *.so
