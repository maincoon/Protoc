# dest object files
OBJS=main/main.o\
src/common/TLog.o\
src/common/TXMLParser.o\
src/protoc/ProtocData.o\
src/protoc/ProtocFormatter.o\
src/protoc/ProtocSchema.o\
src/protoc/Utils.o\
src/tinyxml/tinyprint.o\
src/tinyxml/tinystr.o\
src/tinyxml/tinyxml.o\
src/tinyxml/tinyxmlerror.o\
src/tinyxml/tinyxmlparser.o

# options
DEFS=-DTARGET_UNIX
INCLUDE=-Isdk
CFLAGS=-O3 -g0 -Wall $(DEFS) $(INCLUDE) $(ADDFLAGS)
LFLAGS=-g3 
PROJ=bin/Protoc

# compiler
CC=gcc
CPPC=g++
LD=g++

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<

.cpp.o:
	$(CPPC) -c $(CFLAGS) -o $@ $<

all: $(PROJ)

$(PROJ): $(OBJS)
	$(LD) -o $(PROJ) $(OBJS) $(LFLAGS)

clean:
	rm -f $(OBJS) $(PROJ)
