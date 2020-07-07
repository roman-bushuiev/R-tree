#ifndef CNODE_H
#define CNODE_H

#include "chyperrectangle.h"

#include <cstdint>
#include <vector>
#include <list>
#include <fstream>

using namespace std;

class CNode : public CHyperrectangle
{
public:
    CNode();

    CNode( const vector<double> & start, const vector<double> & dist, const uint32_t id );

    CNode( const vector<double> & start, const vector<double> & dist, const uint32_t id, const list<uint32_t> & child_nodes_id );

    CNode( const vector<double> & start, const vector<double> & dist, const uint32_t id, const uint32_t data_object_id );

    // returns false if the node is overflowed
    bool addChild( const CNode & child );

    bool isData() const;

    uint32_t & id();

    const uint32_t & id() const;

    list<uint32_t> & child_nodes_id();

    const list<uint32_t> & child_nodes_id() const;

    uint32_t & data_object_id();

    const uint32_t & data_object_id() const;

    static uint32_t MIN_CHILD_NODES;
    static uint32_t MAX_CHILD_NODES;
    static uint32_t NULL_ID;

private:
    uint32_t id_;
    list<uint32_t> child_nodes_id_;
    uint32_t data_object_id_;
};

ostream & operator<< ( ostream & os, const CNode & node );

#endif // CNODE_H
