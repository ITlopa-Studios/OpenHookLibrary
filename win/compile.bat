g++ -O2 -Wall -shared -o openhl.dll openhl.cpp -Wl,--out-implib,OpenHL.lib -std=c++14
g++ -O2 -Wall -c openhl.cpp -o openhl.o -std=c++14
ar rcs OpenHL.lib openhl.o
pause