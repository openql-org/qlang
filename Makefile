CC      = g++
CFLAGS  = -g -O0 -MMD -MP `llvm-config --cxxflags` -fexceptions
LDFLAGS = `llvm-config --ldflags --libs --system-libs`
LIBS    = 
INCLUDE = -I ./include
SRC_DIR = ./src
OBJ_DIR = ./build
OPTIMZ  = ./optimizer
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
	# optimize frontend IR with opt ls
	opt -O3 -S -f out.ll -o out_o.ll
	llvm-link out.ll ./build/write.ll -S -o ./build/linked.ll
	opt -S -mem2reg ./build/linked.ll > exe.ll
	lli exe.ll

run-riscv:
	./qlang example/test.q 
	# optimize frontend IR with opt ls
	opt -O3 -S -f out.ll -o out_o.ll
	llvm-link out.ll ./build/write.ll -S -o ./build/linked.ll
	opt -S -mem2reg ./build/linked.ll > exe.ll
	# on risc-v target 
	llc -march=riscv64 -relocation-model=pic -filetype=asm exe.ll -o exe.s
	riscv64-unknown-elf-gcc exe.s -o exe -march=rv64imafdkc

# llvm clang quantum optimizer for c/cpp language pass.
# clang -S -Xclang -load -Xclang libqot.so test/test_q.c
qopt:
	g++ -shared -fPIC -o libqot.so $(OPTIMZ)/quantumOptimizerPass.cpp `llvm-config --cxxflags`




-include $(DEPENDS)

.PHONY: all cleanls
