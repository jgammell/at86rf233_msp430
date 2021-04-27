/*
 * assert_app.c
 *
 *  Created on: Apr 27, 2021
 *      Author: jgamm
 */

#include "assert_app.h"


void assert(bool condition)
{
    if(!condition)
        while(1);
}
