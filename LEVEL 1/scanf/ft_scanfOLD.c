#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>

/* Consomme 0+ espaces ; -1 uniquement si erreur I/O. */
int match_space(FILE *f)
{
    int c;
    while ((c = fgetc(f)) != EOF)
    {
        if (!isspace((unsigned char)c))
        {
            ungetc(c, f);
            break;
        }
    }
    if (ferror(f))
        return -1;
    return 0;
}

/* Fait correspondre un littéral exact. */
int match_char(FILE *f, char c)
{
    int in = fgetc(f);
    if (in == EOF)
    {
        if (ferror(f)) return -1;
        return 0;
    }
    if (in == (unsigned char)c)
        return 1;
    ungetc(in, f);
    return 0;
}

/* --- Conversions : prennent un pointeur sur va_list --- */

/* %c : lit exactement 1 octet (espaces compris). */
int scan_char(FILE *f, va_list *ap)
{
    int c = fgetc(f);
    if (c == EOF)
    {
        if (ferror(f)) return -1;
        return 0; /* pas de donnée dispo -> non-correspondance ici */
    }
    char *out = va_arg(*ap, char *);
    *out = (char)c;
    return 1;
}

/* %d : entier signé décimal, signe optionnel. */
int scan_int(FILE *f, va_list *ap)
{
    int c = fgetc(f);
    if (c == EOF)
    {
        if (ferror(f)) return -1;
        return 0;
    }

    int sign = 1;
    if (c == '+' || c == '-')
    {
        int signch = c;
        int d = fgetc(f);
        if (d == EOF)
        {
            if (ferror(f)) return -1;
            ungetc(signch, f); /* rien consommé */
            return 0;
        }
        if (!isdigit((unsigned char)d))
        {
            ungetc(d, f);
            ungetc(signch, f); /* rien consommé */
            return 0;
        }
        if (signch == '-')
    		sign = -1;
		else
    		sign = 1;
        long val = d - '0';
        while ((c = fgetc(f)) != EOF && isdigit((unsigned char)c))
            val = val * 10 + (c - '0');
        if (c != EOF)
            ungetc(c, f);

        int *out = va_arg(*ap, int *);
        *out = (int)(sign * val);
        return 1;
    }

    if (!isdigit((unsigned char)c))
    {
        ungetc(c, f);
        return 0;
    }

    long val = c - '0';
    while ((c = fgetc(f)) != EOF && isdigit((unsigned char)c))
        val = val * 10 + (c - '0');
    if (c != EOF)
        ungetc(c, f);

    int *out = va_arg(*ap, int *);
    *out = (int)(sign * val);
    return 1;
}

/* %s : lit une suite de non-espaces dans le buffer fourni (ajoute '\0'). */
int scan_string(FILE *f, va_list *ap)
{
    char *s = va_arg(*ap, char *);
    int c = fgetc(f);
    if (c == EOF)
    {
        if (ferror(f)) return -1;
        return -1; /* EOF == input failure pour %s si aucune assignation */
    }

    int n = 0;
    while (c != EOF && !isspace((unsigned char)c))
    {
        s[n++] = (char)c;
        c = fgetc(f);
    }
    if (c != EOF)
        ungetc(c, f);

    if (n == 0)
        return 0; /* aucune assignation */
    s[n] = '\0';
    return 1;
}

/* Dispatcher des conversions (note: va_list *). */
int match_conv(FILE *f, const char **format, va_list *ap)
{
    switch (**format)
    {
        case 'c':
            return scan_char(f, ap);
        case 'd':
            match_space(f);            /* scanf saute espaces avant %d */
            return scan_int(f, ap);
        case 's':
            match_space(f);            /* scanf saute espaces avant %s */
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

    /* peek : si aucune donnée dispo dès le départ -> EOF */
    int c = fgetc(f);
    if (c == EOF)
        return EOF;
    ungetc(c, f);

    while (*format)
    {
        if (*format == '%')
        {
            format++;
            int r = match_conv(f, &format, &ap); /* passe &ap */
            if (r != 1)
            {
                /* r == 0 : non-correspondance ; r == -1 : input failure/EOF en cours */
                if (r == -1 && nconv == 0)
                    return EOF;         /* règle scanf */
                break;                   /* sinon on s'arrête et renvoie nconv */
            }
            nconv++;
        }
        else if (isspace((unsigned char)*format))
        {
            if (match_space(f) == -1)
            {
                if (nconv == 0) return EOF;
                break;
            }
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
    int ret = ft_vfscanf(stdin, format, ap);
    va_end(ap);
    return ret;
}

int main(void)
{
    int n;
    char c;
    char s[100];

    int ret = ft_scanf("%d %c %s", &n, &c, s);
    printf("ret=%d, n=%d, c='%c', s=\"%s\"\n", ret, n, c, s);
    return 0;
}
