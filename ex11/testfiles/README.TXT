** Read this first **

For the memory.txt output, the following assumptions are made for the Buddy implementation:
- The reserved memory area is not fixed and can be moved if there is no real-time job running;
- The minimum block size is 1 MB; and
- The size of each block is a power of two.

Possible output for a Buddy implementation that fixes the reserved memory area at offset 0 MB is given in altbuddymem.txt

The third line of the resource.txt file is wrongly given as 0_1_2_64_2_0_0_1 in OUTPUTS.pdf; it should be 0_1_2_64_2_0_0_0.

The outputs for feedback1.txt and mixed1.txt assume that the “small bug” in the algorithm for exercise 11 is not fixed. If the bug is fixed, the processes should still appear in the same order with the same timings but without the unnecessary suspend and resume signals.

If you have implemented a Buddy memory allocation scheme, the combined.txt output from your host dispatcher must not include the ticks from the job that requests 770 MB of memory. Instead, the job must be rejected.
