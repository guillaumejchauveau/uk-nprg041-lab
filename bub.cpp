#include "bub.h"

void swap (int *a, int *b)
{
  int c = *a;
  *a = *b;
  *b = c;
}

void bubblesort (struct list *L)
{
  struct list *i, *j;
  for (i = L; i; i = i->next)
    {
      for (j = L; j; j = j->next)
        {
          if (j->next && j->val > j->next->val)
            {
              swap (&j->val, &j->next->val);
            }
        }
    }
}

