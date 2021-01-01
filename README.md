# WWcpu-boinc

WWcpu by Bryan Little and Yves Gallot

A BOINC-enabled multithreaded CPU Wieferich and WallSunSun prime search program.

Based on wwwwcl by Mark Rodenkirch.

A checksum will be printed at the end of the results-WWocl.txt upon completion of the test range.
This checksum will allow comparing results between different hardware.


## How it works

This is the CPU implementation of the OpenCL version of this program.
It was mainly developed to allow those without access to GPUs to search for Wieferich/WallSunSun primes.
The OpenCL version is much faster and is recommended.

This program was developed to run on the BOINC platform where results are validated against other computers.
To be able to compare the result checksum between the two programs the same algorithm must be used.
This means the CPU must test base 2 PRPs for Wieferich and WallSunSun like the GPU.  A better
approach would be to use a fast sieve of Eratosthenes like primesieve by Kim Walisch to generate
the candidiates.  However, this would result in a checksum mismatch with the GPU program when a pseudoprime
is tested.  Since creating a fast GPU sieve of Eratosthenes is challenging because of the large number
of memory accesses required, it is easier to adapt the CPU to test like the GPU.

1) A search range is given on the command line.

2) The cpu takes a small chunk of this range and generates a base 2 probable prime.
   These are "industrial grade primes" requiring ~7 times fewer calculations than testing for primality.
   This way we can quickly find candidate primes to Wieferich / WallSunSun test and remove any false 
   positives later.

4) The prp is tested for being a Wieferich prime.  A checksum is recorded for each test.

5) The prp is tested for being a WallSunSun prime.  A checksum is recorded for each test.

6) This repeats until the checkpoint range has been reached.  Results are verified for primality and 
   recorded to the solutions file.  A checksum is updated at each checkpoint.


## Running the program stand-alone

Usage: WWcpu START END -t #

where 127 <= START < END < 2^64

and -t # or --nthreads # is optional number of threads to launch.  Max is 64.

Example format: WWcpu 10000000 20000000 -t 8
            Or: WWcpu 1e7 2e7 -t 8



