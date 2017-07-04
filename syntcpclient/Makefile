ifeq	($(release),1)

FLG := -O3 -fPIC

else

FLG := -O3 -g -fPIC

endif

PRG := bin/libsynTcpCli.so

SRC := $(wildcard src/*.cpp) $(wildcard oracle/*.cpp)

OBJ := $(patsubst %.cpp,%.o,$(SRC))

CC := g++

INC := -I./inc         \
       -I./interface         \
       -I../../../lib/cross/boost/include/ \
	   -I../../../lib/cross/zlib1/inc/  \
	   -I../../../lib_protcomm/sources/protcomm/include/ \
       -I../../../lib_libSelf/cross/utils/inc/
	   
LIB := -L../../../lib/cross/zlib1/bin/so/ -lz
	   
.PHONY: clean

$(PRG): $(OBJ)
	$(CC) -shared -o $@ $^ $(LIB)

%.o: %.cpp
	$(CC) $(FLG) -o $@ -c $< $(INC)

clean:
	rm -f $(OBJ) $(PRG)
