CXXFLAGS:= -std=c++17 -Wall -g -fsanitize=address -fsanitize=undefined
LDFLAGS := -lcapstone -fsanitize=address -fsanitize=undefined
PROG	:= sdb
OBJDIR 	:= ./obj
SRCDIR 	:= ./

.PHONY: all
all: $(PROG)

SRCS := $(notdir $(wildcard ${SRCDIR}/*.cpp))
OBJS := $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(SRCS)))
DEPS := $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.d, $(SRCS)))

$(shell [ -d $(OBJDIR) ] || mkdir -p $(OBJDIR))

-include $(DEPS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -MP -MMD -c $< -o $@ $(CXXFLAGS)

$(PROG): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

.PHONY: test
test: $(PROG)
	./test.sh

.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(PROG)

