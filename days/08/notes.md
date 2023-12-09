So many specifics here.

Part 1 was easy enough.

Part 2 has all the signs of the AoC numerical unfeasibility.
It actually looked **a lot** like the "when do all buses arrive at the same time" tasks from a previous year.

It turns out not to be, however.
Part 2 is actually remarkable well-behaved.
There are no offsets on goal states in the state loops, and even a large common prime factor on *all* loop lengths.
My code would not work in a more general case.

I also think my code would not work with loops containing multiple goal states (hey-o AoC'24?)...

Part 2 actually has a curveball in the second example task, where the "true" loop has period 6, but all nodes within the loop have identical left/right exits, so the actual loop-to-goal-state period is 3.
My code does not deal with this, and I rely on the actual puzzle input having no such Mobius loops.
I hereby claim naming Mobius loops.

