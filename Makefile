CC      = g++
CFLAGS  = -g -O0 -MMD -MP `llvm-config --cxxflags` -fexceptions
LDFLAGS = `llvm-config --ldflags --libs --system-libs`
LIBS    = 
INCLUDE = -I ./include
SRC_DIR = ./src
OBJ_DIR = ./build
OPT_DIR = ./optimizer
SOURCES = $(shell ls $(SRC_DIR)/*.cpp) 
OBJS    = $(subst $(SRC_DIR),$(OBJ_DIR), $(SOURCES:.cpp=.o))
TARGET  = qlang
DEPENDS = $(OBJS:.o=.d)
LLFILES = *.ll
OPTFILE = libqot.so

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
	$(RM) $(OBJS) $(OBJ_DIR)/* $(TARGET) $(DEPENDS) $(LLFILES) $(OPTFILE)

run:
# exp) spike -r2 -q2 pk ./exe
	# optimize frontend IR with opt ls
	./qlang -O3 example/test.q 
	llvm-link out.ll ./build/write.ll -S -o ./build/linked.ll
	opt -S -mem2reg ./build/linked.ll > exe.ll
	lli exe.ll

run-riscv:
	# optimize frontend IR with opt ls
	./qlang -O3 example/test.q 
	llvm-link out.ll ./build/write.ll -S -o ./build/linked.ll
	opt -S -mem2reg ./build/linked.ll > exe.ll
	# on risc-v target 
	llc -march=riscv64 -relocation-model=pic -filetype=asm exe.ll -o exe.s
	riscv64-unknown-elf-gcc exe.s -o exe -march=rv64imafdkc

qopt:
# llvm clang quantum optimizer for c/cpp language pass example.
# exp) clang -S -Xclang -load -Xclang libqot.so test/test_q.c
	g++ -shared -fPIC -o $(OPTFILE) $(OPT_DIR)/quantumOptimizerPass.cpp `llvm-config --cxxflags`

-include $(DEPENDS)

.PHONY: all cleanls
