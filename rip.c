#include <unistd.h>
#include <stdlib.h>

void rip(char *s, int open, int close, char *out, int i, int balance)
{
	if(s[i] == '\0')
	{
		if(balance == 0 && open == 0 && close == 0)
		{
			if(i > 0)
				write(1, out, i);
			write(1, "\n", 1);
		}
		return;
	}
	if(s[i] == '(')
	{
		out[i] = s[i];
		rip(s, open, close, out, i + 1, balance + 1);
		if(open > 0)
		{
			out[i] = ' ';
			rip(s, open - 1, close, out, i + 1, balance);
		}
	}
	else
	{
		if(balance > 0)
		{
			out[i] = s[i];
			rip(s, open, close, out, i + 1, balance - 1);
		}
		if(close > 0)
		{
			out[i] = ' ';
			rip(s, open, close - 1, out, i + 1, balance);
		}
	}
}

int main(int ac, char **av)
{
	int i = 0;
	int open = 0;
	int close = 0;

	if(ac != 2)
	{
		write(1, "\n", 1);
		return 0;
	}
	char *input = av[1];
	while(input[i])
	{
		if(input[i] != '(' || input[i] != ')')
		{
			write(1, "\n", 1);
			return 0;
		}
		if(input[i] == '(')
			open++;
		else
		{
			if(open > 0)
				open--;
			else
				close++;
		}
		i++;
	}
	char out[i];
	rip(input, open, close, out, 0, 0);
	return 0;
}