CXX = g++
CXXFLAGS = -Iinclude -Wall -Wextra -std=c++17 -O3 -msse4.2
LDFLAGS = -lbenchmark -lpthread

# The final executables
all: test_driver benchmark simdtest

simdtest: src/safemem.o test/simdtest.o
	$(CXX) $(CXXFLAGS) $^ -o simdtest

benchmark: src/safemem.o test/benchmark.o
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o benchmark

test_driver: src/safemem.o test/test_driver.o
	$(CXX) $(CXXFLAGS) $^ -o test_driver

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o test/*.o test_driver benchmark simdtest
