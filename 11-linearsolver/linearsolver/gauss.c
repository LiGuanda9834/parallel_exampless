#include<stdio.h>
#define N 3
double A[3][3];
double b[3];
double x[3];

void Pause(){
   char s[128];
   fflush(stderr);
   fprintf(stderr, "<<< Press enter to continue >>>\n");
   fflush(stderr);
   fgets(s, sizeof(s), stdin);
   fflush(stderr);
   return;
}

void PrintLS(int step)
{
    int i,j;
    printf("================= step %d ==========================\033[1;41m b \033[0m===\n",step);
    for(i=0;i<3;i++){
	for(j=0;j<3;j++)
 	    printf("\033[1;32m%8.6f\t\033[0m",A[i][j]);
	printf("\033[1;31m %8.6f \033[0m\n",b[i]);
    }
//    Pause();
//    printf("==============================================\n");
    return ;
}

int main(int argc, char *argv[])
{
    int i,j,k;
    int step=0;
    A[0][0]=3.;A[0][1]=2.;A[0][2]=1.;
    A[1][0]=2.;A[1][1]=3.;A[1][2]=1.;
    A[2][0]=1.;A[2][1]=2.;A[2][2]=3.;
    b[0]=39.;b[1]=34.;b[2]=26.;
    PrintLS(step);
    double alpha;
    for(i=0;i<N-1;i++){
	for(j=i+1;j<N;j++)
	{
         alpha=A[j][i]/A[i][i];
//	 A[j][i]=0.;
	 for(k=i;k<N;k++)
	 {
	   A[j][k]-=A[i][k]*alpha; 
	 }
	 b[j]-=b[i]*alpha; 
	}
     step++;
     PrintLS(step);
    }

   for(i=N-1;i>0;i--){
       for(j=i-1;j>-1;j--){
	   alpha=A[j][i]/A[i][i];
	   for(k=i;k>j-1;k--)
	       A[j][k]-=A[i][k]*alpha;
	   b[j]-=b[i]*alpha;
       }
     step++;
       PrintLS(step);
   }

    printf("================= Resault =======================\n");
    printf(" x = %f / %f = \033[1;31m%f\033[0m\n",b[0],A[0][0],b[0]/A[0][0]);  
    printf(" y = %f / %f = \033[1;31m%f\033[0m\n",b[1],A[1][1],b[1]/A[1][1]);  
    printf(" z = %f / %f = \033[1;31m%f\033[0m\n",b[2],A[2][2],b[2]/A[2][2]);  

    return 0;
}
