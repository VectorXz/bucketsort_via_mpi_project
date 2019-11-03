/*
Sequential Bucketsort by Group 6 SEC1

Saranphon Phaithoon 6088114
Patiphol Pussawong 6088136
Supakarn Wongnil 6088137
Komolmarch Treechaiworapong 6088213

This code is borrowed from below website
https://www.thecrazyprogrammer.com/2017/02/bucket-sort-in-c.html
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

struct bucket
{
    int count;
    int* value;
    int local_min;
    int local_max;
};

int compareIntegers(const void* first, const void* second)
{
    int x = *((int*)first), y =  *((int*)second);
    if (x == y)
    {
        return 0;
    }
    else if (x < y)
    {
        return -1;
    }
    else
    {
        return 1;
    }
}

void bucketSort(int array[],int num, int bucket)
{
    struct bucket buckets[bucket];
    int i, j, k;
    float multiplier = 10000.0/bucket;

    printf("\n Multiplier = %f", multiplier);

    for (i = 0; i < bucket; i++)
    {
        buckets[i].count = 0;
        buckets[i].value = (int*)malloc(sizeof(int) * num);
        buckets[i].local_min = i * multiplier;
        buckets[i].local_max = (i+1) * multiplier;
        if(buckets[i].local_max == 9999) {
            buckets[i].local_max = 10000;
        }
    }

    for(i=0;i<bucket;i++) {
        for(j=0;j<num;j++) {
            if(array[j] >= buckets[i].local_min && array[j] < buckets[i].local_max) {
                buckets[i].value[buckets[i].count++] = array[j];
            }
        }
    }

    for(i=0;i<bucket;i++) {
        printf("\nBucket %d contains %d numbers (%d - %d)\n", i, buckets[i].count, buckets[i].local_min, buckets[i].local_max);
        for(j=0;j<buckets[i].count;j++) {
            printf("%d ",buckets[i].value[j]);
        }
        printf("\n\n");
    }

    int m,n,tmp;

    for (k = 0, i = 0; i < bucket; i++)
    {
        // now using quicksort to sort the elements of buckets
        //qsort(buckets[i].value, buckets[i].count, sizeof(int), &compareIntegers);

        for (m = 0; m < buckets[i].count; m++){
            for (n = m+1; n < buckets[i].count; n++){
                if (buckets[i].value[m] > buckets[i].value[n]){
                    tmp = buckets[i].value[m];
                    buckets[i].value[m] = buckets[i].value[n];
                    buckets[i].value[n] = tmp;
                }
            }
        }

        for (j = 0; j < buckets[i].count; j++)
        {
            array[k + j] = buckets[i].value[j];
        }
        k += buckets[i].count;
        free(buckets[i].value);
    }
}

int main(int argc, char* argv[]){

    clock_t t;
    t = clock();
    int bucket_no = atoi(argv[1]);

    printf("Bucket sorting 1 million numbers with bucket = %d\n\n",bucket_no);

    int num=1000000;
    int *array = (int*)malloc(num * sizeof(int));
    int i,j,k;
    srand(time(NULL));

    for(i=0;i<num;i++) {
        array[i] = rand() % 10000;
    }

    printf("Before Sorting\n");
    for (j = 0; j<num; j++)
    {
        printf("%d ", array[j]);
    }

    bucketSort(array, num, bucket_no);
    printf("\n After Sorting\n");
    for (k = 0; k<num; k++)
        printf("%d ", array[k]);

    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds

    printf("\n\nProgram took %f seconds to execute \n", time_taken);
    return 0;
}
