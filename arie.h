/*
 * Almost lineaR algorIthm for finding undirparEnts (ARIE)
 *
 * Version Alpha 1
 *
 * A library for calculating undirparents
 *
 * Written by Sam Alexander, 11 Nov 2013
 *
 * arie.h: Data structures etc.  See arie.c for algorithms.
 *
 * For working demonstration, visit:
 *   http://www.semitrivial.com/arie.php
 */

/*
 * Prevent double inclusion of this headerfile
 */
#ifndef ARIE_INCLUDED
#define ARIE_INCLUDED

#include <stdlib.h>
#include <stdio.h>

/*
 * Data structure abbreviations
 */
typedef struct ORGANISM organism;
typedef struct CHILD_DATUM child_datum;
typedef struct CONNECTED_COMPONENT connected_component;


/*
 * Data structures
 */
struct ORGANISM
{
  organism *next;
  organism *prev;

  /*
   * Because organisms can have half-siblings,
   * child data is more complicated than a naive "next_sibling, prev_sibling" linked list.
   * Thus each organism has its own separate linked list of children (and these separate
   * linked lists may overlap, necessitating we actually deal in linked lists of "shells"
   * that simply point to the intended children, rather than a linked list directly of
   * children themselves).
   */
  child_datum *first_child;
  child_datum *last_child;

  /*
   * Undirparent is initiated with value NULL.
   * The algorithm will assign it its proper value.
   */
  organism *undirparent;

  /*
   * The component field is used for internal calculations by
   * the library.  Library users do not need to worry about it.
   * During the undirparent calculations, component is used to
   * store a link from the organism to a connected component.
   */
  connected_component *component;
};

struct CHILD_DATUM
{
  child_datum *next;
  child_datum *prev;
  organism *child;
};

struct CONNECTED_COMPONENT
{
  connected_component *next;
  connected_component *prev;
  /*
   * The "orphan" of a connected component is its oldest member.
   */
  organism *orphan;
  /*
   * The "backpointer" of a component (if not self-pointing) points to a bigger component
   * into which the component has been merged.
   */
  connected_component *backpointer;
  /*
   * Estimate of length of longest path in the component
   */
  int depth;
};

/*
 * Global function prototypes
 * (functions intended to be used by users of this library)
 * See arie.c for local function prototypes
 * (functions not intended to be used by users of the library)
 */
void arie_undirparent_calculator( organism *oldest, organism *youngest );
organism *new_organism( void );
void new_edge( organism *parent, organism *child );
void free_organism( organism *o );

/*
 * Memory allocation macro
 */
#define ARIECREATE(result, type, number) 						\
do                                                                                      \
{                                                                                       \
    if (!((result) = (type *) calloc ((number), sizeof(type)))) 			\
    {                                                                                   \
        perror("malloc failure");                                               	\
        fprintf(stderr, "Malloc failure @ %s:%d\n", __FILE__, __LINE__ ); 		\
        abort();                                                                        \
    }                                                                                   \
} while(0)

/*
 * Doubly linked list macros
 */
#define ARIELINK(link, first, last, next, prev)                         \
do                                                                      \
{                                                                       \
   if ( !(first) )                                                      \
   {                                                                    \
      (first) = (link);                                                 \
      (last) = (link);                                                  \
   }                                                                    \
   else                                                                 \
      (last)->next = (link);                                            \
   (link)->next = NULL;                                                 \
   if (first == link)                                                   \
      (link)->prev = NULL;                                              \
   else                                                                 \
      (link)->prev = (last);                                            \
   (last) = (link);                                                     \
} while(0)

#define ARIEUNLINK(link, first, last, next, prev)                       \
do                                                                      \
{                                                                       \
        if ( !(link)->prev )                                            \
        {                                                               \
         (first) = (link)->next;                                        \
           if ((first))                                                 \
              (first)->prev = NULL;                                     \
        }                                                               \
        else                                                            \
        {                                                               \
         (link)->prev->next = (link)->next;                             \
        }                                                               \
        if ( !(link)->next )                                            \
        {                                                               \
         (last) = (link)->prev;                                         \
           if ((last))                                                  \
              (last)->next = NULL;                                      \
        }                                                               \
        else                                                            \
        {                                                               \
         (link)->next->prev = (link)->prev;                             \
        }                                                               \
} while(0)

/*
 * Match #ifndef ARIE_INCLUDED
 */
#endif
