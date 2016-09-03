CC=g++
SDIR=src
ODIR=build
TDIR=test

MAKEFLAGS += --no-builtin-rules

CFLAGS=-Wall -g -O2 -std=gnu++11
LDFLAGS=

TESTLIB=-I/usr/local/include/UnitTest++ -lUnitTest++
TESTCMD=testrunner

EXECUTABLE?=compiler
OUTPUTTAR?=BrettAM.tar

NAME?=menzies
SUBMITURL?="http://ec2-54-200-16-181.us-west-2.compute.amazonaws.com/cgi-bin/fileCapture.py"
ASSIGNMENT?=0
ASSIGNMENT_STRING::=CS451 S14 Assignment $(ASSIGNMENT)

MAIN ::= $(SDIR)/main.cpp
CPPS ::= $(shell find $(SDIR) -name "*.cpp" -not -samefile "$(MAIN)") \
			 $(ODIR)/parser.cpp $(ODIR)/lex.cpp
HPPS ::= $(shell find $(SDIR) -name "*.hpp")
TESTS::= $(shell find $(TDIR) -name "*.cpp")
OBJS ::= $(addprefix $(ODIR)/,$(CPPS:.cpp=.o))
INC  ::= -I$(SDIR) -I$(ODIR)

FLEX ::= $(SDIR)/parser.l
BISON ::= $(SDIR)/parser.y

all: init $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

init:
	mkdir -p $(ODIR)/$(SDIR) $(ODIR)/$(ODIR)

$(ODIR)/parser.%: $(BISON) $(HPPS)
	bison \
		--defines=$(ODIR)/parser.hpp \
		--output=$(ODIR)/parser.cpp \
		$(BISON)

$(ODIR)/lex.cpp: $(FLEX) $(HPPS) $(ODIR)/parser.hpp
	flex --outfile=$(ODIR)/lex.cpp $(FLEX)

$(ODIR)/%.o: %.cpp
	$(CC) -c $(INC) -o $@ $< $(CFLAGS)

$(EXECUTABLE): $(MAIN) $(HPPS) $(OBJS)
	$(CC) $(INC) $(LDFLAGS) $(CFLAGS) $(MAIN) $(OBJS) -o $@

test: $(OBJS) $(TESTS)
	$(CC) $(CFLAGS) $(TESTS) $(INC) $(TESTLIB) $(OBJS) -o $(TESTCMD)
	./$(TESTCMD)

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
