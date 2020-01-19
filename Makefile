all: solver.out tester.out translator.out

%.out: %.c
	gcc -O3 $< -o $@

clean:
	rm -f *.out

.PHONY: all
.PHONY: clean
