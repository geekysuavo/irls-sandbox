
# The IRLS sandbox

A safe place to play with _iteratively reweighted least squares_ (IRLS),
and _variationally reweighted least squares_ (VRLS), algorithms for
estimating sparse solutions to underdetermined linear systems.

Technical details of the algorithms are in preparation for submission to:

> Worley, B., _Variationally reweighted least squares for
> sparse recovery_, Journal of Machine Learning Research, 2019.

## Introduction

FIXME

## Organization

### Solver implementations

The source code for all implemented algorithms is stored in [src](src).

### Documentation

In order to establish a baseline level of notation for the algorithms
implemented in [src](src), a LaTeX writeup was created in [notes](notes)
that provides a sketch of the derivations required to obtain each
algorithm.

### Task management

This project uses [doit](https://github.com/pydoit/doit) to manage tasks,
including compiling solver source code, running experiments, and making
figures. All task management code is stored in [sandbox](sandbox).

## Usage

FIXME
The `make.py` script handles all tasks using _doit_. To see a listing
of the supported tasks, simply run:

```bash
./make.py list --all
```

To see a listing of supported commands, run:

```bash
./make.py help
```

Running `make.py` without arguments will execute all tasks. Unless you're
going on a long coffee break, this is not recommended.

## Licensing

The **irls-sandbox** sources are released under the
[MIT license](https://opensource.org/licenses/MIT). See the
[LICENSE.md](LICENSE.md) file for the complete license terms.

~ Brad.

