#include "train.h"
#include <stdio.h>
#include <stdlib.h>

static void print_sub(int *sub, int size)
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

void find_sub(int *arr, int size, int target, int *sub, int sub_size, int i, int sum)
{
	if(i == size)
	{
		if(target == sum && sub_size > 0)
			print_sub(sub, sub_size);
		return;
	}

	find_sub(arr, size, target, sub, sub_size, i + 1, sum);
	sub[sub_size] = arr[i];
	find_sub(arr, size, target, sub, sub_size + 1, i + 1, sum + arr[i]);
}

int main(int ac, char **av)
{
	int size = ac - 2;
	int target = atoi(av[1]);
	int i = 0;
	int *arr = malloc(sizeof(int) * size);
	if(!arr)
		exit(1);
	while(i < size)
	{
		arr[i] = atoi(av[i + 2]);
		i++;
	}
	int *subset = malloc(sizeof(int) * size);
	if(!subset)
	{
		free(arr);
		exit(1);
	}
	find_sub(arr, size, target, subset, 0, 0, 0);
	free(arr);
	free(subset);
	return 0;
}