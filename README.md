# Traveling Salesman Problem (TSP) Solver

This program solves the **Traveling Salesman Problem (TSP)** using a **Dynamic Programming** approach with **Bitmasking**. Given a list of cities and their pairwise distances, it computes the shortest route that visits each city exactly once and returns to the starting city.

## Features
- Computes the minimum cost route for a traveling salesman to visit all cities.
- Uses Dynamic Programming (DP) with Bitmasking to optimize the search for the shortest path.
- Handles up to 64 cities (modifiable with `MAX_CITIES` constant).
- Reads input data from a file with the format:  
  `City1-City2: Distance`

## Prerequisites
This program requires a C compiler that supports C99 (or later). It uses the standard C libraries, so no additional dependencies are needed.

## Input Format
The program reads a file containing city names and their pairwise distances. Each line in the file should be formatted as follows:

City1-City2: Distance


Where:
- `City1` and `City2` are the names of two cities (up to 511 characters each).
- `Distance` is the non-negative integer distance between the two cities.

### Example input:

New York-Los Angeles: 2451 Los Angeles-Chicago: 2015 Chicago-New York: 787

## Compilation
To compile the program, use a C compiler. For example, using `gcc`:

```sh
gcc -o tsp_solver TSP.c
```

# Usage
After compiling the program, you can run it with the following command:
```sh
./tsp_solver <filename>
```
Where <filename> is the name of the input file that contains the cities and distances.

## Example Usage
```sh
./tsp_solver input.txt
```
### Output
The program will print the optimal order of cities to visit along with the corresponding cost of the journey. For example:
```sh
We will visit the cities in the following order:
New York -( 787 )-> Chicago
Chicago -( 2015 )-> Los Angeles
Los Angeles -( 2451 )-> New York
Total cost: 5253
```

If no valid TSP route is found, the program will output:

```sh
No valid TSP route found.
```

# Memory Management
* The program dynamically allocates memory for the cities, distance matrix, DP table, and the next city table.
* Memory is freed after the computation to prevent memory leaks.

Error Handling
The program includes basic error handling:

# Error handling
* If the file cannot be opened, it will print an error message.
* If there are issues with the input format, such as invalid city names or distances, the program will exit with an error message.
* If the number of cities exceeds MAX_CITIES (currently set to 64), the program will terminate with an error message.
* The program will handle empty input files or files with no valid data.

# Limitations
* The program uses Dynamic Programming with Bitmasking, making it suitable for small problem sizes. For very large numbers of cities, the program may run out of memory or take a long time to compute.

* The maximum number of cities is set to 64 by default. If you need to handle more cities, you can modify the MAX_CITIES constant.
