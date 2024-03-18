#include "libft.h"

int		ft_power(int nbr, int pow)
{
	int	res;

	if (pow == 0)
		return (1);
	res = nbr;
	while (pow > 1)
	{
		res *= nbr;
		pow--;
	}
	return (res);
}
