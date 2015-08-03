/*
 * Almost lineaR algorIthm for finding undirparEnts (ARIE)
 *
 * Version Alpha 1
 *
 * A library for calculating undirparents
 *
 * Written by Sam Alexander, 11 Nov 2013
 *
 * arie.c: The algorithm.  See arie.h for data structures etc.
 *
 * For working demonstration, visit:
 *   http://www.semitrivial.com/arie.php
 */

#include <string.h>
#include "arie.h"

/*
 * Global variables within arie.c scope
 * Or maybe we should call them global arieables?
 */
organism *oldest_organism;
organism *youngest_organism;
connected_component *first_component;
connected_component *last_component;

/*
 * Local function prototypes
 * (functions that are opaque to users of the library)
 * See arie.h for global function prototypes
 * (functions that are intended for usage by people using this library)
 */
void initialize_data( void );
void calculate_undirparents( void );
connected_component *find_component( organism *o );
connected_component *component_union( connected_component *n, connected_component *m );
connected_component *create_singleton_component( organism *o );
void clean_up_memory( void );

/*
 * Global Functions
 * (Intended to be used by users of the library)
 */
void arie_undirparent_calculator( organism *oldest, organism *youngest )
{
  /*
   * arie_undirparent_calculator is the main bridge between the arie library and the
   * outside program where it's included.
   * It should be called with pointers to the head and tail of a doubly linked list
   * of "organism" structures, used to specify a genealogical network,
   * with oldest organisms first, followed by younger organisms.
   * The "organism" data structure is defined in arie.h.
   * After this function is called, the organisms in the linked list will
   * have their "undirparent" fields set to the correct undirparent organisms
   * (or NULL for roots).
   */
   organism *o;

   if ( !oldest || !youngest )
     return;

   oldest_organism = oldest;
   youngest_organism = youngest;

  /*
   * Set undirparent data to NULL for all nodes.
   */
  for ( o = oldest_organism; o; o = o->next )
    o->undirparent = NULL;

  /*
   * Initialize the list of connected components empty.
   */
  first_component = NULL;
  last_component = NULL;

  /*
   * Calculate the undirparents!
   */
  calculate_undirparents( );

  /*
   * Clean up memory.  Note, the organisms themselves are not
   * erased, it's up to the library user to take care of that memory.
   */
  clean_up_memory( );
}

organism *new_organism( void )
{
  /*
   * Call this function to obtain a pointer to a blank organism structure.
   */
  organism *o;

  ARIECREATE( o, organism, 1 );
  o->next = NULL;
  o->prev = NULL;
  o->first_child = NULL;
  o->last_child = NULL;
  o->undirparent = NULL;
  o->component = NULL;
  o->id = NULL;

  return o;
}

void set_organism_id( organism *o, const char *id )
{
  if ( o->id )
    free( o->id );

  o->id = strdup( id );
}

void new_edge( organism *parent, organism *child )
{
  /*
   * Call this function to specify that "child" is a child of "parent".
   * Warning: It's entirely left up to the library user to ensure
   * parents are older than children!  If not, expect undefined behavior.
   */
  child_datum *edge;

  ARIECREATE( edge, child_datum, 1 );
  edge->child = child;

  ARIELINK( edge, parent->first_child, parent->last_child, next );
}

void free_organism( organism *o )
{
  /*
   * Call this function to de-allocate all memory allocated for an organism.
   * All subsequent uses of the pointer will be undefined behavior.
   */
  child_datum *edge, *edge_next;

  for ( edge = o->first_child; edge; edge = edge_next )
  {
    edge_next = edge->next;
    free( edge );
  }

  if ( o->id )
    free( o->id );

  free( o );
}

/*
 * The remainder of the arie.c consists of local functions
 * (functions not intended for direct usage by library users)
 */

/*
 * Arie's main "Loop": calculate undirparents.
 */
void calculate_undirparents( void )
{
  organism *i;
  connected_component *C0;
  child_datum *edge;
  organism *cp;
  connected_component *Cp;

  /*
   * Superfluous "goto" statement in honor of Arie's research on "goto"
   */
  goto beginning_of_calculate_undirparents;

beginning_of_calculate_undirparents:

  /*
   * Let i go from the youngest organism to the oldest.
   */
  for ( i = youngest_organism; i; i = i->prev )
  {
    /*
     * Create a component consisting only of node i,
     * having i as orphan.  Note: the function
     * create_singleton_component automatically makes
     * the single member the orphan.
     */
    C0 = create_singleton_component(i);

    /*
     * Iterate through the children of i.
     * (If i has no children, this for loop does nothing.)
     */
    for ( edge = i->first_child; edge; edge = edge->next )
    {
      cp = edge->child;
      /*
       * Find which component the child is in.
       */
      Cp = find_component(cp);

      /*
       * If i has multiple children in a component,
       * we must be careful only to "process" that component
       * once.  If we've already processed it, then its
       * orphan is i.  If so, go on to the next child of i.
       */
      if ( Cp->orphan == i )
        continue;

      /*
       * The undirparent of the component's previous orphan shall now be i.
       */
      Cp->orphan->undirparent = i;

      /*
       * Unite the components.
       */
      C0 = component_union(C0,Cp);
      C0->orphan = i;
    }
  }
}

connected_component *find_component( organism *o )
{
  connected_component *pointer;
  connected_component *aux;
  connected_component *destination;

  /*
   * It may be that o already points to the correct component.
   */
  if ( o->component->backpointer == o->component )
    return o->component;

  /*
   * If not, follow the pointers til we get the correct component.
   */
  for ( pointer = o->component->backpointer; pointer->backpointer != pointer; pointer = pointer->backpointer )
    ;
  destination = pointer;

  /*
   * Now re-traverse those same pointers, flattening them so they all point to the correct destination.
   */
  for ( pointer = o->component; pointer != destination; pointer = aux )
  {
    aux = pointer->backpointer;
    pointer->backpointer = destination;
  }

  return destination;
}

connected_component *component_union( connected_component *n, connected_component *m )
{
  /*
   * Return the longer component, and set the backpointer of the shorter
   * component to unite the two.  If same length, increment length of returnee.
   */
  if ( n->depth > m->depth )
  {
    m->backpointer = n;
    return n;
  }

  n->backpointer = m;

  if ( n->depth == m->depth )
    m->depth++;

  return m;
}

connected_component *create_singleton_component( organism *o )
{
  connected_component *c;

  ARIECREATE( c, connected_component, 1 );

  c->orphan = o;
  c->backpointer = c;
  c->depth = 1;

  ARIELINK( c, first_component, last_component, next );

  o->component = c;

  return c;
}

void clean_up_memory( void )
{
  organism *o;
  connected_component *c, *c_next;

  /*
   * De-allocate connected component memory
   */
  for ( c = first_component; c; c = c_next )
  {
    c_next = c->next;
    free( c );
  }

  /*
   * Snip obsolete pointers to connected components
   */
  for ( o = oldest_organism; o; o = o->next )
    o->component = NULL;
}
