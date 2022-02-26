CC = g++
CFLAGS = -O3 -I../c++ -mcx16 -march=native -DCILK -fcilkplus -std=c++17

all:	integral


hopcroft-karp:	hopcroft-karp.cpp
	$(CC) $(CFLAGS) -DNDEBUG hopcroft-karp.cpp -o hopcroft-karp

	
clean:
	rm -f hopcroft-karp
