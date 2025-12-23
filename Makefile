CXX = g++
CXXFLAGS = -Iinclude -Wall -Wextra -std=c++17

# The final executable
test_driver: src/safemem.o test/test_driver.o
	$(CXX) $(CXXFLAGS) src/safemem.o test/test_driver.o -o test_driver

# How to compile .cpp files into .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o test/*.o test_driver
