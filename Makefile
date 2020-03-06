all: solver.out tester.out translator.out par_solver.out par_translator.out

par_solver.out: par_solver.c
	gcc -O3 -fopenmp $< -o $@

%.out: %.c
	gcc -O3 $< -o $@

clean:
	rm -f *.out

.PHONY: all
.PHONY: clean
