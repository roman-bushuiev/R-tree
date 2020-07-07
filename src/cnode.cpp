#include "cnode.h"

CNode::CNode()
    : id_( NULL_ID ), data_object_id_( NULL_ID )
{}

CNode::CNode( const vector<double> & start, const vector<double> & dist, const uint32_t id )
    : CHyperrectangle( start, dist ), id_( id ), data_object_id_( NULL_ID )
{}

CNode::CNode( const vector<double> & start, const vector<double> & dist, const uint32_t id, const list<uint32_t> & child_nodes_id )
    : CHyperrectangle( start, dist ), id_( id ), child_nodes_id_( child_nodes_id ), data_object_id_( NULL_ID )
{}

CNode::CNode( const vector<double> & start, const vector<double> & dist, const uint32_t id, const uint32_t data_object_id )
    : CHyperrectangle( start, dist ), id_( id ), data_object_id_( data_object_id )
{}

bool CNode::addChild( const CNode & child )
{
    child_nodes_id_.push_back( child.id() );
    merge( child );
    return ! ( child_nodes_id_.size() > MAX_CHILD_NODES );
}

bool CNode::isData() const { return child_nodes_id_.empty(); }

uint32_t & CNode::id() { return id_; };

const uint32_t & CNode::id() const { return id_; }

list<uint32_t> & CNode::child_nodes_id() { return child_nodes_id_; };

const list<uint32_t> & CNode::child_nodes_id() const { return child_nodes_id_; };

uint32_t & CNode::data_object_id() { return data_object_id_; };

const uint32_t & CNode::data_object_id() const { return data_object_id_; };

// default values preset
unsigned CNode::MIN_CHILD_NODES = 2;
unsigned CNode::MAX_CHILD_NODES = 50;
uint32_t CNode::NULL_ID = 0;

ostream & operator<< ( ostream & os, const CNode & node )
{
    os << node.id() << " ";
    os << ( CHyperrectangle ) node << " ";
    if( node.isData() )
    {
        os << " data_object_id: " << node.data_object_id();
        if( ! node.child_nodes_id().empty() )
            os << "CORRUPTED";
    }
    else
    {
        os << " child_nodes_id: ";
        for( auto x : node.child_nodes_id() )
            os << x << ", ";
    }
    return os;
}
