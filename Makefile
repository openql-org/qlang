CC      = g++
CFLAGS  = -g -O0 -MMD -MP `llvm-config --cxxflags`
LDFLAGS = `llvm-config --ldflags --libs --system-libs`
LIBS    = 
INCLUDE = -I ./include
SRC_DIR = ./src
OBJ_DIR = ./build
SOURCES = $(shell ls $(SRC_DIR)/*.cpp) 
OBJS    = $(subst $(SRC_DIR),$(OBJ_DIR), $(SOURCES:.cpp=.o))
TARGET  = qlang
DEPENDS = $(OBJS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJS) $(LIBS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)
	clang -emit-llvm -S -O -o $(OBJ_DIR)/write.ll $(SRC_DIR)/write.c

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp 
	@if [ ! -d $(OBJ_DIR) ]; \
		then echo "mkdir -p $(OBJ_DIR)"; mkdir -p $(OBJ_DIR); \
	fi
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $< 

clean:
	$(RM) $(OBJS) $(TARGET) $(DEPENDS)

run:
	./qlang example/test.q 
	# optimize frontend IR with opt 
	opt -O3 -S -f out.ll -o out_o.ll
	llvm-link out.ll ./build/write.ll -S -o ./build/linked.ll
	opt -S -mem2reg ./build/linked.ll > exe.ll
	lli exe.ll

-include $(DEPENDS)

.PHONY: all cleanls
