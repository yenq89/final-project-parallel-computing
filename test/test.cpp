 // #include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
int main(int argc, char* argv[])
{
    int numprocs, rank, tag = 100, msg_size=64;
    char *buf;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    if (numprocs != 2) {
        printf("The number of processes must be two!\n");
        MPI_Finalize();
        return(0);
        }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    printf("MPI process %d started...\n", rank);
    fflush(stdout);
    while (msg_size < 10000000) {
        msg_size = msg_size *2;
        buf = (char *)malloc(msg_size * sizeof(char));
        if (rank == 0) {
            MPI_Send(buf, msg_size, MPI_BYTE, rank+1, tag, MPI_COMM_WORLD);
            printf("Message of length %d to process %d\n",msg_size,rank+1);
            fflush(stdout);
            MPI_Recv(buf, msg_size, MPI_BYTE, rank+1, tag,MPI_COMM_WORLD,
            &status);
            }
        if (rank == 1) {
            // MPI_Recv(buf, msg_size, MPI_BYTE, rank-1, tag, MPI_COMM_WORLD,
            // &status);
            MPI_Send(buf, msg_size, MPI_BYTE, rank-1, tag, MPI_COMM_WORLD);
            printf("Message of length %d to process %d\n",msg_size,rank-1);
            fflush(stdout);
            MPI_Recv(buf, msg_size, MPI_BYTE, rank-1, tag, MPI_COMM_WORLD,&status);
            }
        free(buf);
        }
    MPI_Finalize();
}