#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DATA_SIZE 1000

void process_data(int *arr, int len)
{
#pragma omp parallel for
    for (int i = 0; i < len; i++)
    {
        int tid = omp_get_thread_num();
        int cpu_id = sched_getcpu();
        printf("Thread %d running on CPU %d, processing index %d\n", tid, cpu_id, i);
        arr[i] = (arr[i] + 1) * 2;
    }
}

int main(int argc, char **argv)
{
    int rank, world_size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int chunk = DATA_SIZE / world_size;
    int *local_data = (int *)malloc(chunk * sizeof(int));

    if (rank == 0)
    {
        int full_data[DATA_SIZE];
        for (int i = 0; i < DATA_SIZE; i++)
        {
            full_data[i] = i + 1; // 1'den 1000'e kadar sayılar
        }
        for (int node = 1; node < world_size; node++)
        {
            MPI_Send(&full_data[node * chunk], chunk, MPI_INT, node, 0, MPI_COMM_WORLD);
        }
        for (int i = 0; i < chunk; i++)
        {
            local_data[i] = full_data[i];
        }
    }
    else
    {
        MPI_Recv(local_data, chunk, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    process_data(local_data, chunk);

    if (rank == 0)
    {
        int final_data[DATA_SIZE];
        for (int i = 0; i < chunk; i++)
        {
            final_data[i] = local_data[i];
        }
        for (int node = 1; node < world_size; node++)
        {
            MPI_Recv(&final_data[node * chunk], chunk, MPI_INT, node, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        printf("Processed Data:\n");
        for (int i = 0; i < DATA_SIZE; i++)
        {
            printf("%d ", final_data[i]);
        }
        printf("\n");
    }
    else
    {
        MPI_Send(local_data, chunk, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    free(local_data);
    MPI_Finalize();
    return 0;
}
