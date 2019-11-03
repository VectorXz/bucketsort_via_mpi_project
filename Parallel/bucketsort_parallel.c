/*
Parallel Bucketsort via MPI by Group 6 SEC1

Saranphon Phaithoon 6088114
Patiphol Pussawong 6088136
Supakarn Wongnil 6088137
Komolmarch Treechaiworapong 6088213

This code is borrowed from below website
https://sites.google.com/a/g.clemson.edu/cpsc362-fa2013/home/bucketsort-v1
*/
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 1000000

int main(int argc, char* argv[]){

  int rawNum[N];
  int sortNum[N];
  int* local_bucket;
  int rank,size;
  int* proc_count;
  int* disp;
  MPI_Status status;
  int i,j,counter;
  int local_min,local_max;
  float multiplier;
  int tmp;

  MPI_Init(&argc,&argv);

  

  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);
 if (rank == 0){
   srand(time(NULL));
   //generate N number (0-9999) into array rawNum
    for (i = 0; i < N; i++){
      rawNum[i] = rand() % 10000;
    }

    //send total array rawNum to each bucket (processor)
    for (i = 1; i < size; i++){
      MPI_Send(rawNum,N,MPI_INT,i,1,MPI_COMM_WORLD);
    }
  }
  else {

    //each process receive total array rawNum
    MPI_Recv(rawNum,N,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
  }

/*
  for (i = 0; i < N; i++){
    printf("Rank %d Num %d \n",rank,rawNum[i]);
  }
*/

  counter = 0;
  //define range of each bucket
  multiplier = 10000.0/size;
  local_min = rank * multiplier;
  local_max = (rank + 1) * multiplier;
  if(local_max == 9999) {
    local_max = 10000;
  }

  //printf("For rank %d max is %d min is %d (%f)\n",rank,local_max,local_min,multiplier);

  //count for total number that in range of each bucket (to allocate memory / create array)
  for (i = 0; i < N; i++){
    if ((rawNum[i] >= local_min) && (rawNum[i] < local_max)){
      counter += 1;
    }
  }
  

  //allocate memory / create array of local bucket (use count size)
  local_bucket = malloc(counter * sizeof(int));

  //reset counter and put in range number in local array (user counter as a position in local array)
  counter = 0;
  for (i = 0; i < N; i++){
    if ((rawNum[i] >= local_min) && (rawNum[i] < local_max)){
      local_bucket[counter] = rawNum[i];
      counter += 1;
    }
  }

  //printf("RANK %d : got total %d numbers\n",rank, counter);
  
  double tbeg = MPI_Wtime();
  //do normal sort
  for (i = 0; i < counter; i++){
    for (j = i+1; j < counter; j++){
      if (local_bucket[i] > local_bucket[j]){
        tmp = local_bucket[i];
        local_bucket[i] = local_bucket[j];
        local_bucket[j] = tmp;
      }
    }
  }
  //qsort(local_bucket, counter, sizeof(int), &compareIntegers);

  double elapsedTime = MPI_Wtime() - tbeg;

/*
  for (i = 0; i < counter; i++){
    printf("%d %d \n",rank,local_bucket[i]);
  }
*/
  // set up root process
  if (rank == 0){
    //allocate processor size array to calculate displacement
    proc_count = malloc(size * sizeof(int));
    disp = malloc(size * sizeof(int));
  }

  // populate proc_count
  // use counter to count as a member of each processor and keep in proc_count (to calculate displacement)
  MPI_Gather(&counter,1,MPI_INT,proc_count,1,MPI_INT,0,MPI_COMM_WORLD);

/*
  if (rank == 0){
    for (i = 0; i < size; i++) printf("%d ",proc_count[i]);
    printf("\n");
  }
*/

  //calculate displacement for each processor (use displacement to order in global array)
  if (rank == 0){
    disp[0] = 0;
    for (i = 0; i < size-1; i++){
      disp[i+1] = disp[i] + proc_count[i];
    }
  }


  /*if (rank == 0){
    for (i = 0; i < size; i++) printf("[%d] ",disp[i]);
    printf("\n");
  }*/


  // receive final result
  // Gather local array to global array (sortNum) and use displacement as we calculate as a order of each processor elements
  MPI_Gatherv(local_bucket,counter,MPI_INT,sortNum,proc_count,disp,MPI_INT,0,MPI_COMM_WORLD);

  

  printf("\n\nRANK %d {%d-%d}[%d numbers]\n",rank,local_min,local_max,counter);

  double totalTime;
  MPI_Reduce( &elapsedTime, &totalTime, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD );

  if (rank == 0){
    printf("Before sort: \n");
    for (i = 0; i < N; i++) printf("%d ",rawNum[i]);
    printf("\nAfter sort: \n");
    for (i = 0; i < N; i++) {
      printf("%d ",sortNum[i]);
    }
    printf("\n");
    printf( "\nTotal time spent in seconds is %f\n", totalTime );
  }

  MPI_Finalize();
  return 0;
}
