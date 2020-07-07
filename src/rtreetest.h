#ifndef RTREETEST_H
#define RTREETEST_H

#include "crtree.h"
#include "cnotrtree.h"

#include <iostream>
#include <stdexcept>
#include <ctime>
#include <cstdio>
#include <random>
#include <cmath>

using namespace std;

#define MIN_CHILD_NODES 2
#define MAX_CHILD_NODES 50
#define CACHE_SIZE 4096
#define MAX_ERASED 3

// n colinear vectors : { (x, x, x) | x in [0, n) }
// if k != 0 knn is done, otherwise normal search
pair<unsigned, unsigned> rTreeTestA( const unsigned dim, const unsigned n, const unsigned k );

// n random hyperrectangles of "dim" dimension with values from [-1000, 1000)
// if k != 0 knn is done, otherwise normal search
pair<unsigned, unsigned> rTreeTestB( const unsigned dim, const unsigned n, const unsigned k );

#endif // RTREETEST_H
