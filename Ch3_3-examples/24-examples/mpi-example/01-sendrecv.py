from mpi4py import MPI                                                 
import numpy as np                                                     
                                                                       
comm = MPI.COMM_WORLD                                                  
rank = comm.Get_rank()                                                 
size = comm.Get_size()                                                 
                                                                       
if rank == 0:                                                          
    data = np.arange(10, dtype='i')                                    
    comm.Send([data, MPI.INT], dest=1, tag=11)                         
    print("process {} Send buffer-like array {}...".format(rank, data))
else:                                                                  
    data = np.empty(10, dtype='i')                                     
    comm.Recv([data, MPI.INT], source=0, tag=11)                       
    print("process {} recv buffer-like array {}...".format(rank, data))
