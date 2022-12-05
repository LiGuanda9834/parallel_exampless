#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

struct new_type
     {
       char char_var;
//       char char_var_1;
       int int_var;
       double double_var;
//       char char_var_2;
//       char char_var_3;
//       char char_var_4;
     };

int 
main(int argc, char *argv[])
{
  struct new_type var[1];
  MPI_Init(&argc, &argv);
  fprintf (stderr, "sizeof double   = %d\n", sizeof (double));
  fprintf (stderr, "sizeof char     = %d\n", sizeof (char));
  fprintf (stderr, "sizeof int      = %d\n", sizeof (int));
  fprintf (stderr, "sizeof new_type = %d\n", sizeof (struct new_type));

  fprintf (stderr, "address of var[0].double_var = %p\n", &var[0].double_var);
  fprintf (stderr, "address of var[0].char_var   = %p\n", &var[0].char_var);
//  fprintf (stderr, "address of var[0].char_var_1  = %p\n", &var[0].char_var_1);
  fprintf (stderr, "address of var[0].int_var    = %p\n", &var[0].int_var);
//  fprintf (stderr, "address of var[0].char_var_2 = %p\n", &var[0].char_var_2);
//  fprintf (stderr, "address of var[0].char_var_3 = %p\n", &var[0].char_var_3);
//  fprintf (stderr, "address of var[0].char_var_4 = %p\n", &var[0].char_var_4);

#if 0  
  fprintf (stderr, "\naddress of var[1] = %p\n", &var[1]);

  fprintf (stderr, "address of var[1].double_var = %p\n", &var[1].double_var);
  fprintf (stderr, "address of var[1].char_var = %p\n", &var[1].char_var);
  fprintf (stderr, "address of var[1].int_var = %p\n", &var[1].int_var);
#endif
  MPI_Finalize();
  return(0);
}
