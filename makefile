CC=g++
SDIR=src
ODIR=build
TDIR=test

MAKEFLAGS += --no-builtin-rules

WARN_PARSER=
WARN_CODE=-Wall
CFLAGS=-g -O2 -std=gnu++98
TESTFLAGS=-g -O2 -std=gnu++11
LDFLAGS=

TESTLIB=-I/usr/local/include/UnitTest++ -lUnitTest++
TESTCMD=testrunner

EXECUTABLE?=c-
OUTPUTTAR?=BrettAM.tar

NAME?=menzies
SUBMITURL?="http://ec2-52-89-93-46.us-west-2.compute.amazonaws.com/cgi-bin/fileCapture.py"
ASSIGNMENT?=7
ASSIGNMENT_STRING:=CS445 F16 Assignment $(ASSIGNMENT)

MAIN := $(SDIR)/main.cpp
CPPS := $(shell find $(SDIR) -name "*.cpp" -not -samefile "$(MAIN)") \
			 $(ODIR)/parser.cpp $(ODIR)/lex.cpp
HPPS := $(shell find $(SDIR) -name "*.hpp")
TESTS:= $(shell find $(TDIR) -name "*.cpp")
OBJS := $(addprefix $(ODIR)/,$(notdir $(CPPS:.cpp=.o)))
TOBJS:= $(addprefix $(ODIR)/,$(notdir $(TESTS:.cpp=.o)))
INC  := -I$(SDIR) -I$(ODIR)

FLEX := $(SDIR)/parser.l
BISON := $(SDIR)/parser.y

all: init $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

init:
	mkdir -p $(ODIR)

test: init $(TOBJS) $(OBJS)
	$(CC) $(TESTFLAGS) $(INC) $(OBJS) $(TOBJS) $(TESTLIB) -o $(TESTCMD)
	./$(TESTCMD)

$(ODIR)/%.o: $(TDIR)/%.cpp $(HPPS)
	$(CC) $(TESTFLAGS) $(INC) $(TESTLIB) -c -o $@ $<

tar: test
	tar -cf $(OUTPUTTAR) ./makefile ./src

clean:
	rm -f $(OUTPUTTAR) $(EXECUTABLE) $(TESTCMD)
	rm -rf $(ODIR)

submit: tar
	curl \
		-F "student=$(NAME)"  \
		-F "assignment=$(ASSIGNMENT_STRING)" \
		-F "submittedfile=@$(OUTPUTTAR)" \
		$(SUBMITURL)

$(ODIR)/parser.hpp $(ODIR)/parser.cpp: $(BISON) $(HPPS)
	bison --debug\
		--defines=$(ODIR)/parser.hpp \
		--output=$(ODIR)/parser.cpp \
		$(BISON)

$(ODIR)/lex.cpp: $(FLEX) $(HPPS) $(ODIR)/parser.hpp
	flex --outfile=$(ODIR)/lex.cpp $(FLEX)

# cpp files from the source directory
$(ODIR)/%.o: $(SDIR)/%.cpp $(SDIR)/%.hpp $(ODIR)/parser.hpp
	$(CC) $(WARN_CODE) $(INC) $(CFLAGS) -c -o $@ $<

# cpp files from the build directory (generated by flex/bison)
$(ODIR)/%.o: $(ODIR)/%.cpp
	$(CC) $(WARN_PARSER) $(INC) $(CFLAGS) -c -o $@ $<

$(EXECUTABLE): $(MAIN) $(HPPS) $(OBJS)
	$(CC)  $(WARN_CODE) $(INC) $(CFLAGS) $(LDFLAGS) $(MAIN) $(OBJS) -o $@

