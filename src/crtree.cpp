#include "crtree.h"

CompareMindist::CompareMindist( const vector<double> & point )
    : point( point )
{}

bool CompareMindist::operator() ( shared_ptr<CNode> a, shared_ptr<CNode> b ) const
{
    return a->mindist( point ) < b->mindist( point );
}


CRTree::CRTree( const string & pr_name, const uint32_t dim,
                const uint32_t min_child_nodes, const uint32_t max_child_nodes,
                const uint32_t cache_size, const uint32_t erased_max )
    : pr_name( pr_name ), dim( dim ),
      root_id( CNode::NULL_ID ), next_id( 1 ),
      CACHE_SIZE( cache_size ), cache( CACHE_SIZE, nullptr ), ERASED_MAX( erased_max ),
      last_op_io( 0 )
{
    CNode::MIN_CHILD_NODES = min_child_nodes;
    CNode::MAX_CHILD_NODES = max_child_nodes;

    NODE_SIZE = 2 * dim * sizeof( double ) + 2 * sizeof( uint32_t ) + CNode::MAX_CHILD_NODES * sizeof( uint32_t );

    file.open( pr_name, ios::in );
    if( file.good() ) throw logic_error( "\"" + pr_name + "\"" + " already exists." );

    file.open( pr_name, ios::in | ios::out | ios::binary | ios::trunc );
    if( ! file.is_open() ) throw runtime_error( "\"" + pr_name + "\"" + " can't be opened." );

    HEADER_SIZE = sizeof ( dim ) + sizeof ( root_id ) + sizeof ( next_id ) + sizeof ( CACHE_SIZE ) + sizeof ( ERASED_MAX )
                + sizeof ( CNode::MIN_CHILD_NODES ) + sizeof ( CNode::MAX_CHILD_NODES ) + sizeof ( CNode::NULL_ID );

    save();
}

CRTree::CRTree( const string & pr_name )
    : pr_name( pr_name ), last_op_io( 0 )
{

    file.open( pr_name, ios::in | ios::out | ios::binary );
    if( ! file.is_open() ) throw runtime_error( "\"" + pr_name + "\"" + " cannot be opened." );

    file.read( ( char * ) & dim, sizeof ( dim ) );
    file.read( ( char * ) & root_id, sizeof ( root_id ) );
    file.read( ( char * ) & next_id, sizeof ( next_id ) );
    file.read( ( char * ) & ERASED_MAX, sizeof ( ERASED_MAX ) );
    file.read( ( char * ) & CACHE_SIZE, sizeof ( CACHE_SIZE ) );
    file.read( ( char * ) & CNode::MIN_CHILD_NODES, sizeof ( CNode::MIN_CHILD_NODES ) );
    file.read( ( char * ) & CNode::MAX_CHILD_NODES, sizeof ( CNode::MAX_CHILD_NODES ) );
    file.read( ( char * ) & CNode::NULL_ID, sizeof ( CNode::NULL_ID ) );

    if( file.bad() ) throw runtime_error( "\"" + pr_name + "\" is corrupted." );

    cache = vector<shared_ptr<CNode>>( CACHE_SIZE, nullptr );

    NODE_SIZE = 2 * dim * sizeof( double ) + 2 * sizeof( uint32_t ) + CNode::MAX_CHILD_NODES * sizeof( uint32_t );

    HEADER_SIZE = sizeof ( dim ) + sizeof ( root_id ) + sizeof ( next_id ) + sizeof ( CACHE_SIZE ) + sizeof ( ERASED_MAX )
                + sizeof ( CNode::MIN_CHILD_NODES ) + sizeof ( CNode::MAX_CHILD_NODES ) + sizeof ( CNode::NULL_ID );

    file.seekg( HEADER_SIZE + next_id * NODE_SIZE );
    uint64_t erased_size;
    file.read( ( char * ) & erased_size, sizeof ( erased_size ) );
    uint32_t tmp;
    for( unsigned i = 0 ; i < erased_size ; i++ )
    {
        file.read( ( char * ) & tmp, sizeof( tmp ) );
        erased.insert( tmp );
    }

    retrieveUsedIds();
}

CRTree::~CRTree()
{
    save();
    file.close();
}

void CRTree::insert( const uint32_t data_object_id, const vector<double> & start, const vector<double> & dist )
{
    last_op_io = 0;

    if( start.size() != dim || dist.size() != dim )
        throw logic_error( "Wrong dimension." );

    for( const auto x : dist )
        if( x < 0 )
            throw logic_error( "The distance cannot be negative." );

    if( data_object_ids_used.count( data_object_id ) )
        throw logic_error( "The data object with id " + to_string( data_object_id ) + " already exists." );

    if( erased.count( data_object_id ) )
        rebuild();

    parent_nodes.clear();
    data_object_ids_used.insert( data_object_id );

    shared_ptr<CNode> to_insert( new CNode( start, dist, next_id, data_object_id ) );
    writeNode( to_insert );
    next_id++;

    // if to_insert is the first node
    if( next_id == 2 )
    {
        shared_ptr<CNode> root( new CNode( start, dist, next_id, list<uint32_t>{ to_insert->id() } ) );
        cache.at( next_id ) = root;
        next_id++;
        writeNode( root );
        root_id = root->id();
        return;
    }

    shared_ptr<CNode> destination = chooseLeaf( at( root_id ), to_insert );

    shared_ptr<CNode> split_partner = nullptr;
    if( ! destination->addChild( * to_insert ) )
    {
        pair<shared_ptr<CNode> &, shared_ptr<CNode> &>( destination, split_partner ) = split( destination );
    }
    adjustTree( destination, split_partner );
}

list<tuple<uint32_t, vector<double>, vector<double>>> CRTree::search( const vector<double> & start, const vector<double> & dist )
{
    last_op_io = 0;

    if( start.size() != dim || dist.size() != dim )
        throw logic_error( "Wrong dimension." );

    for( const auto x : dist )
        if( x < 0 )
            throw logic_error( "The distance cannot be negative." );

    if( next_id == 1 )
        return {};

    CHyperrectangle searchArea( start, dist );

    list<tuple<uint32_t, vector<double>, vector<double>>> retval;

    queue<shared_ptr<CNode>> q;
    q.push( at( root_id ) );
    shared_ptr<CNode> current;

    while( ! q.empty() )
    {
        current = q.front();
        q.pop();

        for( const uint32_t child_node_id : current->child_nodes_id() )
            if( searchArea.overlaps( * at( child_node_id ) ) )
                q.push( at( child_node_id ) );

        if( current->isData() && searchArea.contains( * current ) )
            if( ! erased.count( current->data_object_id() ) )
                retval.push_back( make_tuple( current->data_object_id(), current->start(), current->dist() ) );
    }

    return retval;
}

void CRTree::erase( const uint32_t id )
{
    last_op_io = 0;

    if( ! data_object_ids_used.count( id ) )
        throw logic_error( "Data object with id " + to_string( id ) + " does not exist." );

    data_object_ids_used.erase( id );

    erased.insert( id );

    if( erased.size() > ERASED_MAX )
        rebuild();
}

unsigned CRTree::lastOpIO() const { return last_op_io; }

shared_ptr<CNode> CRTree::at( const uint32_t id )
{
    if( ! cache.at( id % CACHE_SIZE ) || cache.at( id % CACHE_SIZE )->id() != id )
    {
        cache.at( id % CACHE_SIZE ) = readNode( id );
    }
    return cache.at( id % CACHE_SIZE );
}

bool CRTree::isLeaf( shared_ptr<CNode> node )
{
    return at( node->child_nodes_id().front() )->isData();
}

shared_ptr<CNode> CRTree::chooseLeaf( shared_ptr<CNode> current, const shared_ptr<CNode> to_insert )
{
    if( isLeaf( current ) )
        return current;

    double min_enlargement = DBL_MAX;
    double current_enlargement;
    shared_ptr<CNode> chosen_node = nullptr;

    for( auto child_node_id : current->child_nodes_id() )
    {
        current_enlargement = at( child_node_id )->enlargementWith( * to_insert );

        if( current_enlargement < min_enlargement )
        {
            min_enlargement = current_enlargement;
            chosen_node = at( child_node_id );
        }
        else if( current_enlargement == min_enlargement )
        {
            if( chosen_node == nullptr || at( child_node_id )->volume() < chosen_node->volume() )
            {
                min_enlargement = current_enlargement;
                chosen_node = at( child_node_id );
            }
        }
    }

    parent_nodes.emplace( chosen_node->id(), current->id() );
    return chooseLeaf( chosen_node, to_insert );
}

pair<shared_ptr<CNode>, shared_ptr<CNode>> CRTree::pickSeeds( list<uint32_t> & child_nodes_id )
{
    pair<list<uint32_t>::const_iterator, list<uint32_t>::const_iterator> seeds_it;
    shared_ptr<CNode> currentA, currentB;
    double max_waste = -1;
    double waste;

    for( auto it1 = child_nodes_id.begin() ; it1 != child_nodes_id.end() ; it1++ )
        for( auto it2 = next( it1 ) ; it2 != child_nodes_id.end() ; it2++ )
        {
            currentA = at( * it1 );
            currentB = at( * it2 );
            waste = merge( * currentA, * currentB ).volume() + currentA->volume() + currentB->volume();
            if( waste > max_waste )
            {
                max_waste = waste;
                seeds_it = make_pair( it1, it2 );
            }
        }

    pair<shared_ptr<CNode>, shared_ptr<CNode>> retval = make_pair( at( * seeds_it.first ), at( * seeds_it.second ) );
    child_nodes_id.erase( seeds_it.first );
    child_nodes_id.erase( seeds_it.second );
    return retval;
}


shared_ptr<CNode> CRTree::pickNext( list<uint32_t> & child_nodes,
                                    const shared_ptr<CNode> & nodeA,
                                    const shared_ptr<CNode> & nodeB )
{
    list<uint32_t>::const_iterator picked_it;
    double max_diff = -1;
    double diff;

    for( auto it = child_nodes.begin() ; it != child_nodes.end() ; it++ )
    {
        diff = abs( nodeA->enlargementWith( * at( * it ) ) - nodeB->enlargementWith( * at( * it ) ) );
        if( diff > max_diff )
        {
            max_diff = diff;
            picked_it = it;
        }
    }

    shared_ptr<CNode> retval = at( * picked_it );
    child_nodes.erase( picked_it );
    return retval;
}

pair<shared_ptr<CNode>, shared_ptr<CNode>> CRTree::split( const shared_ptr<CNode> & to_split )
{
    auto seeds = pickSeeds( to_split->child_nodes_id() );
    shared_ptr<CNode> nodeA = make_shared<CNode>( CNode( seeds.first->start(), seeds.first->dist(), to_split->id(), list<uint32_t>{ seeds.first->id() } ) );
    shared_ptr<CNode> nodeB = make_shared<CNode>( CNode( seeds.second->start(), seeds.second->dist(), next_id, list<uint32_t>{ seeds.second->id() } ) );
    next_id++;

    if( to_split->id() != root_id )
        parent_nodes.emplace( nodeB->id(), parent_nodes.at( to_split->id() ) );

    shared_ptr<CNode> next;
    double nodeA_enlargement, nodeB_enlargement;
    double nodeA_volume, nodeB_volume;

    while( ! to_split->child_nodes_id().empty() )
    {

        if( nodeA->child_nodes_id().size() + to_split->child_nodes_id().size() == CNode::MIN_CHILD_NODES )
        {
            for( auto id : to_split->child_nodes_id() )
                nodeA->addChild( * at( id ) );
            break;
        }
        else if( nodeB->child_nodes_id().size() + to_split->child_nodes_id().size() == CNode::MIN_CHILD_NODES )
        {
            for( auto id : to_split->child_nodes_id() )
                nodeB->addChild( * at( id ) );
            break;
        }

        next = pickNext( to_split->child_nodes_id(), nodeA, nodeB );

        nodeA_enlargement = nodeA->enlargementWith( * next );
        nodeB_enlargement = nodeB->enlargementWith( * next );
        if( nodeA_enlargement < nodeB_enlargement )
        {
            nodeA->addChild( * next );
        }
        else if( nodeA_enlargement == nodeB_enlargement )
        {
            nodeA_volume = nodeA->volume();
            nodeB_volume = nodeB->volume();
            if( nodeA_volume < nodeB_volume )
            {
                nodeA->addChild( * next );
            }
            else if( nodeA_volume == nodeB_volume )
            {
                if( nodeA->child_nodes_id().size() <= nodeB->child_nodes_id().size() )
                {
                    nodeA->addChild( * next );
                }
                else
                {
                    nodeB->addChild( * next );
                }
            }
            else
            {
                nodeB->addChild( * next );
            }
        }
        else
        {
            nodeB->addChild( * next );
        }
    }

    return make_pair( nodeA, nodeB );
}

void CRTree::adjustTree( shared_ptr<CNode> current, shared_ptr<CNode> split_partner )
{
    writeNode( current );
    if( split_partner )
        writeNode( split_partner );

    shared_ptr<CNode> parent;

    while( current->id() != root_id )
    {
        parent = at( parent_nodes.at( current->id() ) );
        parent->merge( * current );

        if( split_partner )
        {
            parent->merge( * split_partner );
            if( ! parent->addChild( * split_partner ) )
            {
                pair<shared_ptr<CNode> &, shared_ptr<CNode> &> ( current, split_partner ) = split( parent );
            }
            else
            {
                current = parent;
                split_partner = nullptr;
            }
        }
        else
        {
            current = parent;
            split_partner = nullptr;
        }

        writeNode( current );
        if( split_partner )
            writeNode( split_partner );
    }

    if( split_partner )
    {
        CHyperrectangle merged = merge( * current, * split_partner );
        shared_ptr<CNode> root( new CNode( merged.start(), merged.dist(), next_id, list<uint32_t>{ current->id(), split_partner->id() } ) );
        next_id++;
        root_id = root->id();
        writeNode( root );
    }

}

void CRTree::writeNode( const shared_ptr<CNode> node )
{
    cache.at( node->id() % CACHE_SIZE ) = node;

    file.seekp( HEADER_SIZE + ( node->id() - 1 ) * NODE_SIZE );

    file.write( ( char * ) & node->id(), sizeof( node->id() ) );

    for( const double x : node->start() )
        file.write( ( char * ) & x, sizeof( x ) );
    for( const double x : node->dist() )
        file.write( ( char * ) & x, sizeof( x ) );

    file.write( ( char * ) & node->data_object_id(), sizeof( node->data_object_id() ) );
    for( const uint32_t child_node_id : node->child_nodes_id() )
        file.write( ( char * ) & child_node_id, sizeof( child_node_id ) );
    for( unsigned i = 0 ; i < CNode::MAX_CHILD_NODES - node->child_nodes_id().size() ; i++ )
        file.write( ( char * ) & CNode::NULL_ID, sizeof( CNode::NULL_ID ) );

    if( file.bad() ) throw runtime_error( "\"" + pr_name + "\"" + " is corrupted" );

    last_op_io++;
}

shared_ptr<CNode> CRTree::readNode( const uint32_t id )
{
    CNode node;

    uint32_t tmp_u;
    double tmp_d;

    file.seekg( HEADER_SIZE + ( id - 1 ) * NODE_SIZE );

    file.read( ( char * ) & node.id(), sizeof( node.id() ) );

    for( unsigned i = 0 ; i < dim ; i++ )
    {
        file.read( ( char * ) & tmp_d, sizeof( tmp_d ) );
        node.start().push_back( tmp_d );
    }
    for( unsigned i = 0 ; i < dim ; i++ )
    {
        file.read( ( char * ) & tmp_d, sizeof( tmp_d ) );
        node.dist().push_back( tmp_d );
    }

    file.read( ( char * ) & tmp_u, sizeof ( tmp_u ) );
    if( tmp_u != 0 )
    {
        node.data_object_id() = tmp_u;
    }
    else
    {
        for( unsigned i = 0 ; i < CNode::MAX_CHILD_NODES ; i++ )
        {
            file.read( ( char * ) & tmp_u, sizeof( tmp_u ) );
            if( tmp_u == CNode::NULL_ID )
                break;
            node.child_nodes_id().push_back( tmp_u );
        }
    }

    if( file.bad() ) throw runtime_error( "\"" + pr_name + "\"" + " is corrupted" );

    last_op_io++;
    return make_shared<CNode>( node );
}

void CRTree::retrieveUsedIds()
{
    if( next_id == 1 )
        return;

    queue<shared_ptr<CNode>> q;
    q.push( at( root_id ) );
    shared_ptr<CNode> current;

    while( ! q.empty() )
    {
        current = q.front();
        q.pop();

        for( const uint32_t child_node_id : current->child_nodes_id() )
            q.push( at( child_node_id ) );

        if( current->isData() )
            if( ! erased.count( current->data_object_id() ) )
                data_object_ids_used.insert( current->data_object_id() );
    }
}

uint32_t CRTree::getDim() const
{
    return dim;
}

void CRTree::save()
{
    file.seekp( 0 );

    file.write( ( char * ) & dim, sizeof ( dim ) );
    file.write( ( char * ) & root_id, sizeof ( root_id ) );
    file.write( ( char * ) & next_id, sizeof ( next_id ) );
    file.write( ( char * ) & ERASED_MAX, sizeof ( ERASED_MAX ) );
    file.write( ( char * ) & CACHE_SIZE, sizeof ( CACHE_SIZE ) );
    file.write( ( char * ) & CNode::MIN_CHILD_NODES, sizeof ( CNode::MIN_CHILD_NODES ) );
    file.write( ( char * ) & CNode::MAX_CHILD_NODES, sizeof ( CNode::MAX_CHILD_NODES ) );
    file.write( ( char * ) & CNode::NULL_ID, sizeof ( CNode::NULL_ID ) );

    file.seekp( HEADER_SIZE + next_id * NODE_SIZE );
    uint64_t erased_size = erased.size();
    file.write( ( char * ) & erased_size, sizeof( erased_size ) );
    for( const auto id : erased )
        file.write( ( char * ) & id, sizeof ( id ) );

    if( file.bad() ) throw runtime_error( "Project could not be saved correctly." );
}

void CRTree::rebuild()
{
    auto data_objects = search( at( root_id )->start(), at( root_id )->dist() );

    next_id = 1;
    data_object_ids_used.clear();
    cache = vector<shared_ptr<CNode>>( CACHE_SIZE, nullptr );

    for( const auto data_object : data_objects )
        if( ! erased.count( get<0>( data_object ) ) )
            insert( get<0>( data_object ), get<1>( data_object ), get<2>( data_object ) );

    erased.clear();
    save();
}

bool sortByMinDist( const pair<uint32_t, double> & a, const pair<uint32_t, double> & b )
{
    return a.second < b.second;
}

list<tuple<uint32_t, vector<double>, vector<double>>> CRTree::knn( const unsigned k, const vector<double> & query_point )
{
    last_op_io = 0;

    if( query_point.size() != dim )
        throw logic_error( "Wrong dimension." );

    if( k > data_object_ids_used.size() )
        throw logic_error( "There are " + to_string( data_object_ids_used.size() ) + " data objects in total, which is less then k." );

    if( next_id == 1 || k == 0 )
        return {};

    list<tuple<uint32_t, vector<double>, vector<double>>> res;

    stack<shared_ptr<CNode>> s;
    s.push( at( root_id ) );

    shared_ptr<CNode> current;
    priority_queue<shared_ptr<CNode>, vector<shared_ptr<CNode>>, CompareMindist> pq((CompareMindist( query_point )));
    vector<pair<uint32_t, float>> res_id;

    while( ! s.empty() )
    {
        current = s.top();
        s.pop();

        if( current->isData() )
        {
            if( res_id.size() == k )
            {
                if( current->mindist( query_point ) <= res_id.back().second )
                {
                    res_id.pop_back();
                    res_id.push_back( make_pair( current->id(), current->mindist( query_point ) ) );
                    sort( res_id.begin(), res_id.end(), sortByMinDist );
                }
            }
            else
            {
                res_id.push_back( make_pair( current->id(), current->mindist( query_point ) ) );
                sort( res_id.begin(), res_id.end(), sortByMinDist );
            }
        }else
        {
            for( const uint32_t child_node_id : current->child_nodes_id() )
            {
                if( res_id.size() == 0 || res_id.size() < k || at( child_node_id )->mindist( query_point ) <= res_id.back().second )
                    pq.push( at( child_node_id ) );
            }

            while( ! pq.empty() )
            {
                s.push( pq.top() );
                pq.pop();
            }
        }
    }

    for( auto nn : res_id )
        res.push_back( make_tuple( at( nn.first )->data_object_id(), at( nn.first )->start(), at( nn.first )->dist() ) );

    return res;
}

ostream & operator<<( ostream & os, CRTree & rtree )
{
    cout << "root id: " << rtree.root_id << endl;
    cout << "CNode::MAX_CHILD_NODES: " << CNode::MAX_CHILD_NODES << endl;
    cout << "CNode::MIN_CHILD_NODES: " << CNode::MIN_CHILD_NODES << endl;
    for( unsigned i = 1 ; i < rtree.next_id ; i++ )
        os << * rtree.at( i ) << endl;
    os << "data object ids used: ";
    for( auto x : rtree.data_object_ids_used )
        os << x << ", ";
    os << endl;
    os << "erased: ";
    for( auto x : rtree.erased )
        os << x << ", ";
    os << endl;
    return os;
}
