#/bin/bash

g++ randomtest.cpp -o randomtest -O2 -Wall -g -std=c++0x
g++ smalltest.cpp -o smalltest -O2 -Wall -g -std=c++0x
g++ benchmark.cpp -o benchmark -DNDEBUG -O2 -Wall -g -std=c++0x -lrt
