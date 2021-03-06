#include <criterion/criterion.h>
#include <stdio.h>
#include "Node.h"

struct Node *node_a;
struct Node *node_b;
struct Node *node_c;
struct Node *node_d;
struct Node *node_e;
struct Node *node_f;
struct Node *node_g;

Test(Node, creation_destruction)
{
    struct Node *node = Node_new();
    struct Coordinates *zero = Coordinates_zero();

    cr_assert(Coordinates_haveTheSameValues(node->coordinates, zero));
    cr_assert_eq(node->actual_number_of_neighbours, 0);
    cr_assert_eq(node->neighbours[0], node);
    cr_assert_eq(node->neighbours[1], node);
    cr_assert_eq(node->neighbours[2], node);
    cr_assert_eq(node->neighbours[3], node);
    cr_assert_eq(node->distance_to_neighbours[0], 0);
    cr_assert_eq(node->distance_to_neighbours[1], 0);
    cr_assert_eq(node->distance_to_neighbours[2], 0);
    cr_assert_eq(node->distance_to_neighbours[3], 0);


    Node_delete(node);
    Coordinates_delete(zero);
}

void setup_Node(void)
{
    node_a = Node_new();
    node_b = Node_new();
    node_c = Node_new();
    node_d = Node_new();
    node_e = Node_new();
    node_f = Node_new();
    node_g = Node_new();
    node_b->coordinates->x = 3;
    node_b->coordinates->y = 4;
    node_c->coordinates->x = 6;
    node_c->coordinates->y = 8;
    node_d->coordinates->x = 9;
    node_d->coordinates->y = 12;
    node_e->coordinates->x = 12;
    node_e->coordinates->y = 16;
    node_f->coordinates->x = 15;
    node_f->coordinates->y = 20;
    node_g->coordinates->x = 18;
    node_g->coordinates->y = 24;
}

void teardown_Node(void)
{
    Node_delete(node_a);
    Node_delete(node_b);
    Node_delete(node_c);
    Node_delete(node_d);
    Node_delete(node_e);
    Node_delete(node_f);
    Node_delete(node_g);
}

Test(Node, given_twoNodesWithNoNeighbours_when_attemptsToConnectNeighbours_then_theTwoNodesAreEachOthersNeighbours
     , .init = setup_Node
     , .fini = teardown_Node)
{
    Node_attemptToConnectAsNeighbours(node_a, node_b);

    cr_assert_eq(node_a->neighbours[0], node_b);
    cr_assert_eq(node_b->neighbours[0], node_a);
}

Test(Node, given_twoNodesWithNoNeighbours_when_attemptsToConnectNeighbours_then_eachNodeNowHasOneActualNeighbour
     , .init = setup_Node
     , .fini = teardown_Node)
{
    Node_attemptToConnectAsNeighbours(node_a, node_b);

    cr_assert_eq(node_a->actual_number_of_neighbours, 1);
    cr_assert_eq(node_b->actual_number_of_neighbours, 1);
}

Test(Node,
     given_twoNodesWithAlreadyNeighbours_when_attemptsToConnectNeighbours_then_theyDoNotBecomeNeighboursASecondTime
     , .init = setup_Node
     , .fini = teardown_Node)
{
    Node_attemptToConnectAsNeighbours(node_a, node_b);
    Node_attemptToConnectAsNeighbours(node_a, node_b);

    cr_assert_neq(node_a->neighbours[1], node_b);
    cr_assert_neq(node_b->neighbours[1], node_a);
    cr_assert_eq(node_a->actual_number_of_neighbours, 1);
    cr_assert_eq(node_b->actual_number_of_neighbours, 1);
}

Test(Node,
     given_aNodeWithMaxNumberOfActualNeighbour_when_attempsToConnectToANewNeighbour_then_theNewNeighbourIsntAdded
     , .init = setup_Node
     , .fini = teardown_Node)
{
    Node_attemptToConnectAsNeighbours(node_a, node_b);
    Node_attemptToConnectAsNeighbours(node_a, node_c);
    Node_attemptToConnectAsNeighbours(node_a, node_d);
    Node_attemptToConnectAsNeighbours(node_a, node_e);
    Node_attemptToConnectAsNeighbours(node_a, node_f);
    Node_attemptToConnectAsNeighbours(node_a, node_g);

    cr_assert_eq(node_a->actual_number_of_neighbours, MAX_NUMBER_OF_NEIGHBOURS);
}

Test(Node, given_twoNodesWithZeroDistanceBetweenThem_when_attempsToAConnectNeighbours_then_theyAreNotAddedAsNeighbours
     , .init = setup_Node
     , .fini = teardown_Node)
{
    struct Node *node_with_the_same_coordinates_as_a = Node_new();
    Node_attemptToConnectAsNeighbours(node_a, node_with_the_same_coordinates_as_a);

    cr_assert_eq(node_a->actual_number_of_neighbours, 0);

    Node_delete(node_with_the_same_coordinates_as_a);
}

Test(Node, given_aNodeWithNeighbours_when__then_itHasTheCorrectDistanceToItsNeighbours
     , .init = setup_Node
     , .fini = teardown_Node)
{
    Node_attemptToConnectAsNeighbours(node_a, node_b);
    Node_attemptToConnectAsNeighbours(node_a, node_c);
    Node_attemptToConnectAsNeighbours(node_a, node_d);
    Node_attemptToConnectAsNeighbours(node_a, node_e);

    cr_assert_eq((int)(node_a->distance_to_neighbours[0]), 5);
    cr_assert_eq((int)(node_b->distance_to_neighbours[0]), 5);
    cr_assert_eq((int)(node_a->distance_to_neighbours[1]), 10);
    cr_assert_eq((int)(node_c->distance_to_neighbours[0]), 10);
    cr_assert_eq((int)(node_a->distance_to_neighbours[2]), 15);
    cr_assert_eq((int)(node_d->distance_to_neighbours[0]), 15);
    cr_assert_eq((int)(node_a->distance_to_neighbours[3]), 20);
    cr_assert_eq((int)(node_e->distance_to_neighbours[0]), 20);
}

Test(Node, given_twoNeighbourNodes_when_askedToReturnTheirConnectingDistance_then_itIsReturned
     , .init = setup_Node
     , .fini = teardown_Node)
{
    Node_attemptToConnectAsNeighbours(node_a, node_b);
    float distance = Node_distanceToNeighbour(node_a, node_b);
    cr_assert_float_eq(distance, node_a->distance_to_neighbours[0], 0.1);
}

Test(Node, given_twoNotNeighbourNodes_when_askedToReturnTheirConnectingDistance_then_InfinityIsReturned
     , .init = setup_Node
     , .fini = teardown_Node)
{
    float distance = Node_distanceToNeighbour(node_a, node_b);
    cr_assert_float_eq(distance, MAX_VALUE_OF_FLOAT, 0.1);
}

Test(Node, given_twoNeighbourNodes_when_askedIfTheyAreNeighbours_then_theyAre
     , .init = setup_Node
     , .fini = teardown_Node)
{
    Node_attemptToConnectAsNeighbours(node_a, node_b);
    int connected = Node_areNeighbours(node_a, node_b);
    cr_assert(connected);
}

Test(Node, given_twoNotNeighbourNodes_when_askedIfTheyAreNeighbours_then_theyAreNot
     , .init = setup_Node
     , .fini = teardown_Node)
{
    int connected = Node_areNeighbours(node_a, node_b);
    cr_assert(!connected);
}

