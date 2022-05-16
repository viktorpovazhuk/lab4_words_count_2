# Lab work 3: Parallel indexing of words

Authors (team):

- [Petro Hentosh]()
- [Viktor Povazhuk](https://github.com/viktorpovazhuk)

## Prerequisites

Tools to be installed:

* on each OS:
    - pip
* on Windows:
    - MinGW
    - gcc (in MinGW)
    - cmake (in MinGW)
    - boost (in MinGW)
* on Linux:
    - gcc
    - cmake
    - boost
* on Apple:
    - gcc
    - cmake
    - boost

### Compilation

To compile program:

On Windows run MinGW and cd to repository directory.
```shell
[user@pc .] $ ./compile.sh --optimize-build
```

### Installation

Compiled executable: automatically installed in ./bin during compilation.

Create virtual python environment and install requirements in ./dependencies.

### Usage

1. Put your data in some folder on computer.
2. Specify options in configuration file in ./configs/index.cfg
3. Run the program or run the script.

Example of program run:

```shell
[user@pc .]$ cd bin
[user@pc bin]$ ./counter ../configs/index.cfg

Total=12767229
Reading=8022285
Finding=9437633
Writing=3329271
```

Example of script run:

```shell
[user@pc .]$ cd bin
[user@pc bin]$ python ../scripts/script.py
```

### Important!

Tested on Windows and Ubuntu.

### Results

<mark>DESCRIBE THE RESULTS OF THE WORK YOU DID. WHAT DID YOU LEARN OR FIND INTERESTING?</mark>

