#include "train.h"
#include <stdlib.h>
#include <unistd.h>

void sort_arr(char *str, int size)
{
	int i = 0;
	int j;
	char temp;
	while(i < size - 1)
	{
		j = 0;
		while(j < size - i - 1)
		{
			if(str[j] > str[j + 1])
			{
				temp = str[j];
				str[j] = str[j + 1];
				str[j + 1] = temp;
			}
			j++;
		}
		i++;
	}
}

void permute(char *arr, char *temp, char *used, int size, int pos)
{
	int i = 0;
	if(pos == size)
	{
		write(1, temp, size);
		write(1, "\n", 1);
		return;
	}
	while(i < size)
	{
		if(!used[i])
		{
			used[i] = 1;
			temp[pos] = arr[i];
			permute(arr, temp, used, size, pos + 1);
			used[i] = 0;
		}
		i++;
	}
}

int main(int ac, char **av)
{
	int i = 0;
	int size = 0;
	if(ac != 2)
	{
		write(1, "\n", 1);
		return 0;
	}
	while(av[1][size])
		size++;
	if(size == 0)
	{
		write(1, "\n", 1);
		return 0;
	}
	char *arr = malloc(sizeof(char) * size);
	char *temp = malloc(sizeof(char) * size);
	char *used = calloc(size, sizeof(int));
	if(!arr || !temp || !used)
		return 1;
	while(i < size)
	{
		arr[i] = av[1][i];
		i++;
	}
	sort_arr(arr, size);
	permute(arr, temp, used, size, 0);
	free(arr);
	free(temp);
	free(used);
	return 0;
}