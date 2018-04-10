// N16064103 Chun-Yii Liu, HW1 (method II)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>	// To use sin()
#include <time.h>

int main()
{
	clock_t t1, t2;						// t1 and t2 is in ms
	int i, j=0, k=1, N=1000000000;		// N is a billion
	double T1, T2, a=1.234, b=2.456;
	printf("For computational time of 1000000000 (a billion) '+', '*' and 'sin', \n");
	
	// For "+" operation ---------------------------------------------------------------
	t1 = clock();
	for(i=0;i<N;++i)
	{
		j = k+1;
		k = j+1;
	}
	t2 = clock();
	T1 = (t2-t1)/(double) CLOCKS_PER_SEC;
	printf("(+) time x 3 + 1 loop:%f\n",T1);
	//printf("(a,b)=%f %f\n", a,b);
	
	t1 = clock();
	for(i=0;i<N;++i)
	{
		j = k+1;
		k = j+1;
		j = k+1;
		k = j+1;
	}
	t2 = clock();
	T2 = (t2-t1)/(double) CLOCKS_PER_SEC;
	printf("(+) time x 6 + 1 loop:%f\n",T2);
	//printf("(a,b)=%f %f\n", a,b);
	printf("Real (+) time: %f\n",(T2-T1)/2.0);	
	//printf("(a,b)=%f %f\n\n", a,b);
}
