#include "libft.h"

void	ft_skip_char(const char *str, int *i, char c)
{
	while (str[*i] == c)
		(*i)++;
}
