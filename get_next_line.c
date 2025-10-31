#include "get_next_line.h"

char *ft_strchr(char *s, int c)
{
	int i = 0;
	while (s[i] && s[i] != (char)c)
		i++;
	if (s[i] == (char)c)
		return s + i;
	else
		return NULL;
}

void *ft_memcpy(void *dest, const void *src, size_t n)
{
	while (n--)
		((char *)dest)[n] = ((const char *)src)[n];
	return dest;
}

size_t ft_strlen(char *s)
{
	size_t res = 0;
	if (!s)
		return 0;
	while (*s)
	{
		s++;
		res++;
	}
	return res;
}

int str_append_mem(char **s1, char *s2, size_t size2)
{
	size_t size1 = ft_strlen(*s1);
	char *tmp = malloc(size2 + size1 + 1);
	if (!tmp)
		return 0;
	if (size1)
		ft_memcpy(tmp, *s1, size1);
	if (size2)
		ft_memcpy(tmp + size1, s2, size2);
	tmp[size1 + size2] = '\0';
	free(*s1);
	*s1 = tmp;
	return 1;
}

int str_append_str(char **s1, char *s2)
{
	return str_append_mem(s1, s2, ft_strlen(s2));
}

void *ft_memmove(void *dest, const void *src, size_t n)
{
	size_t i;

	if (dest == src || n == 0)
		return dest;
	if (dest < src)
	{
		i = 0;
		while (i < n)
		{
			((char *)dest)[i] = ((const char *)src)[i];
			i++;
		}
	}
	else
	{
		i = n;
		while (i-- > 0)
			((char *)dest)[i] = ((const char *)src)[i];
	}
	return dest;
}

char *get_next_line(int fd)
{
	static char b[BUFFER_SIZE + 1] = "";
	char *ret = NULL;
	char *tmp = ft_strchr(b, '\n');
	while (!tmp)
	{
		if (!str_append_str(&ret, b))
			return NULL;
		int read_ret = read(fd, b, BUFFER_SIZE);
		if (read_ret == -1)
		{
			free(ret);
			return (NULL);
		}
		if (read_ret == 0)
		{
			if (ret && ret[0])
			{
				b[0] = '\0';
				return ret;
			}
			free(ret);
			return NULL;
		}
		b[read_ret] = 0;
		tmp = ft_strchr(b, '\n');
	}
	if (!str_append_mem(&ret, b, (size_t)(tmp - b + 1)))
	{
		free(ret);
		return NULL;
	}
	ft_memmove(b, tmp + 1, ft_strlen(tmp + 1) + 1);
	return ret;
}
int main(void)
{
	char *s;
	int i = 0;
	while ((s = get_next_line(0)))
	{
		while (s[i])
			i++;
		write(1, s, i);
		free(s);
	}
}