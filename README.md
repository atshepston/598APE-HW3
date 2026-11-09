# CS598APE HW3

This repository contains code for homework 3 of CS598APE. It is relatively
simple in comparison to HW1 and HW2 to ensure you have enough time to work on
the course project. 

`sandpile.c` contains the implementation of finding the identity element of a 2D
abelian sandpile.

![Identity sandpile on a 1001x1001 pacman grid](identity-ref.png)

*Identity sandpile on a 1001x1001 pacman grid*


To compile the program, run:

```bash
make all
```

To clean existing build artifacts, run:

```bash
make clean
```

Usage (after compiling):

```bash
./sandpile.exe <N> 
```

where `N` is the grid size

Consider speeding up the generation of this 1001x1001 identity sandpile (which
takes approximately ~3m 40s on my local laptop under the default setup)

```bash
./sandpile.exe 1001
```

For an optional, ungraded challenge, see how large a sandpile you can compute
on the course VMs and report the grid size and runtime. There is no prescribed
upper limit on `N` for this challenge.

## Artifact

Each commit contains the cumulative changes through that optimization:

| Commit | Optimization |
|---|---|
| `49ca01d` | Original baseline |
| `32fcbfd` | Hoisted geometry calculations |
| `e3f1574` | Precomputed column ranges |
| `a9d852c` | Batched toppling |
| `03bf12b` | Reverse traversal |
| `f8094b1` | Two-pass gather updates |
| `ee76392` | OpenMP parallelization |
| `4cb6288` | Native CPU instructions |

Select a stage, rebuild, and measure elapsed wall time using GNU Time:

```bash
git checkout <commit>
make all
/usr/bin/time -f '%e' ./sandpile.exe 401
/usr/bin/time -f '%e' ./sandpile.exe 1001
```

Requirements: Git, Make, GCC with OpenMP support, and GNU Time. Timing includes
initialization, stabilization, and PNG generation, but excludes compilation.
Reported results use the minimum of three runs for `401` and one run for `1001`.
Overall speedup is the baseline time divided by the optimized time at the same size.

Each run writes `identity.png`. Save the original baseline image separately for
each size, then compare later outputs byte for byte using, for example,
`cmp baseline-401.png identity.png`.
