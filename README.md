# Dance Show Scheduler
This repository contains C programs that can be used to find schedules for dance shows
to avoid dancers having too short of changes. Specifically, using these programs, dancers
are always given at least two numbers between consecutive dances.

All programs can be compiled to executables using the `make` command.

The main program is provided in solver.c, and is compiled by the Makefile to an executable
named solver.out. This program takes 3 files as input, and is executed in the format:
```
./solver.out <dance list> <dancer list> <restrictions>
```

Examples of all of these can be found in the `example` directory.

### Dance List
The `<dance list>` file is a list of dance names, one per line. As a note, for shows with
an intermission, this dance list should contain dances that are placeholders for intermission,
so there should be two dances `Intermission 1` and `Intermission 2`. In addition, the order of
the dances in this file is roughly the order that dances are explored in, so the initially 
generated show orders will be pretty similar to the order the dances are provided in, and as the
program runs for longer lengths of time, the show orders generated will differ more and more.

### Dancer List
The `<dancer list>` file is a list of dancer names followed by the names of the dances they are
in (separated by commas).

### Restrictions
The `<restrictions>` file provides the positions of any dances that are required to be placed at 
a certain location. For example, if you want the first dance to be "Dance C", then the first line
of the restrictions file should say "Dance C". Any place in the show order that does not have a
dance assigned to it, should be left as a blank line.

## Further Processing
The translator.c program, which is compiled to a translator.out executable, can be used to take
the output of the solver and convert it from the numbers of dances to names of dances. The easiest
way to do this is to pipe the output of the solver into the translator, int he form:
```
./solver.out <dance list> <dancer list> <restrictions> | ./translator.out <dance list>
```

Because the solver will (generally) generate many show orders, there is also a tester.c program
which is compiled to a tester.out executable that can be used to further narrow the number of
show orders to consider. This program finds the longest run any dancer has (a run being the number
of dances that a dancer has where they are all separated by just 2 dances). This tester program will
take the show orders generated by the solver and only output the first generated show order and then
only output show orders where the longest run is shorter than the previously output show order, or
less dancers have a run of that length. This can be run using:
```
./solver <dance list> <dancer list> <restricgtions> | ./tester.out <dance list> <dancer list>
```

## Parallelization
*Note: The parallel solver described below has not been sufficiently tested.* 

The par\_solver.c program works the same as the normal solver.c program except that it designed
to run multiple threads to parallel to explore a wider range of the search space. This parallelization
is achieved through the OpenMP extension in GCC, and is set to utilize as many threads as the
host machine has. The actual code changes necessary to make this change are relative simple. One
note on usage, the results of ./par\_solver.out need to be translated from binary before they can
be used by the further processing features described above. To do this, pipe the results from the
./par\_solver.out into ./par\_translator.out which must be provided the number of dances as its one
command-line argument. 
