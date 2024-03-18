#include "libft.h"

void	ft_lstadd_front(t_list **alst, t_list *newt)
{
	if (!alst || !newt)
		return ;
	newt->next = *alst;
	*alst = newt;
}
