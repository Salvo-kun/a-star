#ifndef _DOMAIN
#define _DOMAIN

// Data structures declaration
enum Domain
{
  GRID_2D
};

// Function prototypes

/*
    This function returns the reading function of a specific domain.
    If the specified domain does not exist, a NULL pointer is returned.
*/
extern void *(*choose_domain_reader(enum Domain d))(char *, int *);

#endif
