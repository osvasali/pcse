#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mpi.h"
#include <unistd.h>
#include "tools.h"

int main(int argc,char **argv) {

  MPI_Init(&argc,&argv);
  MPI_Comm comm = MPI_COMM_WORLD;

  int nprocs, procno;
  MPI_Comm_size(comm,&nprocs);
  MPI_Comm_rank(comm,&procno);

  srand((int)(procno*(double)RAND_MAX/nprocs));
  int num = rand() % 10; // random number to assign to processor
  double *dataset, *data_Sendrecv;
  dataset = (double *)num; // assign number to each processor
  data_Sendrecv = (double *)0; // used to compare values using MPI_Sendrecv

  int *size_buffer=NULL;
  if (procno==0) {
    size_buffer = (int*) malloc( nprocs*sizeof(int) );
  }

  MPI_Gather(&dataset,1,MPI_INT,size_buffer,1,MPI_INT,0,comm); // collect processor values

  if(procno == 0){
    printf("\noriginal:");
    for (int i_element=0; i_element<nprocs; i_element++) {
      printf("%d,",size_buffer[i_element]);
    }
    printf("\n\n");
  }

  //sorting
  for (int i = 0; i < nprocs-1; i++) {
    if (i%2 == 0 && procno%2 == 0 && procno != nprocs-1 ){
      MPI_Sendrecv(&dataset,1,MPI_DOUBLE, procno+1,0,
                   &data_Sendrecv,1,MPI_DOUBLE, procno+1,0, comm,MPI_STATUS_IGNORE);
      if(dataset > data_Sendrecv){
        dataset = data_Sendrecv; // keep data from processor to right
      }
    }
    if (i%2 == 0 && procno%2 != 0){
      MPI_Sendrecv(&dataset,1,MPI_DOUBLE, procno-1,0,
                   &data_Sendrecv,1,MPI_DOUBLE, procno-1,0, comm,MPI_STATUS_IGNORE);
      if(dataset < data_Sendrecv){
        dataset = data_Sendrecv;
      }
    }
    if (i%2 != 0 && procno%2 != 0 && procno != nprocs-1){
      MPI_Sendrecv(&dataset,1,MPI_DOUBLE, procno+1,0,
                   &data_Sendrecv,1,MPI_DOUBLE, procno+1,0, comm,MPI_STATUS_IGNORE);
      if(dataset > data_Sendrecv){
        dataset = data_Sendrecv;
      }
    }
    if (i%2 != 0 && procno%2 == 0 && procno != 0){
      MPI_Sendrecv(&dataset,1,MPI_DOUBLE, procno-1,0,
                   &data_Sendrecv,1,MPI_DOUBLE, procno-1,0, comm,MPI_STATUS_IGNORE);
      if(dataset < data_Sendrecv){
        dataset = data_Sendrecv;
      }
    }
    MPI_Gather(&dataset,1,MPI_INT,size_buffer,1,MPI_INT,0,comm);
    if(procno == 0){
      printf("gather %d: \n", i);
      for (int i_element=0; i_element<nprocs; i_element++) {
        printf("%d,",size_buffer[i_element]); // print gather
      }
      printf("\n");
    }
  }
  if(procno == 0){
    printf("\nsorted:");
    for (int i_element=0; i_element<nprocs; i_element++) {
      printf("%d,",size_buffer[i_element]); // print sorted
    }
    printf("\n\n");
  }

  MPI_Finalize();
  return 0;
  }
