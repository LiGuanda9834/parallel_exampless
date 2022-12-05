#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#define DIM 2
#define true 1
#define false 0

int
main (int argc, char *argv[])
{
  int i, j, k, i1, j1, k1, flag;

  int nprocs;
  int myrank, newrank;
  int ndims[DIM];
  int cart_coord[DIM];
  int neighbour_proc[2][DIM];
  int periods[DIM];
  MPI_Comm MPI_COMM_CART;

  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  for (i = 0; i < DIM; i++)
    {
      ndims[i] = 0;
      periods[i] = true;
    }
  
  // 由MPI系统辅助确定各维方向上的进程数。
  // 如果数组ndims[i]不等于0，则说明第i维
  // 方向上的进程数由用户指定为ndims[i]。
  MPI_Dims_create (nprocs, DIM, ndims);

  for (i = 0, nprocs = 1; i < DIM; i++)
    nprocs *= ndims[i];

  if (myrank == 0)
    {
      fprintf (stderr,"Dimension arranged as: ");
      for (i = 0; i < DIM; i++)
	fprintf (stderr,"ndims[%d] = %d\t", i, ndims[i]);
      fprintf (stderr,"\nTotal %d process in using.\n", nprocs);
    }

  // 在初始通信器基础上，创建一个拥有DIM维笛卡尔拓扑结构的通信器。
  // 新通信器在所有坐标方向上均具有周期性，并且对所有进程重新排序。
  MPI_Cart_create (MPI_COMM_WORLD, DIM, ndims,
		   periods, false, &MPI_COMM_CART);

  // 获取本地进程在新通信器中的进程号。
  MPI_Comm_rank (MPI_COMM_CART, &newrank);
  
  // 获取本地进程在新通信器中的坐标。
  MPI_Cart_coords (MPI_COMM_CART, newrank, DIM, cart_coord);

#if 0
  fprintf (stderr,"\n\tmyrank %d <---> (", myrank);
  for (i = 0; i < DIM; i++)
    fprintf (stderr," %d ", cart_coord[i]);
  fprintf (stderr,")");
  fprintf (stderr,"  newrank: %d \n", newrank);

#else
  for (i = 0; i < DIM; i++)
    {
      //确定本地进程在各方向上的邻居。
      MPI_Cart_shift (MPI_COMM_CART, i, 1, &neighbour_proc[0][i],
		      &neighbour_proc[1][i]);
      if (i == 0)
        {
          fprintf (stderr,"\tnewrank %d: upper = %d, down = %d\n",
	           newrank, neighbour_proc[0][i], neighbour_proc[1][i]);
        }
      else if (i == 1)
        {
          fprintf (stderr,"\tnewrank %d: left = %d, right = %d\n",
	           newrank, neighbour_proc[0][i], neighbour_proc[1][i]);
        }
    }
#endif

  MPI_Finalize ();
  return (0);
}
