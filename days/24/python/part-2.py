
import sys
import z3

## This problem is one big equation system: we are looking for a line that
## intersects all hailstones' trajectories.
## The unknowns are:
## - rock origin (the puzzle solution)
## - rock velocity
## - time to impact for each hailstone
## In total, that is 6+1*hailstones unknowns.
## Taking the first 3 hailstones yields 3 equations (1 per x/y/z), so this
## should be overdetermined and no more hailstones are needed.
## It also does not matter which 3 hailstones we use, as the solution must hit
## all hailstones anyway.

## Parse first 3 lines of input file
lines = open(sys.argv[1]).read().splitlines()[:3]
first_three_hailstones = [[int(i) for i in line.replace("@", ",").split(",")] for line in lines]

## Setup solver and unknowns
solver = z3.Solver()

## Unknowns for thrown rock
rock_start_x = z3.Real("x")
rock_start_y = z3.Real("y")
rock_start_z = z3.Real("z")
rock_velocity_x = z3.Real("dx")
rock_velocity_y = z3.Real("dy")
rock_velocity_z = z3.Real("dz")

## Unknowns for hailstone impact times, and system equations
for hailstone_index in range(3):
    x, y, z, vx, vy, vz = first_three_hailstones[hailstone_index]
    hailstone_time = z3.Real("t{}".format(hailstone_index))
    solver.add(rock_start_x + hailstone_time * rock_velocity_x == x + hailstone_time * vx)
    solver.add(rock_start_y + hailstone_time * rock_velocity_y == y + hailstone_time * vy)
    solver.add(rock_start_z + hailstone_time * rock_velocity_z == z + hailstone_time * vz)

## Solve system
if solver.check() != z3.sat:
    print("could not solve")

## Retrieve solution
x = solver.model().eval(rock_start_x).as_long()
y = solver.model().eval(rock_start_y).as_long()
z = solver.model().eval(rock_start_z).as_long()

print(x, y, z)
print("Sum of thrown rock's origin coordinates:", x + y + z)

