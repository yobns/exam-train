#include "trainer.h"
#include <stdlib.h>
#include <stdio.h>

static void print_sum(int *sub, int size)
{
	int i = 0;
	while(i < size)
	{
		printf("%d", sub[i]);
		if(i + 1 < size)
			printf(" ");
		i++;
	}
	printf("\n");
}

void powerset(int *arr, int size, int target, int *sub, int sub_size, int i, int sum)
{
	if(i == size)
	{
		if(sum == target && sub_size > 0)
			print_sum(sub, sub_size);
		return;
	}
	powerset(arr, size, target, sub, sub_size, i + 1, sum);

	sub[sub_size] = arr[i];
	powerset(arr, size, target, sub, sub_size + 1, i + 1, sum + arr[i]);
}

int main(int ac, char **av)
{
	int size = ac - 2;
	int i = 0;
	int target = atoi(av[1]);
	int *arr = malloc(sizeof(int) * size);
	if(!arr)
		return 1;
	while(i < size)
	{
		arr[i] = atoi(av[i + 2]);
		i++;
	}
	int *sub = malloc(sizeof(int) * size);
	if(!sub)
	{
		free(arr);
		return 1;
	}
	powerset(arr, size, target, sub, 0, 0, 0);
	free(arr);
	free(sub);
	return 0;
}