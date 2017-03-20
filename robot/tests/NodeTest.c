#include <criterion/criterion.h>
#include <stdio.h>
#include "Node.h"

static struct Node *node;

Test(Node, creation_destruction)
{
    struct Node *node = Node_new();
    struct Coordinates *zero = Coordinates_zero();

    cr_assert(Coordinates_haveTheSameValues(node->coordinates, zero));
    cr_assert_eq(node->actual_number_of_neighbours, 0);


    Node_delete(node);
    Coordinates_delete(zero);
}

void setup_Node(void)
{
    node = Node_new();
}

void teardown_Node(void)
{
    Node_delete(node);
}

