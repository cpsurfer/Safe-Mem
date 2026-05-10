CXX = g++

# --- SYSTEM INSTALLATION PREFIX ---
PREFIX ?= /usr/local

# 1. Default Build Mode
MODE ?= release

# 2. Base Flags
# Added -fPIC for shared library support
CXXFLAGS = -Iinclude -Wall -Wextra -std=c++17 -msse4.2 -fPIC

# 3. Mode-specific Flags
ifeq ($(MODE), release)
	CXXFLAGS += -O3 -DNDEBUG -march=native
else
	CXXFLAGS += -g -O0
endif

LDFLAGS = -L/usr/local/lib -lbenchmark -lpthread

# Now building both .a and .so by default
all: libfmem.a libfmem.so test_driver benchmark simdtest

# --- LIBRARY GENERATION BLOCK ---

# Static library
libfmem.a: src/safemem.o
	ar rcs $@ $^
	@echo "✅ Static Library $@ generated."

# Shared library (Required for Python/Global use)
libfmem.so: src/safemem.o
	$(CXX) $(CXXFLAGS) -shared $^ -o $@
	@echo "✅ Shared Library $@ generated."

# --- EXECUTABLE BLOCK ---
simdtest: src/safemem.o test/simdtest.o
	$(CXX) $(CXXFLAGS) $^ -o simdtest

benchmark: src/safemem.o test/benchmark.o
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o benchmark

test_driver: src/safemem.o test/test_driver.o
	$(CXX) $(CXXFLAGS) $^ -o test_driver

# Compilation: .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- CLEAN TARGET ---
clean:
	rm -f src/*.o test/*.o test_driver benchmark simdtest libfmem.a libfmem.so result.json
	@echo "Cleaning complete."

# --- INSTALL TARGET (Global Access) ---
.PHONY: install uninstall
install: libfmem.a libfmem.so
	@echo "Installing Safe-Mem globally..."
	install -d $(PREFIX)/include
	install -d $(PREFIX)/lib
	install -m 644 include/safemem.h $(PREFIX)/include/
	install -m 644 libfmem.a $(PREFIX)/lib/
	install -m 755 libfmem.so $(PREFIX)/lib/
	@ldconfig || echo "⚠️  ldconfig failed. Please run 'sudo ldconfig' manually."
	@echo "Safe-Mem Installation Complete!"

# --- UNINSTALL TARGET ---
uninstall:
	@echo "Removing Safe-Mem from system..."
	rm -f $(PREFIX)/include/safemem.h
	rm -f $(PREFIX)/lib/libfmem.a
	rm -f $(PREFIX)/lib/libfmem.so
	@ldconfig
	@echo "Safe-Mem successfully uninstalled."
