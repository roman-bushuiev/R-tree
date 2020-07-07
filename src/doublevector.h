#ifndef DOUBLEVECTOR_H
#define DOUBLEVECTOR_H

#include <vector>
#include <string>
#include <QString>

std::vector<double> stodv( const std::string & a );

QString dvtoqs( const std::vector<double> & a );

std::string str_zero_vector( unsigned dim );

#endif // DOUBLEVECTOR_H
