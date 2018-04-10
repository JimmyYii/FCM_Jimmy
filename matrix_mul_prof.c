#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>	// Why I need this, some don't need 

int main()
{
	clock_t t1, t2;					// variables for computing clocks
	double **A, *x, *b, *c, T1, T2;		// x = 1234, a location in memory, x[0] the value at memory 1234, 1235, 1236, ... 1041
	int **C;
	int i, j, k, L, M, N=10000;		//
	
	T1 = 1.2;
	x = &T1;
	printf("%u %f\n", x, x[0]);
	//printf("%u %f\n",x,x[0]);	//u: unsigned

	printf("%d\n", rand());	// Will be the same in different execution
	printf("%d\n", rand());
	printf("%d\n", rand());
	printf("%d\n", rand());
	printf("%d\n", rand());
	printf("-------------\n");

	srand(time(NULL)); 		// Set random seed at present time
	printf("%d\n", rand());
	printf("%d\n", rand());
	printf("%d\n", rand());
	printf("%d\n", rand());
	printf("%d\n", rand());
	printf("-------------\n");
	
	//-------------------------------------------- Create random elements of matrix and vector
	
	// Method 1
	//double A[10000][10000];		// Directly give sizes of matrix or array
	//double x[10000];
	
	for(N=16;N<=16;N*=2)
	{												   // N=4, sizeof(double*)=4, 16 bytes
													   // 1234(00), 1235(00), 1236(04), 1237(00)... 
													   // A = 1234
													   // A[0] = (00000400)_HEX = 1024, A[1] = 1238..1241
													   // A[0][0],   memory 1024, 1025, 1026 ,.... 1031 => double 8 bytes
													   // A[0][1],   memory 1032, 1033, 1034 ,.... 1039
		
		A = (double **) malloc( N * sizeof(double*) ); // Take N double* memory,
													   // and put the initial pointer at A, A[0], A[1],...
													   // But the values of A[0], A[1], A[2],... A[N-1] are not given
		
		A[0] = (double *) malloc( N*N*sizeof(double)); // Take N*N double* memory,
													   // and put the initial pointer at A[0]
													   // A[0][0], A[0][1],..., A[0][N*N-1] are not given yet
		
		for(i=1;i<N;++i) A[i] = A[i-1] + N;			   // A[1] = A[0]+N ==> A[1][0] = A[0][N]
													   // A[2] = A[1]+N = A[0]+2N ==> A[2][0] = A[0][2N]
													   // case: N = 3
													   // A[0][0] A[0][1] A[0][2]
													   // A[1][0] A[1][1] A[1][2] ==> A[0][3], A[0][4], A[0][5]
													   // A[2][0] A[2][1] A[2][2] ==> A[0][6], A[0][7], A[0][8] 
		// A[1]=A[0]+N --> A[2]=A[1]+N --> A[3]=A[2]+N : Which is not possible to parallelize
		// , because they are dependent
		
		#pragma omp parallel for
		for(i=1;i<N;++i) A[i] = A[0] + i*N;
		// A[1]=A[0]+N --> A[2]=A[0]+2N --> A[3]=A[0]+3N : Which is able to be parallelized

		x = (double *) malloc( N * sizeof(double) );
		b = (double *) malloc( N * sizeof(double) );
		c = (double *) malloc( N * sizeof(double) );
		
		C = (int **) malloc(N*sizeof(int*));	// Create memory of C
		C[0] = (int *) malloc(N*N*sizeof(int));
		for(i=1;i<N;++i) C[i] = C[0]+i*N; 
		 
		M = N/4;
		//srand() below need a overall parallel here
		#pragma omp parallel num_threads(4) private(i,j,k,L)
		{
			k = omp_get_thread_num();
			printf("The seed at thread %d is : %d\n",k,time(NULL)>>k);	// >> : divided by 2
			srand(time(NULL)>>k);			// Setting initial value in each thread 
			//#pragma omp parallel for		// Then go on to take random number
			for(i=k*M;i<(k+1)*M;++i)
			{
				//L = omp_get_thread_num();
				//printf("thread %d, %d\n",k,L);
				for(j=0;j<N;++j)
				{
					A[i][j] = rand();
					C[i][j] = rand() % 10;
				}
				x[i] = rand();
			}
		}
		for(i=0;i<N;++i) 
		{ 
			for(j=0;j<N;++j)
			{
				printf("%d ",C[i][j]);
			}
			printf("\n");
		}
		
		// ---------------------------------------------- Do Ax = b
		
		// For correct answer
		double t;
		t1 = clock();
		for(i=0;i<N;++i) 
		{
			t = 0.0;
			for(j=0;j<N;++j)
			{
				t += A[i][j]*x[j];
			}
			b[i] = t;
		}
		t2 = clock();
		T1 = (t2-t1)/(double) CLOCKS_PER_SEC;
		printf("Time of [A]*[x] (without parallelization):%f\n",T1);
		
		t1 = clock();
		// Simplest way to do!
		#pragma omp parallel for private(i,j,t)
		for(i=0;i<N;++i) 
		{
			k = omp_get_thread_num();
			t = 0.0;
			#pragma omp parallel for 
			for(j=0;j<N;++j)
			{
				L = omp_get_thread_num();
				//printf("%d %d\n",k,L);
				t += A[i][j]*x[j];
			}
			c[i] = t;
		}
		t2 = clock();
		T2 = (t2-t1)/(double) CLOCKS_PER_SEC;
		
		for(i=0;i<N;++i)
		{
			//printf("%f\n",fabs(b[i]-c[i]));		// Check correctness
		}
		printf("Time of [A]*[x] (with parallelization):%f\n",T2);
		free(b);
		free(x);
		free(A[0]);
		free(A);	
	} 

	return 0;
} 
