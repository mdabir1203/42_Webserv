#include "libft.h"

void	ft_lstadd_back(t_list **alst, t_list *newt)
{
	t_list	*temp;

	if (!alst)
		return ;
	if (*alst != NULL)
	{
		temp = *alst;
		while (temp->next != NULL)
			temp = temp->next;
		temp->next = newt;
	}
	else
		*alst = newt;
}
