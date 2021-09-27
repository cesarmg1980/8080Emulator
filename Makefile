tests: tests_chip8080.o src/chip8080.c src/tools.c
	gcc tests_chip8080.o src/tools.c src/chip8080.c -o test -lcmocka && ./test

tests_chip8080.o: tests/tests_chip8080.c src/chip8080.c src/tools.c
	gcc -c tests/tests_chip8080.c src/chip8080.c src/tools.c

clean:
	rm -fv *.o
	rm -fv *.out
	rm -fv src/*.o 
	rm -fv src/*.out
	rm -fv tests/*.o
	rm -fv tests/*.out
