#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <time.h>

#define K 5
#define MAX_ITERATIONS 1000

typedef struct {
  double x;
  double y;
} point;

point points[] = {{30.324161 , 97.602743 },
                  {30.32364238, 97.60284224},
                  {30.32364238, 97.60230312},
                  {30.324453 , 97.602394 },
                  {30.324997 , 97.603051 },
                  {30.32417317, 97.60266756},
                  {30.32417959, 97.60267311},
                  {30.32417756, 97.60266912},
                  {30.3241937 , 97.6026681 },
                  {30.32416856, 97.60266513},
                  {30.32417459, 97.60266231},
                  {30.32418453, 97.60266565},
                  {30.32417386, 97.60266459},
                  {30.32417388, 97.60266052},
                  {30.32418146, 97.60266554},
                  {30.32418475, 97.60266367},
                  {30.32417532, 97.6026676 },
                  {30.32417604, 97.60266726},
                  {30.32417108, 97.60266959},
                  {30.32418086, 97.60267227},
                  {30.32418659, 97.60266619},
                  {30.32417251, 97.60266379},
                  {30.32417398, 97.60266636},
                  {30.32418022, 97.60266541},
                  {30.32417901, 97.60266577},
                  {30.32418078, 97.60266294},
                  {30.32418021, 97.60266708},
                  {30.32418246, 97.60266482},
                  {30.32417327, 97.60266794},
                  {30.32418024, 97.60266564},
                  {30.324186 , 97.6026621 },
                  {30.32418388, 97.60266767},
                  {30.32417674, 97.60266799},
                  {30.32417925, 97.60266199},
                  {30.32417572, 97.60266512},
                  {30.32417839, 97.60266653},
                  {30.3241804 , 97.60266367},
                  {30.3241793 , 97.60266325},
                  {30.32417694, 97.60266519},
                  {30.32418074, 97.60266375},
                  {30.32418055, 97.60266182},
                  {30.32417837, 97.60266617},
                  {30.32418437, 97.60266894},
                  {30.32418593, 97.60266621},
                  {30.32418528, 97.60266775},
                  {30.32418166, 97.60266075},
                  {30.32418033, 97.60266941},
                  {30.32418216, 97.60266467},
                  {30.3241854 , 97.60266387},
                  {30.32418273, 97.6026636 }};

point centroids[K];

void initialize_centroids() {
  int i;
  for (i = 0; i < K; i++) {
    centroids[i].x = points[i].x;
    centroids[i].y = points[i].y;
  }
}

int find_closest_centroid(point p) {
  int i, closest_centroid_index = 0;
  double distance, min_distance = INFINITY;

  for (i = 0; i < K; i++) {
    distance = sqrt(pow(p.x - centroids[i].x, 2) + pow(p.y - centroids[i].y, 2));
    if (distance < min_distance) {
      min_distance = distance;
      closest_centroid_index = i;
    }
  }

  return closest_centroid_index;
}


double update_centroids(int iteration) {
  int i, j;
  double sum_x[K], sum_y[K];
  int num_points_in_cluster[K];
  double time_taken = 0.0;

  clock_t start_time = clock();

#pragma omp parallel for private(j)
  for (i = 0; i < K; i++) {
    sum_x[i] = sum_y[i] = num_points_in_cluster[i] = 0;

#pragma omp parallel for reduction(+:sum_x[i], sum_y[i], num_points_in_cluster[i])
    for (j = 0; j < sizeof(points) / sizeof(point); j++) {
      if (find_closest_centroid(points[j]) == i) {
        sum_x[i] += points[j].x;
        sum_y[i] += points[j].y;
        num_points_in_cluster[i]++;
      }
    }

    centroids[i].x = sum_x[i] / num_points_in_cluster[i];
    centroids[i].y = sum_y[i] / num_points_in_cluster[i];
  }

  clock_t end_time = clock();
  time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;
  //printf("Time taken for iteration %d: %f seconds\n", iteration, time_taken);

  return time_taken;
}


void print_clusters() {
  int i, j;
  point cluster_points[K][sizeof(points) / sizeof(point)];
  int cluster_sizes[K] = {0};

  for (j = 0; j < sizeof(points) / sizeof(point); j++) {
    int closest_centroid_index = find_closest_centroid(points[j]);
    cluster_points[closest_centroid_index][cluster_sizes[closest_centroid_index]] = points[j];
    cluster_sizes[closest_centroid_index]++;
  }
  printf("Mean Latitude and Longitude for Each Cluster:\n");
  for (i = 0; i < K; i++) {
    printf("Cluster %d: ", i);
    if (cluster_sizes[i] > 0) {
      printf("(%g, %g)", cluster_points[i][0].x, cluster_points[i][0].y);
    }
    printf("\n");
  }
}

int main() {
  int i, j, iteration_count = 0;
  int cluster_assignments[sizeof(points) / sizeof(point)];
  double max_time = 0.0;
  double total_time = 0.0;
  int max_iteration = 0;

  initialize_centroids();

  do {
    clock_t start_time = clock();

#pragma omp parallel for private(i) shared(points, cluster_assignments)
    for (i = 0; i < sizeof(points) / sizeof(point); i++) {
      cluster_assignments[i] = find_closest_centroid(points[i]);
    }

    double time_taken = update_centroids(iteration_count);

    if (time_taken > max_time) {
      max_time = time_taken;
      max_iteration = iteration_count;
    }

    iteration_count++;

    clock_t end_time = clock();
    total_time += (double)(end_time - start_time) / CLOCKS_PER_SEC;
  } while (iteration_count < MAX_ITERATIONS);

  printf("\n-------\nMax time taken: %f seconds (iteration %d)\n", max_time, max_iteration);
  printf("Total time taken: %f seconds\n", total_time);
  printf("Average time taken per iteration: %f seconds\n-------\n", total_time / MAX_ITERATIONS);
  print_clusters();

  return 0;
}
