CXX		?= g++
CXXFLAGS:= -std=c++17 -Wall -g -D_GNU_SOURCE -fPIC -fconcepts
LDFLAGS := -ldl
PROGS	:= sandbox.so
OBJDIR 	:= ./obj
SRCDIR 	:= ./

.PHONY: all
all: $(PROGS)

SRCS := $(notdir $(wildcard ${SRCDIR}/*.cpp))
OBJS := $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(SRCS)))
DEPS := $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.d, $(SRCS)))

$(shell [ -d $(OBJDIR) ] || mkdir -p $(OBJDIR))

-include $(DEPS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -MP -MMD -c $< -o $@ $(CXXFLAGS)

$(PROGS): $(OBJS)
	$(CXX) -o $@ -shared $^ $(LDFLAGS)

.PHONY: test
test: $(PROGS)
	./test.sh

.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(PROGS) ./*.log
