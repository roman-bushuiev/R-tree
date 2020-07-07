#include "cnotrtree.h"

CDataObject::CDataObject( const uint32_t id, const vector<double> & start, const vector<double> & dist )
    : CHyperrectangle( start, dist ), id_( id )
{}

CDataObject::CDataObject()
{}

uint32_t & CDataObject::id() { return id_; };

const uint32_t & CDataObject::id() const { return id_; }

CNotRTree::CNotRTree( const string & pr_name, const uint32_t dim )
    : pr_name( pr_name ), dim( dim ), next_id( 0 ), last_op_io( 0 )                                                                                                                    // <-----
{
    DATA_OBJECT_SIZE = 2 * dim * sizeof( double ) + sizeof( uint32_t );

    file.open( pr_name, ios::in );
    if( file.good() ) throw logic_error( "\"" + pr_name + "\"" + " already exists." );

    file.open( pr_name, ios::in | ios::out | ios::binary | ios::trunc );
    if( ! file.is_open() ) throw runtime_error( "\"" + pr_name + "\"" + " can't be opened." );
}

CNotRTree::~CNotRTree()
{
    file.close();
    remove( pr_name.c_str() );
}

void CNotRTree::insert( const uint32_t id, const vector<double> & start, const vector<double> & dist )
{
    last_op_io = 0;

    if( start.size() != dim || dist.size() != dim )
        throw logic_error( "Wrong dimension." );

    if( data_object_ids_used.count( id ) )
        throw logic_error( "The data object with id " + to_string( id ) + " already exists." );

    data_object_ids_used.insert( id );

    CDataObject to_insert( id, start, dist );
    writeDataObject( to_insert );
    next_id++;
}

list<tuple<uint32_t, vector<double>, vector<double>>> CNotRTree::search( const vector<double> & start, const vector<double> & dist )
{
    last_op_io = 0;

    if( start.size() != dim || dist.size() != dim )
        throw logic_error( "Wrong dimension." );

    if( next_id == 0 )
        return {};

    CHyperrectangle searchArea( start, dist );

    list<tuple<uint32_t, vector<double>, vector<double>>> retval;

    CDataObject current;
    for( size_t id = 0 ; id < next_id ; id++ )
    {
        current = readDataObject( id );
        if( searchArea.contains( current ) )
            retval.push_back( make_tuple( current.id(), current.start(), current.dist() ) );
    }

    return retval;
}

bool sortByMinDist( const pair<CDataObject, double> & a, const pair<CDataObject, double> & b )
{
    return a.second < b.second;
}

list<tuple<uint32_t, vector<double>, vector<double>>> CNotRTree::knn( const unsigned k, const vector<double> & query_point )
{
    last_op_io = 0;

    if( query_point.size() != dim )
        throw logic_error( "Wrong dimension." );

    if( k > data_object_ids_used.size() )
        throw logic_error( "There are " + to_string( data_object_ids_used.size() ) + " data objects in total, which is less then k." );

    if( next_id == 0 || k == 0 )
        return {};

    list<tuple<uint32_t, vector<double>, vector<double>>> res;

    vector<pair<CDataObject, double>> nn;

    CDataObject current;
    double current_mindist;
    for( size_t id = 0 ; id < next_id ; id++ )
    {
        current = readDataObject( id );
        current_mindist = current.mindist( query_point );

        if( nn.size() < k )
        {
            nn.push_back( make_pair<>( current, current_mindist ) );
            if( nn.size() == k )
                sort( nn.begin(), nn.end(), sortByMinDist );
        }
        else
        {
            if( current_mindist < nn.back().second )
            {
                nn.back() = make_pair<>( current, current_mindist );
                sort( nn.begin(), nn.end(), sortByMinDist );
            }
        }
    }

    for( unsigned i = 0 ; i < k ; i++ )
        res.push_back( make_tuple( nn.at( i ).first.id(), nn.at( i ).first.start(), nn.at( i ).first.dist() ) );

    return res;
}

unsigned CNotRTree::lastOpIO() const { return last_op_io; }

void CNotRTree::writeDataObject( const CDataObject & obj )
{
    file.seekp( obj.id() * DATA_OBJECT_SIZE );

    file.write( ( char * ) & obj.id(), sizeof( obj.id() ) );

    for( const double x : obj.start() )
        file.write( ( char * ) & x, sizeof( x ) );
    for( const double x : obj.dist() )
        file.write( ( char * ) & x, sizeof( x ) );

    if( file.bad() ) throw runtime_error( "\"" + pr_name + "\"" + " is corrupted" );

    last_op_io++;
}

CDataObject CNotRTree::readDataObject( const uint32_t id )
{
    CDataObject obj;

    double tmp_d;

    file.seekg( id * DATA_OBJECT_SIZE );

    file.read( ( char * ) & obj.id(), sizeof( obj.id() ) );

    for( unsigned i = 0 ; i < dim ; i++ )
    {
        file.read( ( char * ) & tmp_d, sizeof( tmp_d ) );
        obj.start().push_back( tmp_d );
    }
    for( unsigned i = 0 ; i < dim ; i++ )
    {
        file.read( ( char * ) & tmp_d, sizeof( tmp_d ) );
        obj.dist().push_back( tmp_d );
    }


    if( file.bad() ) throw runtime_error( "\"" + pr_name + "\"" + " is corrupted" );

    last_op_io++;
    return obj;
}
