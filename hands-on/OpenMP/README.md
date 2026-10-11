An Introduction to Parallel Programming using OpenMP

This directory contains exercises and solutions for a hands-on
course that uses OpenMP to teach the core priciples of parallel progamming.
Information about these programs can be found in the comments and in the 
slides from the course.

Most of these programs can be compiled (assuming Linux and a gcc compiler)

  gcc -fopenmp program.c

The Solutions directory contains a number of solutions to the parallel programming
problems we explore in this course.  It is important, however, that you
DO NOT look at those solutions until you have completed the exercises.

The directory named Challenge_Problems contains additional exercises for 
more advanced students.  These programs have not been as carefully tested 
and may have problems building and running on some systems.

To support a wide range of systems, the makefiles reference an include file
called make.def.  A collection of them used for other systems can be found
in the directory Make_def_files.   
