CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

SRC = src/main.cpp src/object_store.cpp
OUT = mgit

build: 
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

clean: 
	rm -f $(OUT)
	rm -rf .mgit