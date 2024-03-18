#include "libft.h"

int		ft_baselen(long n, int base)
{
	int		size;

	size = 0;
	if (base < 2)
		return (0);
	if (n == 0)
		return (1);
	if (n < 0)
		size++;
	while (n != 0)
	{
		n = n / base;
		size++;
	}
	return (size);
}
