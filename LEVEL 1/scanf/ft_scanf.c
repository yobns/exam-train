#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>

static va_list *g_ap = NULL;

int match_space(FILE *f)
{
    int c;
	while((c = fgetc(f)) != EOF && isspace(c))
		;
	if(c != EOF)
		ungetc(c, f);
    return (1);
}

int match_char(FILE *f, char c)
{
    int got = fgetc(f);
	if(got == EOF)
		return -1;
	if(got == c)
		return 1;
	ungetc(got, f);
    return (0);
}

int scan_char(FILE *f, va_list ap)
{
	(void)ap;
    int c = fgetc(f);
	if(c == EOF)
		return -1;
	char *dest = va_arg(*g_ap, char *);
	*dest = (char)c;
    return (1);
}

int scan_int(FILE *f, va_list ap)
{
	(void)ap;
    int sign = 1;
	int val = 0;
	int c = fgetc(f);
	if(c == EOF)
		return -1;
	if(c == '+' || c == '-')
	{
		int s = c;
		int next = fgetc(f);
		if(next == EOF)
		{
			ungetc(s, f);
			return 0;
		}
		if(!isdigit(next))
		{
			ungetc(next, f);
			ungetc(s, f);
			return 0;
		}
		if(s == '-')
			sign = -1;
		c = next;
	}
	if(!isdigit(c))
	{
		ungetc(c, f);
		return 0;
	}
	while(c != EOF && isdigit(c))
	{
		val = val * 10 + (c - '0');
		c = fgetc(f);
	}
	if(c != EOF)
		ungetc(c, f);
	int *dest = va_arg(*g_ap, int *);
	*dest = (int)(val * sign);
    return 1;
}

int scan_string(FILE *f, va_list ap)
{
	(void)ap;
    int c = fgetc(f);
	if(c == EOF)
		return -1;
	if(isspace(c))
	{
		ungetc(c, f);
		return 0;
	}
	char *dest = va_arg(*g_ap, char *);
	char *p = dest;
	while(c != EOF && !isspace(c))
	{
		*p++ = (char)c;
		c = fgetc(f);
	}
	if(c != EOF)
		ungetc(c, f);
	*p = '\0';
    return (1);
}


int	match_conv(FILE *f, const char **format, va_list ap)
{
	switch (**format)
	{
		case 'c':
			return scan_char(f, ap);
		case 'd':
			match_space(f);
			return scan_int(f, ap);
		case 's':
			match_space(f);
			return scan_string(f, ap);
		case EOF:
			return -1;
		default:
			return -1;
	}
}

int ft_vfscanf(FILE *f, const char *format, va_list ap)
{
	int nconv = 0;

	int c = fgetc(f);
	if (c == EOF)
		return EOF;
	ungetc(c, f);

	while (*format)
	{
		if (*format == '%')
		{
			format++;
			if (match_conv(f, &format, ap) != 1)
				break;
			else
				nconv++;
		}
		else if (isspace(*format))
		{
			if (match_space(f) == -1)
				break;
		}
		else if (match_char(f, *format) != 1)
			break;
		format++;
	}
	
	if (ferror(f))
		return EOF;
	return nconv;
}


int ft_scanf(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	g_ap = &ap;
	int ret = ft_vfscanf(stdin, format, ap);
	g_ap = NULL;
	va_end(ap);
	if(ret == 0 && feof(stdin))
		return EOF;
	return ret;
}

int main(void)
{
	char c;
	int n;
	char s[10];
	ft_scanf("%c %d %s", &c, &n, s);
	printf("%c %d %s", c, n, s);
}