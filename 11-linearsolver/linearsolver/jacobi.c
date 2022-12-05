#include<stdio.h>
#include<math.h>
#define N 3
double A[3][3];
double b[3];
double x[3];
double res;

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
    printf("================= step %d ==============\033[1;41m b \033[0m  ======== \033[1;41m x \033[0m===\n",step);
    for(i=0;i<3;i++){
	for(j=0;j<3;j++)
 	    printf("\033[1;32m%.6f    \033[0m",A[i][j]);
	printf("\033[1;31m %.6f\033[0m    \033[1;32m %.6f\033[0m\n",b[i],x[i]);
    }
   printf(" ||b-Ax|| = %le\n\n",sqrt(res));
//    Pause();
    return ;
}

int main(int argc, char *argv[])
{
    int i,j,k;
    int step=0;
    double x1[N];
    A[0][0]=3.;A[0][1]=2.;A[0][2]=1.;
    A[1][0]=2.;A[1][1]=3.;A[1][2]=1.;
    A[2][0]=1.;A[2][1]=2.;A[2][2]=3.;
    b[0]=39.;b[1]=34.;b[2]=26.;
    x[0]=0.;x[1]=0.;x[2]=0.;
    PrintLS(step);
    while(step<1000){
    for(i=0;i<N;i++){
	x1[i]=0;
       for(j=0;j<N;j++)
	{
	  if(i!=j)
	   x1[i]+=A[i][j]*x[j];
	}
       x1[i]=(b[i]-x1[i])/A[i][i];
     }

     for(i=0;i<N;i++)
	 x[i]=x1[i];
     res=0;
     for(i=0;i<N;i++){
	 x1[i]=b[i];
	 for(j=0;j<N;j++)
	   x1[i]-=A[i][j]*x[j];
	 res+=x1[i]*x1[i];
     }
     step++;
     PrintLS(step);
     if(sqrt(res)<1e-8)  break;
    }

    printf("================= Resault =======================\n");
    printf(" x =  \033[1;31m%f\033[0m\n",x[0]);  
    printf(" y =  \033[1;31m%f\033[0m\n",x[1]);  
    printf(" z =  \033[1;31m%f\033[0m\n",x[2]);  

    return 0;
}
