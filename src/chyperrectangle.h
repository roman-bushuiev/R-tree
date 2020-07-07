#ifndef CHYPERRECTANGLE_H
#define CHYPERRECTANGLE_H

#include <vector>
#include <list>
#include <vector>
#include <string>
#include <math.h>

#include <iostream>

using namespace std;

class CHyperrectangle
{
public:
    CHyperrectangle();

    CHyperrectangle( const vector<double> & start, const vector<double> & dist );

    // Point/vector as hyperrectangle
    CHyperrectangle( const vector<double> & start );

    bool contains( const CHyperrectangle & other ) const;

    bool overlaps( const CHyperrectangle & other ) const;

    void merge( const CHyperrectangle & other );

    double enlargementWith( const CHyperrectangle & other ) const;

    double volume() const;

    double mindist( const vector<double> & point );

    vector<double> & start();

    const vector<double> & start() const;

    vector<double> & dist();

    const vector<double> & dist() const;

protected:
    // left lower point
    vector<double> start_;
    // distances from start in positive directions
    vector<double> dist_;
};

CHyperrectangle merge( const CHyperrectangle & A, const CHyperrectangle & B );

// debug
ostream & operator<<( ostream & os, const CHyperrectangle & hrectangle );

#endif // CHYPERRECTANGLE_H
