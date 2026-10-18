#pragma once

class Point{
public:
    Point(int x, int y);
    int x();
    int y();

    int distance(Point p);
    Point rotate90();

private:
    int m_x, m_y;
};