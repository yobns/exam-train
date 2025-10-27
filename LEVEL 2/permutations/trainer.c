#include <unistd.h>
#include <stdlib.h>

void printer(char *str, int size)
{
	write(1, str, size);
	write(1, "\n", 1);
}
void sort_arr(char *str, int size)
{
	int i = 0;
	int j;
	char c;
	while(i < size - 1)
	{
		j = 0;
		while(j < size - i - 1)
		{
			if(str[j] > str[j + 1])
			{
				c = str[j];
				str[j] = str[j + 1];
				str[j + 1] = c;
			}
			j++;
		}
		i++;
	}
}
void permutation(char *str, char* temp, int *used, int size, int pos)
{
	int i = 0;
	if(pos == size)
		return(printer(temp, size));
	while(i < size)
	{
		if(!used)
		{
			used[i] = 1;
			temp[pos] = str[i];
			permutation(str, temp, used, size, pos + 1);
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
	char *str = malloc(sizeof(char) * size);
	char *temp = malloc(sizeof(char) * size);
	char *used = calloc(size, sizeof(int));
	if(!str || !temp || !used)
		return 1;
	while(i < size)
	{
		str[i] = av[1][i];
		i++;
	}
	sort_arr(str, size);
	permutation(str, temp, used, size, 0);
	free(str);
	free(temp);
	free(used);
	return 0;
}