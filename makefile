CC = g++
CFLAGS = -O2 -I../c++ -mcx16 -march=native -DCILK -fcilkplus -std=c++17

all:	hopcroft-karp


hopcroft-karp:	hopcroft-karp.cpp
	$(CC) $(CFLAGS) -DNDEBUG hopcroft-karp.cpp -o hopcroft-karp

	
clean:
	rm -f hopcroft-karp
