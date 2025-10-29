#include <stdlib.h>
#include <stdio.h>

void solve(int *rows, int n, int col)
{
	int i = 0;

	if(n == col)
	{
		while(i < n)
		{
			if(i)
				fprintf(stdout, " ");
			fprintf(stdout, "%d", rows[i]);
			i++;
		}
		fprintf(stdout, "\n");
		return;
	}
	while(i < n)
	{
		int safe = 1;
		int prev_col = 0;
		while(prev_col < col)
		{
			int diag = rows[prev_col] - i;
			if(diag < 0)
				diag = -diag;
			if(rows[prev_col] == i || diag == col - prev_col)
			{
				safe = 0;
				break;
			}
			prev_col++;
		}
		if(safe)
		{
			rows[col] = i;
			solve(rows, n, col + 1);
		}
		i++;
	}
}
int main(int ac, char **av)
{
	if(ac != 2)
		return 0;
	int n = atoi(av[1]);
	if(n <= 0)
		return 0;
	int rows[n];
	solve(rows, n, 0);
	return 0;
}