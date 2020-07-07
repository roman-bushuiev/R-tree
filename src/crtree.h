#ifndef CRTREE_H
#define CRTREE_H

#include "chyperrectangle.h"
#include "cnode.h"

#include <list>
#include <array>
#include <vector>
#include <string>
#include <memory>
#include <limits>
#include <float.h>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <queue>
#include <stack>
#include <tuple>
#include <stdexcept>
#include <algorithm>
#include <iostream>

using namespace std;

class CompareMindist
{
public:
    CompareMindist( const vector<double> & point );

    bool operator() ( shared_ptr<CNode> a, shared_ptr<CNode> b ) const;
private:
    const vector<double> & point;
};


class CRTree
{
public:
    CRTree( const string & pr_name, const uint32_t dim,
            const uint32_t min_child_nodes, const uint32_t max_child_nodes,
            const uint32_t cache_size, const uint32_t erased_max );

    CRTree( const string & pr_name );

    ~CRTree();

    void insert( const uint32_t id, const vector<double> & start, const vector<double> & dist );

    list<tuple<uint32_t, vector<double>, vector<double>>> search( const vector<double> & start, const vector<double> & dist );
	
	list<tuple<uint32_t, vector<double>, vector<double>>> knn( const unsigned k, const vector<double> & quary_pint );

    void erase( const uint32_t id );

    uint32_t getDim() const;

    unsigned lastOpIO() const;
private:

    shared_ptr<CNode> at( const uint32_t id );

    bool isLeaf( shared_ptr<CNode> node );

    shared_ptr<CNode> chooseLeaf( shared_ptr<CNode> current, const shared_ptr<CNode> to_insert );

    pair<shared_ptr<CNode>, shared_ptr<CNode>> pickSeeds( list<uint32_t> & child_nodes_id );

    shared_ptr<CNode> pickNext( list<uint32_t> & child_nodes,
                                const shared_ptr<CNode> & A,
                                const shared_ptr<CNode> & B );

    // "quadratic split"
    pair<shared_ptr<CNode>, shared_ptr<CNode>> split( const shared_ptr<CNode> & to_split );

    void adjustTree( const shared_ptr<CNode> current, shared_ptr<CNode> current_split_partner = nullptr );

    void writeNode( const shared_ptr<CNode> node );

    shared_ptr<CNode> readNode( const uint32_t id );

    void save();

    void retrieveUsedIds();

    void rebuild();

    string pr_name;
    fstream file;

    uint32_t HEADER_SIZE;
    uint32_t NODE_SIZE;

    uint32_t dim;

    uint32_t root_id;
    uint32_t next_id;
    unordered_set<uint32_t> data_object_ids_used;

    uint32_t CACHE_SIZE;
    vector<shared_ptr<CNode>> cache;

    unordered_map<uint32_t, uint32_t> parent_nodes;

    unordered_set<uint32_t> erased;
    uint32_t ERASED_MAX;

    unsigned last_op_io;

    friend ostream & operator<<( ostream & os, CRTree & rtree );
};

bool sortByMinDist( const pair<uint32_t, double> & a, const pair<uint32_t, double> & b );

// debug
ostream & operator<<( ostream & os, CRTree & rtree );

#endif // CRTREE_H
