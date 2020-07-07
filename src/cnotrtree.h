#ifndef CNOTRTREE_H
#define CNOTRTREE_H

#include "chyperrectangle.h"

#include <string>
#include <list>
#include <array>
#include <vector>
#include <string>
#include <memory>
#include <limits>
#include <float.h>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <fstream>
#include <queue>
#include <tuple>
#include <stdexcept>

using namespace std;

class CDataObject : public CHyperrectangle
{
public:
    CDataObject();

    CDataObject( const uint32_t id, const vector<double> & start, const vector<double> & dist );

    uint32_t & id();

    const uint32_t & id() const;

private:
    uint32_t id_;
};

class CNotRTree
{
public:
    CNotRTree ( const string & pr_name, const uint32_t dim );

    ~CNotRTree();

    void insert( const uint32_t id, const vector<double> & start, const vector<double> & dist );

    list<tuple<uint32_t, vector<double>, vector<double>>> search( const vector<double> & start, const vector<double> & dist );

    list<tuple<uint32_t, vector<double>, vector<double>>> knn( const unsigned k, const vector<double> & query_point );

    unsigned lastOpIO() const;

private:

    void writeDataObject( const CDataObject & obj );

    CDataObject readDataObject( const uint32_t id );

    string pr_name;
    fstream file;

    uint32_t DATA_OBJECT_SIZE;
    uint32_t dim;

    unordered_set<uint32_t> data_object_ids_used;

    uint32_t CACHE_SIZE;

    uint32_t next_id;

    unsigned last_op_io;
};

bool sortByMinDist( const pair<CDataObject, double> & a, const pair<CDataObject, double> & b );

#endif // CNOTRTREE_H
