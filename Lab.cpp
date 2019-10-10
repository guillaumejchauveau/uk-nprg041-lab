#include <cstdio>
#include <cstdlib>
#include "list.h"
#include "bub.h"

void printer (const struct list *list)
{
  printf ("The value of this node is: %d!", list->val);
}

void printList (struct list *list)
{
  for (; list; list = list->next)
    {
      printf ("value: %d!\n", list->val);
    }
}

void pushList (int val, struct list **list)
{
  auto L = (struct list *) malloc (sizeof (struct list));
  L->val = val;
  L->next = *list;
  *list = L;
}

void popList (struct list **L)
{
  struct list *tmp = *L;
  *L = (*L)->next;
  free (tmp);
}

void freeList (struct list **L)
{
  while (*L)
    {
      popList (L);
    }
}

int main ()
{
  struct list *ll = nullptr;
  pushList (1, &ll);
  pushList (2, &ll);
  pushList (6, &ll);
  bubblesort (ll);
  printList (ll);
  popList (&ll);
  printList (ll);
  freeList (&ll);
  printList (ll);
  return 0;
}
