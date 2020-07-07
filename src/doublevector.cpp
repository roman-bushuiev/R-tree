#include "doublevector.h"

#include <vector>
#include <string>
#include <QString>

std::vector<double> stodv( const std::string & a )
{
    std::string str = a;
    std::string delimiter = ",";
    size_t pos = 0;
    std::vector<double> res;

    while( ( pos = str.find( delimiter )) != std::string::npos )
    {

        res.push_back( std::stod( str.substr( 0, pos ) ) );
        str.erase( 0, pos + delimiter.length() );
    }
    res.push_back( std::stod( str ) );

    return res;
}

QString dvtoqs( const std::vector<double> & a )
{
    QString res = "";
    for( auto & d : a )
    {
        res += QString::fromStdString( std::to_string( d ) );
        if ( & d != & a.back() )
            res += ", ";
    }

    return res;
}

std::string str_zero_vector( unsigned dim )
{
    std::string res = "";

    for( unsigned i = 0; i < dim; i++ )
    {
        if( i != 0 )
            res.append( ", " );
        res.append( "0" );
    }

    return res;
}
