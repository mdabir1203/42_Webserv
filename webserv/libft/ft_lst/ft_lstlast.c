#include "libft.h"

t_list	*ft_lstlast(t_list *lst)
{
	t_list	*temp;

	temp = NULL;
	if (lst == NULL)
		return (temp);
	else
		temp = lst;
	while (temp->next != NULL)
		temp = temp->next;
	return (temp);
}
