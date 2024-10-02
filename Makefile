OBJS_C = main.cpp graphics.cpp renderer.cpp utility.cpp load.cpp
OBJS_H = main.h graphics.h entity.h renderer.h utility.h load.h
CC = g++

COMPILER_FLAGS = -w
# -g for debug
DEBUGGER_FLAGS = -g

LINKER_FLAGS = -lSDLmain -lSDL 

OBJ_NAME = engine

all: $(OBJS)
	$(CC) $(DEBUGGER_FLAGS) $(OBJS_C) $(OBJS_H) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

