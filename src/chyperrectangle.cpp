#include "chyperrectangle.h"


CHyperrectangle::CHyperrectangle() {}

CHyperrectangle::CHyperrectangle( const vector<double> & start, const vector<double> & dist )
    : start_( start ), dist_( dist )
{}

CHyperrectangle::CHyperrectangle( const vector<double> & start )
    : start_( start ), dist_( start.size(), 0 )
{}

bool CHyperrectangle::contains( const CHyperrectangle & other ) const
{
    for( unsigned i = 0 ; i < start_.size() ; i++ )
        if( start_.at( i ) > other.start().at( i )
            || start_.at( i ) + dist_.at( i ) < other.start().at( i ) + other.dist().at( i ) )
        {
            return false;
        }
    return true;
}

bool CHyperrectangle::overlaps( const CHyperrectangle & other ) const
{
    for( unsigned i = 0 ; i < start_.size() ; i++ )
        if( start_.at( i ) + dist_.at( i ) < other.start().at( i )
            || other.start().at( i ) + other.dist().at( i ) < start_.at( i ) )
        {
            return false;
        }
    return true;
}

void CHyperrectangle::merge( const CHyperrectangle & other )
{
    double end;
    for( unsigned i = 0 ; i < start_.size() ; i++ )
    {
        end = max( start_.at( i ) + dist_.at( i ), other.start().at( i ) + other.dist().at( i ) );
        start_.at( i ) = min( start_.at( i ), other.start().at( i ) );
        dist_.at( i ) = abs( end - start_.at( i ) );
    }
}

double CHyperrectangle::enlargementWith( const CHyperrectangle & other ) const
{
    CHyperrectangle merged( * this );
    merged.merge( other );
    return merged.volume() - volume();
}

double CHyperrectangle::volume() const
{
    double volume = 1;
    for( auto d : dist_ )
        volume *= d;
    return volume;
}

double CHyperrectangle::mindist( const vector<double> & point )
{
    double res = 0;
    double r;

    for( unsigned i = 0; i < point.size(); i++ )
    {
        if( point.at( i ) < start_.at( i ) )
        {
            r = start_.at( i );
        }else if( point.at( i ) > start_.at( i ) + dist_.at( i ) )
        {
            r = start_.at( i ) + dist_.at( i );
        }else
        {
            r = point.at( i );
        }
        res += pow( abs( point.at( i ) - r ), 2 );
    }

    return res;
}

vector<double> & CHyperrectangle::start() { return start_; }

const vector<double> & CHyperrectangle::start() const { return start_; }

vector<double> & CHyperrectangle::dist() { return dist_; }

const vector<double> & CHyperrectangle::dist() const { return dist_; }

CHyperrectangle merge( const CHyperrectangle & A, const CHyperrectangle & B )
{
    CHyperrectangle merged( A );
    merged.merge( B );
    return merged;
}

ostream & operator<<( ostream & os, const CHyperrectangle & hrectangle )
{
    os << "{ ";
    for( auto x : hrectangle.start() )
        os << x << ", ";
    os << "}, { ";
    for( auto x : hrectangle.dist() )
        os << x << ", ";
    os << "}";
    return os;
}
