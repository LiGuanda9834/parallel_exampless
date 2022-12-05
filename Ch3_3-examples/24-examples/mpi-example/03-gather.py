from mpi4py import MPI                                              
import numpy as np                                                  
                                                                    
comm = MPI.COMM_WORLD                                               
rank = comm.Get_rank()                                              
size = comm.Get_size()                                              
                                                                    
send_data = rank                                                    
print ("process {} send data {} to root...".format(rank, send_data))  
recv_data = comm.gather(send_data, root=0)                          
if rank == 0:                                                       
    print("process {} gather all data {}...".format(rank, recv_data))
