#include <gtest/gtest.h>

#include <stk_search/BoundingBox.hpp>

#include <string>
#include <iostream>

namespace {


TEST( stk_search_bounding_box, Point)
{
  using namespace stk::search;

  Point<double> p;

  {
    std::ostringstream out;
    out << p;
    EXPECT_EQ( out.str(), std::string("(0,0,0)"));
  }

  p = Point<double>(1,2,3);

  EXPECT_EQ(p, Point<double>(1,2,3));
  EXPECT_NE(p, Point<double>());
}


TEST( stk_search_bounding_box, Sphere)
{
  typedef stk::search::Sphere<double> Sphere;
  typedef stk::search::Point<double> Point;

  using stk::search::is_valid;

  Sphere s;
  EXPECT_FALSE( is_valid(s));

  {
    std::ostringstream out;
    out << s;
    EXPECT_EQ( out.str(), std::string("{(0,0,0):-1}"));
  }

  s = Sphere(Point(1,2,3),4);

  EXPECT_EQ( s.center(), Point(1,2,3) );
  EXPECT_EQ( s.radius(), 4 );

  EXPECT_TRUE( is_valid(s));
  EXPECT_EQ(s, Sphere(Point(1,2,3),4));
  EXPECT_NE(s, Sphere());
}


TEST( stk_search_bounding_box, Box)
{
  typedef stk::search::Box<double> Box;
  typedef stk::search::Point<double> Point;

  using stk::search::is_valid;

  Box b;
  EXPECT_FALSE( is_valid(b));

  {
    std::ostringstream out;
    out << b;
    EXPECT_EQ( out.str(), std::string("{(1,1,1)->(0,0,0)}"));
  }

  b = Box(Point(0,0,0),Point(1,1,1));

  EXPECT_EQ( b.min_corner(), Point(0,0,0) );
  EXPECT_EQ( b.max_corner(), Point(1,1,1) );

  EXPECT_TRUE( is_valid(b));
  EXPECT_EQ(b, Box(Point(0,0,0),Point(1,1,1)));
  EXPECT_NE(b, Box());
}

TEST( stk_search_bounding_box, min_max_center)
{
  typedef stk::search::Sphere<double> Sphere;
  typedef stk::search::Box<double> Box;
  typedef stk::search::Point<double> Point;

  using stk::search::min_corner;
  using stk::search::max_corner;
  using stk::search::center;

  Point p(0,0,0);
  EXPECT_EQ( Point(0,0,0), min_corner(p) );
  EXPECT_EQ( Point(0,0,0), max_corner(p) );
  EXPECT_EQ( Point(0,0,0), center(p) );

  Box b(Point(0,0,0),Point(1,1,1));
  EXPECT_EQ( Point(0,0,0), min_corner(b) );
  EXPECT_EQ( Point(1,1,1), max_corner(b) );
  EXPECT_EQ( Point(0.5,0.5,0.5), center(b) );

  Sphere s(Point(1,1,1),1.5);
  EXPECT_EQ( Point(-0.5, -0.5, -0.5), min_corner(s) );
  EXPECT_EQ( Point(2.5, 2.5, 2.5), max_corner(s) );
  EXPECT_EQ( Point(1, 1, 1), center(s) );

}

TEST( stk_search_bounding_box, intersects)
{
  typedef stk::search::Sphere<double> Sphere;
  typedef stk::search::Box<double> Box;
  typedef stk::search::Point<double> Point;

  using stk::search::intersects;

  //Point,Point
  {
    Point a(0,0,0);
    Point b(0,0,0);
    EXPECT_TRUE( intersects(a,b) );
    a[0] = 1;
    EXPECT_FALSE( intersects(a,b) );
  }

  //Point,Sphere
  {
    Point a(0,0,0);
    Sphere b(Point(0,0,0),1);
    EXPECT_TRUE( intersects(a,b) );
    EXPECT_TRUE( intersects(b,a) );
    a[0] = 2;
    EXPECT_FALSE( intersects(a,b) );
    EXPECT_FALSE( intersects(b,a) );
  }

  //Point,Box
  {
    Point a(0.5,0.5,0.5);
    Box b(Point(0,0,0),Point(1,1,1));
    EXPECT_TRUE( intersects(a,b) );
    EXPECT_TRUE( intersects(b,a) );
    a[0] = 2;
    EXPECT_FALSE( intersects(a,b) );
    EXPECT_FALSE( intersects(b,a) );
  }

  //Sphere,Sphere
  {
    Sphere a(Point(0,0,0),2);
    Sphere b(Point(1,1,1),2);
    EXPECT_TRUE( intersects(a,b) );
    EXPECT_TRUE( intersects(b,a) );
    a.set_center(Point(-3,-3,-3));
    EXPECT_FALSE( intersects(a,b) );
    EXPECT_FALSE( intersects(b,a) );
  }

  //Sphere,Box
  {
    Sphere a(Point(0,0,0),1);
    Box b(Point(.3,.3,.3),Point(2,2,2));
    EXPECT_TRUE( intersects(a,b) );
    EXPECT_TRUE( intersects(b,a) );
    b.set_min_corner(Point(.9,.9,.9));
    EXPECT_FALSE( intersects(a,b) );
    EXPECT_FALSE( intersects(b,a) );
  }

  //Box,Box
  {
    Box a(Point(0,0,0),Point(1.5,1.5,1.5));
    Box b(Point(1,1,1),Point(3,3,3));
    EXPECT_TRUE( intersects(a,b) );
    EXPECT_TRUE( intersects(b,a) );
    b.set_min_corner(Point(2,1,1));
    EXPECT_FALSE( intersects(a,b) );
    EXPECT_FALSE( intersects(b,a) );
  }
}


} // unnamed namespace
