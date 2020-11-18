#include "geometry.h"

using std::cout;
using std::endl;

int main(){
    Vector v = {1,1};
    cout << v << endl;

    Position p = {2,2};
    cout << p << endl;

    Rectangle r = {3,3,p};
    cout << r << endl;

    Rectangles rs({{1,1,{2,2}},r});
    cout << rs << endl;

    p += v;
    cout << p << endl;

    v += v;
    cout << v << endl;

    p = v + p;
    cout << p << endl;

    p = p + v;
    cout << p << endl;

    v = v + v;
    cout << v << endl;

    r = r + v;
    cout << r << endl;

    r = v + r;
    cout << r << endl;

    rs = rs + v;
    cout << rs << endl;

    rs = v + rs;
    cout << rs << endl;

    Position p1(1,2);
    Vector v1(p1);
    Vector v2(2,3);
    Position p2(v2);
    cout << p1 << endl;
    cout << v1 << endl;
    cout << p2 << endl;
    cout << v2 << endl;

    p1 = Position(v2);
    v1 = Vector(p2);
    cout << p1 << endl;
    cout << v1 << endl;
    cout << p2 << endl;
    cout << v2 << endl;
    
    Rectangles recs1({r,r,r});
    Rectangles recs2 = recs1 + Vector(1,1);
    cout << recs1 << endl << recs2 << endl;

    Position err(INT32_MAX, INT32_MAX);
    err += {1, 1};
}