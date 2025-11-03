#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void solve(int *queens, int n, int col)
{
	int l = 0;
	if(col == n)
	{
		while(l < n)
		{
			if(l)
				fprintf(stdout, " ");
			fprintf(stdout, "%d", queens[l]);
			l++;
		}
		fprintf(stdout, "\n");
		return;
	}
	while(l < n)
	{
		int prev_col = 0;
		int safe = 1;
		while(prev_col < col)
		{
			int diag = queens[prev_col] - l;
			if(diag < 0)
				diag = -diag;
			if(queens[prev_col] == l || diag == col - prev_col)
			{
				safe = 0;
				break;
			}
			prev_col++;
		}
		if(safe)
		{
			queens[col] = l;
			solve(queens, n, col + 1);
		}
		l++;
	}
}
int main(int ac, char **av)
{
	if(ac != 2)
		return 1;
	int n = atoi(av[1]);
	if(n <= 0)
		return 1;
	int queens[n];
	solve(queens, n, 0);
	return 0;
}