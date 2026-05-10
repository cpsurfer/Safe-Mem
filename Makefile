CXX = g++

# --- SYSTEM INSTALLATION PREFIX ---
PREFIX ?= /usr/local

# 1. Default Build Mode
# Use: 'make MODE=release' or 'make MODE=debug'
MODE ?= release

# 2. Base Flags (Hamesha use hone wale)
CXXFLAGS = -Iinclude -Wall -Wextra -std=c++17 -msse4.2

# 3. Mode-specific Flags
ifeq ($(MODE), release)
	# Release Mode: Full Optimization + Debug checks off
	CXXFLAGS += -O3 -DNDEBUG -march=native
else
	# Debug Mode: Symbol information + No optimization
	CXXFLAGS += -g -O0
endif

LDFLAGS = -L/usr/local/lib -lbenchmark -lpthread

# The final targets (Added libfmem.a here)
all: libfmem.a test_driver benchmark simdtest

# --- LIBRARY GENERATION BLOCK ---
# Static library create karne ka rule
libfmem.a: src/safemem.o
	ar rcs $@ $^
	@echo "Static Library $@ generated successfully."

# --- EXECUTABLE BLOCK ---
simdtest: src/safemem.o test/simdtest.o
	$(CXX) $(CXXFLAGS) $^ -o simdtest

benchmark: src/safemem.o test/benchmark.o
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o benchmark

test_driver: src/safemem.o test/test_driver.o
	$(CXX) $(CXXFLAGS) $^ -o test_driver

# Compilation block: .cpp se .o banana
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- CLEAN TARGET ---
# Safely clean objects, binaries, AND the static library
clean:
	rm -f src/*.o test/*.o test_driver benchmark simdtest libfmem.a

# --- INSTALL TARGET ---
.PHONY: install uninstall
install: libfmem.a
	@echo "Installing Safe-Mem globally..."
	install -d $(PREFIX)/include
	install -d $(PREFIX)/lib
	install -m 644 include/safemem.h $(PREFIX)/include/
	install -m 644 libfmem.a $(PREFIX)/lib/
	@echo "Safe-Mem Installation Complete!"
	@echo "You can now use '#include <safemem.h>' and link with '-lfmem' anywhere."

# --- UNINSTALL TARGET ---
uninstall:
	@echo "Removing Safe-Mem from system..."
	rm -f $(PREFIX)/include/safemem.h
	rm -f $(PREFIX)/lib/libfmem.a
	@echo "Safe-Mem successfully uninstalled."
