#include <criterion/criterion.h>
#include <stdio.h>
#include "CommandSender.h"


/*
 *
Move :
uint_8 *data;
data[0] Commande (move --> 0)
data[1] taille des data envoyés (move --> 8)
data[2..5] float x     Unit = 1 m
data[6..9] float y     Unit = 1 m
struct __attributes__((__packed__)) {
    float x;
    float y;
}
*/
