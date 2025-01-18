// In this program we calculate the shortest route of a trip to solve the
// Travelling Salesman Problem (TSP).
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CITIES 64 // The maximum number of cities we will visit.
#define NO_PATH                                                                \
  UINT64_MAX // We assign the a very large number to indicate that there is no
             // path between 2 cities.

// A function to get index of each city in the cities array. It is a helping
// function to make sure that every city will only be included and thus visited
// once.
int get_city_index(char *cities[], int city_count, const char *city_name) {
  for (int i = 0; i < city_count; i++) {
    if (strcmp(cities[i], city_name) ==
        0) { // We loop through the cities array until we find a duplicate.
      return i;
    }
  }
  return -1;
}

// A function to determine the minimum-cost path. We divide the problem into sub
// problems by simulating all possible visits, then summing the costs to find
// the best route. We store minimum distances in a db table to avoid recomputing
// the same distances.
uint64_t tsp_dp(int current, uint64_t visited, int city_count,
                uint64_t di[MAX_CITIES][MAX_CITIES], uint64_t **dp,
                int **next_city) {
  if (visited == (1ULL << city_count) -
                     1) { // This is a binary representation of cities visited
                          // (bitmask). If all cities have been visited, then it
                          // will assign all cities with 1 and since we exclude
                          // the cost to return to the first city, we return 0.
    return 0;
  }

  if (dp[current][visited] != NO_PATH) {
    return dp[current]
             [visited]; // If the solution has already be computed and the
                        // distance is stored in dp array, we get the distance
                        // value instead of recalculating it again.
  }

  uint64_t min_cost = NO_PATH; // Initialize minimum cost.
  int best_next_city = -1;     // Initialize the best next city cost.

  for (int next = 0; next < city_count;
       next++) { // We loop through all the cities we can visit, simulating all
                 // possible routes to find the minimum cost.
    if (!(visited & (1ULL << next)) &&
        di[current][next] !=
            NO_PATH) { // If we have not visited the next city and there is a
                       // path to it, we calculate the cost and update next city
                       // to visited city.
      uint64_t cost =
          di[current][next] +
          tsp_dp(next,
                 visited |
                     (1ULL << next), // We recursively do the same for the next
                                     // city until all possible routes have been
                                     // covered and we sum the cost.
                 city_count, di, dp, next_city);

      // We find the minimum cost route and according to this the next city that
      // we will visit. Doing this for all routes, we can compare all costs to
      // find the minimum.
      if (cost < min_cost) {
        min_cost = cost;
        best_next_city = next;
      }
    }
  }
  // Finally we fill the dp array with the minimum cost routes and we get the
  // minimum cost. Since we recursively call this function, min_cost will be
  // updated to the minimum cost, while next_city will be filled with the best
  // route.
  dp[current][visited] = min_cost;
  next_city[current][visited] = best_next_city;
  return min_cost;
}

// A function to compute and print the results of tsp solution.
void solve_tsp(int city_count, uint64_t di[MAX_CITIES][MAX_CITIES],
               char *cities[]) {
  // Dynamically allocate the dp and next_city tables.
  uint64_t **dp = malloc(
      city_count *
      sizeof(
          uint64_t *)); // An array to store the city paths and their distances.
                        // It will help store the minimum cost paths.
  int **next_city = malloc(
      city_count *
      sizeof(int *)); // A similar array to store the possible next cities to
                      // vist. It will help determine all possible paths.
  for (int i = 0; i < city_count; i++) {
    // We allocate enough memory for every single city in the arrays.
    dp[i] = malloc((1ULL << city_count) * sizeof(uint64_t));
    next_city[i] = malloc((1ULL << city_count) * sizeof(int));
    for (uint64_t j = 0; j < (1ULL << city_count); j++) {
      dp[i][j] = NO_PATH; // We initialize all possible combination distances to
                          // NO PATH.
      next_city[i][j] = -1; // We initialize next city routes to -1.
    }
  }

  uint64_t result = tsp_dp(0, 1, city_count, di, dp,
                           next_city); // We compute the minimum cost route.

  if (result == NO_PATH) {
    printf("No valid TSP route found.\n"); // Error handling in case the file
                                           // only contains NO PATH routes.
  } else {
    printf("We will visit the cities in the following order:\n"); // Result.
    int current = 0;
    uint64_t visited = 1;
    uint64_t total_cost = 0;

    while (1) {
      int next = next_city[current][visited]; // Initialize next city.
      if (next == -1) {
        break; // Make sure there is path to the next city.
      }

      printf("%s -( %" PRIu64 " )-> %s\n", cities[current], di[current][next],
             cities[next]);
      total_cost += di[current][next]; // Total cost = sum of all min costs.
      visited |= (1ULL << next);
      current = next;
    }

    printf("Total cost: %" PRIu64 "\n",
           total_cost); // We print the total cost to visit the cities.
  }

  // Free dynamically allocated memory
  for (int i = 0; i < city_count; i++) {
    free(dp[i]);
    free(next_city[i]);
  }
  free(dp);
  free(next_city);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: ./tsp_solver <filename>\n"); // Error handling.
    return 1;
  }

  FILE *file = fopen(argv[1], "r");
  if (!file) {
    fprintf(stderr, "Error opening the file\n"); // Error handling.
    return 1;
  }

  char *cities[MAX_CITIES]; // We store the cities set a unique index for each
                            // city.
  uint64_t di[MAX_CITIES][MAX_CITIES]; // We store the distances of all cities.
  int city_count = 0;

  for (int i = 0; i < MAX_CITIES; i++) { // Initialize the distances to NO PATH
    for (int j = 0; j < MAX_CITIES; j++) {
      di[i][j] = NO_PATH;
    }
  }

  char line[100]; // Assuming each city doesn't have many characters, we store
                  // the characters of the line in this array to then store the
                  // seperate cities and distances to the corresponding tables.
  while (fgets(line, sizeof(line), file)) {
    char city1[512], city2[512]; // Use larger buffer for temporary storage
    uint64_t distance;
    if (sscanf(line, "%511[^-]-%511[^:]: %" SCNu64, city1, city2, &distance) !=
        3) {
      fprintf(stderr, "Error reading file\n");
      fclose(file);
      return 1;
    }

    // Check for city name length exceeding the limit.
    if (strlen(city1) >= 512 || strlen(city2) >= 512) {
      fprintf(stderr, "Error: City name exceeds the maximum allowed length of "
                      "511 characters.\n"); // We suppose that the user will not attempt to visit a city with 511 characters length.
      fclose(file);
      return 1;
    }

    // Check and allocate memory for city1 if it's not already in the list
    int city1_index = get_city_index(cities, city_count, city1);
    if (city1_index == -1) {
      cities[city_count] = malloc(strlen(city1) + 1);
      strcpy(cities[city_count], city1);
      city1_index = city_count++;
    }

    // Check and allocate memory for city2 if it's not already in the list
    int city2_index = get_city_index(cities, city_count, city2);
    if (city2_index == -1) {
      cities[city_count] = malloc(strlen(city2) + 1);
      strcpy(cities[city_count], city2);
      city2_index = city_count++;
    }

    di[city1_index][city2_index] = distance;
    di[city2_index][city1_index] = distance;
  }

  fclose(file);

  if (city_count > MAX_CITIES) {
    fprintf(stderr, "Error: Too many cities (maximum is %d).\n", MAX_CITIES);
    return 1;
  }

  if (city_count == 0) {
    fprintf(
        stderr,
        "Error: The input file is empty or contains no valid data.\n"); // Handle
                                                                        // empty
                                                                        // files.
    return 1;
  }

  solve_tsp(city_count, di, cities); // We compute and print the results.

  for (int i = 0; i < city_count; i++) {
    free(cities[i]);
  }

  return 0;
}
