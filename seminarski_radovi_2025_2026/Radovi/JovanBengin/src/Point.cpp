#include "Point.hpp"
#include <algorithm>

Point::Point(int x, int y) : m_x(x), m_y(y){}
int Point::x(){ return m_x; }
int Point::y(){ return m_y; }

int Point::distance(Point p){ return abs(x() - p.x()) + abs(y() - p.y()); }
Point Point::rotate90(){ return Point(-m_y, m_x); }