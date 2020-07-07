#include "rtreetest.h"

pair<unsigned, unsigned> rTreeTestA( const unsigned dim, const unsigned n, const unsigned k )
{
    unsigned time_rt;
    unsigned time_no_rt;

    string tmp_path1 = "rttmp1.txt";
    string tmp_path2 = "rttmp2.txt";

    {
        CRTree t1( tmp_path1, dim, MIN_CHILD_NODES, MAX_CHILD_NODES, CACHE_SIZE, MAX_ERASED );
        CNotRTree nt1( tmp_path2, dim );

        // initialize random generator
        random_device rd;
        mt19937 gen( rd() );
        uniform_real_distribution<> dis( 0, n );

        // insert data
        for( uint32_t i = 0 ; i < n ; i++ )
        {
            t1.insert( i, vector<double>( dim, i ), vector<double>( dim, 0 ) );
            nt1.insert( i, vector<double>( dim, i ), vector<double>( dim, 0 ) );
        }

        // create random search area
        vector<double> start;
        vector<double> dist;
        for( unsigned i = 0 ; i < dim ; i++ )
        {
            start.push_back( dis( gen ) );
            dist.push_back( fabs( dis( gen ) ) );
        }

        cout << "-----NO-R-TREE-----\n";

        list<tuple<uint32_t, vector<double>, vector<double>>> searched;

        // run timer and search
        clock_t time_a = clock();
        if( k == 0 )
            searched = nt1.search( start, dist );
        else
            searched = nt1.knn( k, start );
        clock_t time_b = clock();

        if ( time_a == ( (clock_t) - 1 ) || time_b == ( (clock_t) - 1 ) )
            throw runtime_error( "Unable to calculate time" );
        else
            time_no_rt = (unsigned)( time_b - time_a );

        // print results
        cout << "Search result = { ";
        for( const auto & x : searched )
        {
            cout << get<0>( x );
            if( & x != & searched.back() )
                cout << ", ";
        }
        cout << " }.\n";
        cout << "Runtime = " << time_no_rt << " milliseconds.\n";
        cout << nt1.lastOpIO() << " IO operations were required.\n";


        cout << "------R-TREE-------\n";

        // run timer and search
        time_a = clock();
        if( k == 0 )
            searched = t1.search( start, dist );
        else
            searched = t1.knn( k, start );
        time_b = clock();

        if ( time_a == ( (clock_t) - 1 ) || time_b == ( (clock_t) - 1 ) )
            throw runtime_error( "Unable to calculate time" );
        else
            time_rt = (unsigned)( time_b - time_a );

        // print results
        cout << "Search result = { ";
        for( const auto & x : searched )
        {
            cout << get<0>( x );
            if( & x != & searched.back() )
                cout << ", ";
        }
        cout << " }.\n";
        cout << "Runtime = " << time_rt << " milliseconds.\n";
        cout << t1.lastOpIO() << " IO operations were required.\n";
    }

    remove( tmp_path1.c_str() );
    remove( tmp_path2.c_str() );

    return make_pair<>( time_no_rt, time_rt );
}

pair<unsigned, unsigned> rTreeTestB( const unsigned dim, const unsigned n, const unsigned k )
{
    unsigned time_rt;
    unsigned time_no_rt;

    string tmp_path1 = "rttmp1.txt";
    string tmp_path2 = "rttmp2.txt";

    {
        CRTree t1( tmp_path1, dim, MIN_CHILD_NODES, MAX_CHILD_NODES, CACHE_SIZE, MAX_ERASED );
        CNotRTree nt1( tmp_path2, dim );

        // initialize random generator
        random_device rd;
        mt19937 gen( rd() );
        uniform_real_distribution<> dis( -1000, 1000 );

        // insert random data
        vector<double> start;
        vector<double> dist;
        for( uint32_t id = 0 ; id < n ; id++ )
        {
            start.clear();
            dist.clear();
            for( unsigned i = 0 ; i < dim ; i++ )
            {
                start.push_back( dis( gen ) );
                dist.push_back( fabs( dis( gen ) ) / 2 );
            }
            t1.insert( id, start, dist );
            nt1.insert( id, start, dist );
        }

        // create random search area
        start.clear();
        dist.clear();
        for( unsigned i = 0 ; i < dim ; i++ )
        {
            start.push_back( dis( gen ) );
            dist.push_back( fabs( dis( gen ) ) );
        }

        cout << "-----NO-R-TREE-----\n";

        list<tuple<uint32_t, vector<double>, vector<double>>> searched;

        // run timer and search
        clock_t time_a = clock();
        if( k == 0 )
            searched = nt1.search( start, dist );
        else
            searched = nt1.knn( k, start );
        clock_t time_b = clock();

        if ( time_a == ( (clock_t) - 1 ) || time_b == ( (clock_t) - 1 ) )
            throw runtime_error( "Unable to calculate time" );
        else
            time_no_rt = (unsigned)( time_b - time_a );

        // print results
        cout << "Search result = { ";
        for( const auto & x : searched )
        {
            cout << get<0>( x );
            if( & x != & searched.back() )
                cout << ", ";
        }
        cout << " }.\n";
        cout << "Runtime = " << time_no_rt << " milliseconds.\n";
        cout << nt1.lastOpIO() << " IO operations were required.\n";


        cout << "------R-TREE-------\n";

        // run timer and search
        time_a = clock();
        if( k == 0 )
            searched = t1.search( start, dist );
        else
            searched = t1.knn( k, start );
        time_b = clock();

        if ( time_a == ( (clock_t) - 1 ) || time_b == ( (clock_t) - 1 ) )
            throw runtime_error( "Unable to calculate time" );
        else
            time_rt = (unsigned)( time_b - time_a );

        // print results
        cout << "Search result = { ";
        for( const auto & x : searched )
        {
            cout << get<0>( x );
            if( & x != & searched.back() )
                cout << ", ";
        }
        cout << " }.\n";
        cout << "Runtime = " << time_rt << " milliseconds.\n";
        cout << t1.lastOpIO() << " IO operations were required.\n";
    }

    remove( tmp_path1.c_str() );
    remove( tmp_path2.c_str() );

    return make_pair<>( time_no_rt, time_rt );
}
