#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void prime(int, int*);


int main()
{
	int * arry;
	int in;
	arry = (int *)malloc(sizeof(int));
	
	printf("Assert prime until: ");
	scanf("%d", &in);
	
	prime(in, arry);
	
	return 0;
};

void prime(int n, int *arr)
{
	int i;
	int k = 1;
	for(i = 0; i<=n; i++) {
		if(i == 1) {
			arr[i] = 2; //mark 1 as special
		}
		else {
			arr[i] = 1; //mark value with 1
		}
	}
	
	int root = sqrt(n);
	while( k <= root)
	{
		if(arr[k] == 1)
		{
			for(int j=k; (k*j)<=n; j++)
			{
				arr[(k*j)] = 0;
			}
		}
		k++;
	}
	printf("\nPrime 1-%d: ", n);
	for(i=2; i<=n; i++) {
			if(arr[i] == 1) {
			printf("%d ", i);
		}
	}
	printf("\n");
}

