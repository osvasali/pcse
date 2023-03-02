#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <string.h>

void transpose(MPI_Comm comm, int nprocs, int* data);
void print(MPI_Comm comm, int procno, int nprocs, int* data);

int main(int argc, char **argv) {

  MPI_Init(&argc, &argv);
  MPI_Comm comm = MPI_COMM_WORLD;

  int nprocs, procno; 
  MPI_Comm_size(comm, &nprocs);
  MPI_Comm_rank(comm, &procno);

  int data = procno;

  print(comm, procno, nprocs, &data);
  transpose_matrix(comm, nprocs, &data);

  MPI_Finalize();
  return 0;
}

void transpose(MPI_Comm comm, int nprocs, int* data) {
  MPI_Comm split_comm;
  int color, procno, split_comm_size, target_rank;
  int new;
  MPI_Comm_rank(comm, &procno);

  int ncols = sqrt(nprocs);
  int col_no = procno % ncols;
  int row_no = procno / ncols;

  if (row_no < ncols/2 && col_no < ncols/2) color = 0;
      
  else if (row_no >= ncols/2 && col_no < ncols/2) color = 1;
          
  else if (row_no < ncols/2 && col_no >= ncols/2) color = 2;
          
  else if (row_no >= ncols/2 && col_no >= ncols/2) color = 3;

  MPI_Comm_split(comm, color, procno, &split_comm);

  if (color == 1 || color == 2) {
      
    if (color == 2) target_rank = procno - ncols/2 + (ncols/2)*ncols;
        
    else if (color == 1) target_rank = procno + ncols/2 - (ncols/2)*ncols;
    MPI_Sendrecv(data, 1, MPI_INT, target_rank, 0, &new, 1, MPI_INT, target_rank, 0, comm, MPI_STATUS_IGNORE);
    *data = new;
  }

  MPI_Comm_size(split_comm, &split_comm_size);
  if (split_comm_size > 1) transpose_matrix(split_comm, split_comm_size, data);

  print(comm, procno, nprocs, data);
}

void print(MPI_Comm comm, int procno, int nprocs, int* data) {
  int* size_buffer = NULL;
  int sqrtnprocs = sqrt(nprocs);

  if (procno == 0) {
    size_buffer = (int*) malloc(nprocs * sizeof(int));
    memset(size_buffer, 0, nprocs * sizeof(int));
  }

  MPI_Gather(data, 1, MPI_INT, size_buffer, 1, MPI_INT, 0, comm);

  if (procno == 0) {
    printf("\nMatrix\n");
    printf("---------\n");
    for (int i_element = 0; i_element < sqrtnprocs; i_element++) {
      for (int j_element = 0; j_element < sqrtnprocs; j_element++) {
        printf("| %2d ", size_buffer[sqrtnprocs * i_element + j_element]);
      }
      printf("|\n");
      printf("---------\n");
    }
    free(size_buffer);
  }
}
