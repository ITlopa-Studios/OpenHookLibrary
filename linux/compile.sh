g++ -fPIC -shared -o libopenhl.so openhl.cpp
g++ -c openhl.cpp -o openhl.o
ar rcs libopenhl.a openhl.o