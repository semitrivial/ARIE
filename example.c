/*
 * Simple 3-organism example of the Almost lineaR undIrparEnt algorithm
 *
 * Scenario: "a" gives birth to "b", and later to "c".
 * Find each organism's undirparents.
 * (This is of course a very trivial example.)
 */
#include "arie.h"

int main( void )
{
  /*
   * The organisms
   */
  organism *a, *b, *c;

  /*
   * The linked list from oldest to youngest,
   * as well as pointers for traversing it.
   */
  organism *head = NULL, *tail = NULL, *ptr, *ptr_next;

  /*
   * Create blank organisms
   */
  a = new_organism();
  b = new_organism();
  c = new_organism();

  /*
   * Set names for easier output later
   */
  set_organism_id( a, "A" );
  set_organism_id( b, "B" );
  set_organism_id( c, "C" );

  /*
   * Make 'a' a parent of 'b' and 'c'
   */
  new_edge( a, b );
  new_edge( a, c );

  /*
   * Populate the linked list.  (ARIELINK2 macro since it's
   * doubly-linked.)
   */
  ARIELINK2( a, head, tail, next, prev );
  ARIELINK2( b, head, tail, next, prev );
  ARIELINK2( c, head, tail, next, prev );

  /*
   * Calculate undirparents
   */
  arie_undirparent_calculator( head, tail );

  /*
   * Report undirparents
   */
  for ( ptr = head; ptr; ptr = ptr->next )
  {
    if ( ptr->undirparent )
      printf( "Organism %s has undirparent %s\n", ptr->id, ptr->undirparent->id );
    else
      printf( "Organism %s has no undirparent\n", ptr->id );
  }

  /*
   * Clean up memory
   */
  for ( ptr = head; ptr; ptr = ptr_next )
  {
    ptr_next = ptr->next;

    free_organism( ptr );
  }

  return 1;
}
