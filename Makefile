# List the names of your C++ source files here (without the .cpp extension)
SRCS := shell parser alias analyze_tokens history wild_characters

# Specify the g++ compiler
CXX := g++

# Compile flags (none in this case)
CXXFLAGS :=

# List the names of your header files (with .hpp extension)
HEADERS := alias.hpp analyze_proto.hpp history_proto.hpp parser_proto.hpp wild_proto.hpp global.hpp

# List the names of your object files (generated from your source files)
OBJS := $(SRCS:=.o)

# Name of your executable (change this to whatever you want)
EXEC := mysh

# Default target
all: $(EXEC)

# Rule to link the object files into an executable
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(EXEC)

# Rule to compile each source file into an object file
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Remove generated files
clean:
	rm -f $(OBJS) $(EXEC)

