OBJS_C = main.cpp graphics.cpp generator.cpp effects.cpp load.cpp minimap.cpp
OBJS_H = main.h graphics.h effects.h generator.h entity.h load.h minimap.h
CC = g++

COMPILER_FLAGS = -w
# -g for debug
DEBUGGER_FLAGS = -g

LINKER_FLAGS = -lSDLmain -lSDL 

OBJ_NAME = engine

all: $(OBJS)
	$(CC) $(DEBUGGER_FLAGS) $(OBJS_C) $(OBJS_H) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

