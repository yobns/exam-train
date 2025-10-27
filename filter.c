#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int ft_strncmp(char *s1, char *s2, int n)
{
	int i = 0;
	while(i < n && s1[i] && s2[i])
	{
		if(s1[i] != s2[i])
			return((unsigned char)s1[i] - (unsigned char)s2[i]);
		i++;
	}
	if(i < n)
		return((unsigned char)s1[i] - (unsigned char)s2[i]);
	return 0;
}
char *read_input(int fd)
{
	ssize_t bytes;
	int i = 0;
	char c;
	char *buff = malloc(100000);
	if(!buff)
	{
		perror("error");
		return NULL;
	}
	while((bytes = read(fd, &c, 1)) > 0)
	{
		buff[i] = c;
		i++;
	}
	if(bytes == -1)
	{
		perror("error");
		free(buff);
		return NULL;
	}
	buff[i] = '\0';
	return buff;
}

void process(char *buff, char *str, int len)
{
	int i = 0;
	int y;
	while(buff[i])
	{
		if(ft_strncmp(&buff[i], str, len) == 0)
		{
			y = 0;
			while(y < len)
			{
				printf("*");
				y++;
			}
			i = i + len;
		}
		else
		{
			printf("%c", buff[i]);
			i++;
		}
	}
}

int main(int ac, char **av)
{
	char *buff;
	if(ac != 2 || strlen(av[1]) == 0)
		return 1;
	buff = read_input(STDIN_FILENO);
	if(!buff)
		return 1;
	process(buff, av[1], strlen(av[1]));
	free(buff);
	return 0;
}
