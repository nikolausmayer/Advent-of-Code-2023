Ah yes, the classical path search task.
It would not be christmas (or AoC) without it.

Part 1 is simple and straightforward.

Part 2 is a very slight simplification of the rules, which alone makes the task itself simpler, but the search space significantly larger.
The complication is not huge, however, so it was enough to rewrite the path search to first identify nodes (I used path crossings) and compute path lengths between those, and then run the main search between these nodes.
