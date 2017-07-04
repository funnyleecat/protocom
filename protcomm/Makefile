ifeq	($(release),1)

FLG := -O3 -fPIC

else

FLG := -O0 -g -fPIC

endif

SRC := $(wildcard src/*.cpp)

OBJ := $(patsubst %.cpp,%.o,$(SRC))

CC := g++

SHARED_BIN := bin/libprotcomm.so
SHARED_LINKER := g++ -shared -o 

STATIC_BIN :=  bin/libprotocol.a
STATIC_LINKER :=  ar cr

PRG := $(SHARED_BIN) 
LINKER := $(SHARED_LINKER)

INC :=  -I./inc -I./include -I./protcomm \
		-I../../../lib_libSelf/cross/utils/inc \
		-I../../../lib/cross/boost/include \
		-I../../../lib_tcpcomm2/sources/tcpcomm2/include \
		-I../../../lib/cross/zlib1/inc

LIB :=	-L../../../lib_tcpcomm2/sources/tcpcomm2/bin -ltcpcomm2 \
		-L../../../lib/cross/zlib1/bin/a -lz

.PHONY: clean

$(PRG): $(OBJ)
	$(LINKER) $@ $^ $(LIB)

%.o: %.cpp
	$(CC) $(FLG) -o $@ -c $< $(INC)

clean:
	rm -f $(OBJ) $(PRG)
