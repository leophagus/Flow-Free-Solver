MINISATDIR = ../minisat+/

# all minisat objs except Main.o. TODO make shared lib
MINISATOBJS = $(MINISATDIR)/ADTs/Global.o $(MINISATDIR)/ADTs/FEnv.o $(MINISATDIR)/ADTs/File.o $(MINISATDIR)/Pb*.o $(MINISATDIR)/Sat*.o $(MINISATDIR)/Hard*.o $(MINISATDIR)/Mini*.o $(MINISATDIR)/Debug.o

CXX = g++
CXXFLAGS = -ggdb -D DEBUG -ffloat-store -std=c++11 -D_FILE_OFFSET_BITS=64 -I$(MINISATDIR) -I$(MINISATDIR)/ADTs/
LDFLAGS = -lz -lgmp

SRCS = main.cpp satform.cpp problem.cpp board.cpp
INCS = board.h problem.h satform.h
OBJS := $(SRCS:.cpp=.o)


EXEC = router

.PHONY : all clean

all: $(EXEC)

clean:
	rm -f $(OBJS) $(EXEC)

$(EXEC): $(OBJS) $(MINISATOBJS)
	$(CXX) $(CFLAGS) $^ $(LDFLAGS) -o $@


