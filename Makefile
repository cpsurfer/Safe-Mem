CXX = g++

# 1. Default Build Mode: Isse aap command line se change kar sakte hain
# Use: 'make MODE=release' or 'make MODE=debug'
MODE ?= release

# 2. Base Flags (Hamesha use hone wale)
CXXFLAGS = -Iinclude -Wall -Wextra -std=c++17 -msse4.2

# 3. Mode-specific Flags
ifeq ($(MODE), release)
    # Release Mode: Full Optimization + Debug checks off
    # -O3: Maximum speed
    # -DNDEBUG: Google Benchmark ki warning hatane ke liye
    # -march=native: Aapke current CPU ke liye best instructions use karega
    CXXFLAGS += -O3 -DNDEBUG -march=native
else
    # Debug Mode: Symbol information + No optimization
    # -g: GDB debugging ke liye symbols add karta hai
    # -O0: Optimization band (debugging easy hoti hai)
    CXXFLAGS += -g -O0
endif

LDFLAGS = -lbenchmark -lpthread

# The final executables
all: test_driver benchmark simdtest

# Linker block: Objects se executable banana
simdtest: src/safemem.o test/simdtest.o
	$(CXX) $(CXXFLAGS) $^ -o simdtest

benchmark: src/safemem.o test/benchmark.o
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o benchmark

test_driver: src/safemem.o test/test_driver.o
	$(CXX) $(CXXFLAGS) $^ -o test_driver

# Compilation block: .cpp se .o banana
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Safely clean objects and binaries
clean:
	rm -f src/*.o test/*.o test_driver benchmark simdtest
