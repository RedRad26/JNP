all:
	g++ -Wall -Wextra -std=c++17 -O2 -c encstrset.cc
	g++ -Wall -Wextra -std=c++17 -O2 -c encstrset_test2.cc
	gcc -Wall -Wextra -std=c11 -O2 -c encstrset_test1.c
	g++ encstrset.o encstrset_test1.o -o t1
	g++ encstrset.o encstrset_test2.o -o t2

clean:
	rm *.o
	rm t1 t2

	