# GoGraph
This repository contains the code implementation for the paper titled "Fast Iterative Graph Computing with Updated Neighbor States."

## Introduction

GoGraph introduces a graph reordering algorithm designed to establish an optimized sequence for graph processing. Its primary objective is to minimize the number of iteration cycles required, thereby enhancing the speed of iterative computations.

## Installation

> Dependencies

Ensure the following dependencies are installed on your system:

- g++ (version 4.9.2 or newer)
- Boost C++ Libraries (version 1.58.0)
- libnuma-dev (version 2.0.12-1)
- libtcmalloc_minimal (version 2.7-1ubuntu2)

> Building GoGraph

Clone the repository and navigate into the project directory:



```
git clone https://github.com/zhouyijie9/GoGraph
cd GoGraph
```

## Usage

To reorder a graph dataset, first place your graph files in the `dataset` folder. Then open a terminal and execute:

```
./run_all.sh [GRAPH_NAME]
```

Replace [GRAPH_NAME] with the name(s) of the graph dataset(s) you intend to reorder. It is possible to specify one or multiple filenames.

For instance, to reorder `indochina.mtx` and `web-google.mtx`, you would enter:
```
./run_all.sh indochina.mtx web-google.mtx
```

## Input Format

Input should be provided as an edge file for the dataset, where each line comprises two non-negative integers representing vertex IDs, separated by a space. An example from the `indochina.mtx` file in the `dataset` folder is as follows:

```
551 1
11338 1
70 2
71 2
414 2
```

## Output Format


Each line in the output file lists two IDs, corresponding to the reordered vertex IDs from the original file.

```
5145 975
5010 975
4721 4745
4715 4745
4769 4745
```

The output files are located in the same directory as the dataset, with the suffix `.GoGraph`.

## Test

Within the `test` directory, you'll find a file named `pagerank-async.cpp`. Use this file to test the effectiveness of the reordered files.

```
cd test
g++ pagerank-async.cpp -std=c++11 -o pagerank.out
./pagerank.out ../dataset/indochina.mtx.GoGraph
```
