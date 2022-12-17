#include <sstream>
#include <iostream>
#include "../../geometry.h"
#include <cassert>
#include <utility>
#include <vector>

using std::vector;
using std::cout;
using std::endl;


void print_vector (const vector<Point>& in)
{
    for (auto i:in)
    {
        cout << "(" <<i.x << ", " << i.y << ")" << "; ";
    }
    cout << endl;
}

int main()
{
    Point a (1,1);
    Point b (2,2);
    assert (a!=b && "a(1,1) b(2,2)");
    Point c = a;
    assert (a==c && "c = a");

    Line eq(1, 0);
    Line fromPoints (a,b);
    assert (eq == fromPoints && "eq(1,0) fromPoints ((1,1), (2,2))");
    assert (eq != Line (Point(1,1), Point(1,2)) && "eq(1,0)");
    
    vector <Shape*> shapes;

    vector <Point> points1 = {Point(0,1), Point(1,1), Point(1,0), Point(0,0)};
    Polygon pa (points1);
    Polygon pb (Point(1,1), Point(1,0), Point(0,0), Point(0,1));
    assert (pa==pb);
    pb = Polygon (Point(-1,1), Point(-1,0), Point(0,0), Point(0,1));
    pb.reflect (Line(0,-1));
    assert (pa.isCongruentTo (pb));
    assert (pa.isSimilarTo(pb));
    shapes.push_back (&pa);

    Rectangle ra(Point(0,0), Point (1,1), 1);
    assert (ra == pa);
    assert (pa == ra);
    assert (ra.isCongruentTo (pb));
    assert (pb.isSimilarTo (ra));
    shapes.push_back (&ra);
    Polygon* aa = dynamic_cast <Polygon*> (&ra);

    Square sa(Point(0,0), Point(1,1));
    assert (sa == pa);
    assert (pa == sa);
    assert (sa.isCongruentTo (pb));
    assert (pb.isSimilarTo (sa));
    shapes.push_back (&sa);

    Ellipse ea (Point (1,1), Point(1,1), 2);
    assert (ea.focuses() == std::make_pair (Point(1,1), Point(1,1)));
    ea.directrices();
    assert (ea.eccentricity() < 1e-4);
    shapes.push_back (&ea);

    Circle ca (Point(1,1), 1);
    assert (ca.center() == Point(1,1));
    assert (ca.focuses() == std::make_pair (Point(1,1), Point(1,1)));
    assert (ca == ea);
    assert (ea == ca);
    assert (ca.isCongruentTo(ea));
    assert (ea.isSimilarTo(ca));

    Triangle ta (Point (0,0), Point (1,1), Point (1,0));
    Triangle tb (Point (0,0), Point (1,1), Point (0,1));

    ta.circumscribedCircle();
    ta.inscribedCircle();
    ta.centroid();
    ta.orthocenter();
    ta.EulerLine();
    ta.ninePointsCircle();
    assert (ta.area() + tb.area() - sa.area() < 1e-4);
    assert (ta != tb);
    assert (ta.isCongruentTo(tb));
    assert (ta.isSimilarTo(tb));

    Triangle tc (Point (0,0), Point (1,0), Point(0.5,1));
    shapes.push_back (&tc);

    for (auto i: shapes)
    {
        assert (i->area() > 0);
        assert (i->containsPoint (Point (0.5,0.5)));
        double perimeter = i->perimeter();
        double area = i->area();
        i->reflect (Point (10,10));
        i->reflect (Line(0,0));
        i->rotate (Point(10,10), 90);
        assert (perimeter - i->perimeter() < 1e-4);
        assert (area - i->area() < 1e-4);
        assert (!(*i == ta));
    }
}