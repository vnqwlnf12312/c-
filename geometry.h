#include <iostream>
#include <vector>
#include <cmath>

namespace geometry {
  bool IsSame(double, double);

  const double epsilon = 1e-8;
}

bool geometry::IsSame(double a, double b) {
  return std::abs(a - b) < geometry::epsilon;
}

struct Point {
 public:
  Point() = default;

  Point(double, double);

  Point(const Point&) = default;

  Point& operator=(const Point&) = default;

  bool operator==(const Point&) const;

  bool operator!=(const Point&) const;

  Point& operator+=(const Point&);

  Point& operator-=(const Point&);

  Point operator-() const;

  Point& operator*=(double);

  Point& operator/=(double);

  Point& rotate(const Point&, double);

  Point& reflect(const Point&);

  Point& scale(const Point&, double);

  double ScalarProd(const Point&) const;

  double VectorProd(const Point&) const;

  double x;
  double y;
};

Point::Point(double x, double y) : x(x), y(y) {
}

bool Point::operator==(const Point& second) const {
  return geometry::IsSame(x, second.x) && geometry::IsSame(y, second.y);
}

bool Point::operator!=(const Point& second) const {
  return !(*this == second);
}

Point& Point::operator+=(const Point& second) {
  x += second.x;
  y += second.y;
  return *this;
}

Point operator+(const Point& first, const Point& second) {
  Point result = first;
  return result += second;
}

Point& Point::operator-=(const Point& second) {
  x -= second.x;
  y -= second.y;
  return *this;
}

Point Point::operator-() const {
  return {-x, -y};
}

Point operator-(const Point& first, const Point& second) {
  Point result = first;
  return result -= second;
}

Point& Point::operator*=(double coefficient) {
  x *= coefficient;
  y *= coefficient;
  return *this;
}

Point operator*(const Point& first, double coefficient) {
  Point result = first;
  return result *= coefficient;
}

Point& Point::operator/=(double coefficient) {
  return *this *= (1 / coefficient);
}

Point operator/(const Point& first, double coefficient) {
  Point result = first;
  return result /= coefficient;
}

Point& Point::rotate(const Point& center_of_rotate, double angle) {
  double new_x, new_y;
  new_x = (x - center_of_rotate.x) * cos(angle)
      - (y - center_of_rotate.y) * sin(angle);
  new_y = (x - center_of_rotate.x) * sin(angle)
      + (y - center_of_rotate.y) * cos(angle);
  *this = Point(new_x, new_y) + center_of_rotate;
  return *this;
}

Point& Point::reflect(const Point& reflect_center) {
  *this = reflect_center * 2 - *this;
  return *this;
}

Point& Point::scale(const Point& center_of_scale, double coefficient) {
  *this = (*this - center_of_scale) * coefficient + center_of_scale;
  return *this;
}

double Point::ScalarProd(const Point& second_point) const {
  return x * second_point.x + y * second_point.y;
}

double Point::VectorProd(const Point& second_point) const {
  return x * second_point.y - y * second_point.x;
}

class Line {
 public:
  Line() = default;

  Line(const Point&, const Point&);

  Line(const double&, const double&);

  Line(const Point&, const double&);

  Line(const Point&);

  bool operator==(const Line&) const;

  bool operator!=(const Line&) const;

  Point Intersection(const Line&) const;

  Point Height(const Point&) const;

  Point& reflect_point(Point&) const;

  Line& rotate(const Point&, double);

  bool IsParallel(const Line&) const;

 protected:
  double A, B, C;
};

Line::Line(const Point& first_point, const Point& second_point) :
    A(second_point.y - first_point.y),
    B(first_point.x - second_point.x),
    C(second_point.x * first_point.y - first_point.x * second_point.y) {
}

Line::Line(const double& slope, const double& shift)
    : A(slope), B(-1), C(shift) {
}

Line::Line(const Point& point, const double& slope)
    : A(slope), B(-1), C(point.y - slope * point.x) {
}

Line::Line(const Point& point) : A(1), B(0), C(-A * point.x) {
}

bool Line::operator==(const Line& second) const {
  return geometry::IsSame(A * second.B, second.A * B)
      && geometry::IsSame(B * second.C, C * second.B);
}

bool Line::operator!=(const Line& second) const {
  return !(*this == second);
}

Line& Line::rotate(const Point& rotate_center, double angle) {
  Point first_point, second_point;
  if (geometry::IsSame(A, 0)) {
    first_point = {1, -C / B};
    second_point = {-1, -C / B};
  } else {
    first_point = {-C / A, 0};
    second_point = {(-C - B) / A, 1};
  }
  first_point.rotate(rotate_center, angle);
  second_point.rotate(rotate_center, angle);
  *this = Line(first_point, second_point);
  return *this;
}

Point Line::Intersection(const Line& second_line) const {
  Point answer;
  answer.y = (A * second_line.C - C * second_line.A)
      / (B * second_line.A - A * second_line.B);
  answer.x = -(B * second_line.C - C * second_line.B)
      / (B * second_line.A - A * second_line.B);
  return answer;
}

Point Line::Height(const Point& point) const {
  Line second_line;
  geometry::IsSame(A, 0) ? second_line = point : second_line = {point, B / A};
  return Intersection(second_line);
}

Point& Line::reflect_point(Point& point) const {
  point = Height(point) * 2 - point;
  return point;
}

bool Line::IsParallel(const Line& second) const {
  return geometry::IsSame(A * second.B - B * second.A, 0);
}

class Shape {
 public:
  virtual double area() const = 0;

  virtual double perimeter() const = 0;

  virtual Shape& rotate(const Point&, double) = 0;

  virtual Shape& reflect(const Point&) = 0;

  virtual Shape& reflect(const Line&) = 0;

  virtual Shape& scale(const Point&, double) = 0;

  bool operator==(const Shape&) const;

  bool operator!=(const Shape&) const;

  virtual bool isCongruentTo(const Shape&) const = 0;

  virtual bool isSimilarTo(const Shape&) const = 0;

  virtual bool containsPoint(const Point&) const = 0;

  virtual ~Shape() = default;
};

class Polygon : public Shape {
 public:
  Polygon() = default;

  Polygon(const std::vector<Point>&);

  template<class... Args>
  Polygon(const Point& p, Args... args);

  size_t verticesCount() const;

  const std::vector<Point>& getVertices() const;

  bool isConvex() const;

  Polygon& rotate(const Point&, double) override;

  Polygon& reflect(const Point&) override;

  Polygon& reflect(const Line&) override;

  Polygon& scale(const Point&, double) override;

  double perimeter() const override;

  double area() const override;

  bool isCongruentTo(const Shape&) const override;

  bool isSimilarTo(const Shape&) const override;

  bool containsPoint(const Point&) const override;

 protected:
  std::vector<Point> vertices_;
};

size_t Polygon::verticesCount() const {
  return vertices_.size();
}

const std::vector<Point>& Polygon::getVertices() const {
  return vertices_;
}

bool Polygon::isConvex() const {
  if (vertices_.size() <= 2) {
    return false;
  }
  bool vertices_on_right = false;
  Point vector1, vector2;
  vector1 = vertices_[1] - vertices_[0];
  vector2 = vertices_[2] - vertices_[1];
  double delta = vector1.VectorProd(vector2);
  if (delta < 0) {
    vertices_on_right = true;
  }
  size_t size = vertices_.size();
  for (size_t i = 1; i < size; ++i) {
    vector1 = vertices_[(i + 1) % size] - vertices_[i];
    vector2 = vertices_[(i + 2) % size] - vertices_[(i + 1) % size];
    delta = vector1.VectorProd(vector2);
    if (delta < 0 == !vertices_on_right) {
      return false;
    }
  }
  return true;
}

Polygon::Polygon(const std::vector<Point>& dots) : vertices_(dots) {
}

template<class... Args>
Polygon::Polygon(const Point& p, Args... args) : Polygon(args...) {
  vertices_.push_back(p);
}

Polygon& Polygon::rotate(const Point& rotate_center, double angle) {
  for (Point& point : vertices_) {
    point.rotate(rotate_center, angle * M_PI / 180);
  }
  return *this;
}

Polygon& Polygon::reflect(const Point& reflect_center) {
  for (Point& point : vertices_) {
    point.reflect(reflect_center);
  }
  return *this;
}

Polygon& Polygon::reflect(const Line& axis) {
  for (Point& point : vertices_) {
    axis.reflect_point(point);
  }
  return *this;
}

Polygon& Polygon::scale(const Point& center_of_scale, double coefficient) {
  for (Point& point : vertices_) {
    point.scale(center_of_scale, coefficient);
  }
  return *this;
}

bool Polygon::isCongruentTo(const Shape& second) const {
  const Polygon* second_copy = dynamic_cast<const Polygon*>(&second);
  if (second_copy == nullptr
      || verticesCount() != second_copy->verticesCount()) {
    return false;
  }
  size_t size2 = second_copy->verticesCount();
  bool is_congruent;
  Point vector1, vector2, vector3, vector4;
  double vector_prod1, vector_prod2;
  for (size_t i = 0; i < size2; ++i) {
    is_congruent = true;
    for (size_t j = 0; j < size2; ++j) {
      vector1 = vertices_[j] - vertices_[(j + size2 - 1) % size2];
      vector2 = vertices_[(j + 1) % size2] - vertices_[j];
      vector3 = second_copy->vertices_[(i + j) % size2]
          - second_copy->vertices_[(i + j + size2 - 1) % size2];
      vector4 = second_copy->vertices_[(i + j + 1) % size2]
          - second_copy->vertices_[(i + j) % size2];
      vector_prod1 = (vector1).VectorProd(vector2);
      vector_prod2 = (vector3).VectorProd(vector4);
      if (!geometry::IsSame(std::abs(vector_prod1
                                         / ((hypot(vector1.x, vector1.y) * hypot(vector2.x, vector2.y)))),
                            std::abs(vector_prod2
                                         / ((hypot(vector3.x, vector3.y) * hypot(vector4.x, vector4.y)))))
          || !geometry::IsSame(hypot(vector1.x, vector1.y), hypot(vector3.x, vector3.y))
          || !geometry::IsSame(hypot(vector2.x, vector2.y),
                               hypot(vector4.x, vector4.y))) {
        is_congruent = false;
        break;
      }
    }
    if (is_congruent) {
      return true;
    }
    is_congruent = true;
    for (size_t j = 0; j < size2; ++j) {
      vector1 = vertices_[j] - vertices_[(j + size2 - 1) % size2];
      vector2 = vertices_[(j + 1) % size2] - vertices_[j];
      vector3 = second_copy->vertices_[(i + size2 - j) % size2]
          - second_copy->vertices_[(i + size2 - j + 1) % size2];
      vector4 = second_copy->vertices_[(i + size2 - j - 1) % size2]
          - second_copy->vertices_[(i + size2 - j) % size2];
      vector_prod1 = (vector1).VectorProd(vector2);
      vector_prod2 = (vector3).VectorProd(vector4);
      if (!geometry::IsSame(std::abs(vector_prod1
                                         / ((hypot(vector1.x, vector1.y) * hypot(vector2.x, vector2.y)))),
                            std::abs(vector_prod2
                                         / ((hypot(vector3.x, vector3.y) * hypot(vector4.x, vector4.y)))))
          || !geometry::IsSame(hypot(vector1.x, vector1.y), hypot(vector3.x, vector3.y))
          || !geometry::IsSame(hypot(vector2.x, vector2.y),
                               hypot(vector4.x, vector4.y))) {
        is_congruent = false;
        break;
      }
    }
    if (is_congruent) {
      return true;
    }
  }
  return false;
}

bool Polygon::isSimilarTo(const Shape& second) const {
  const Polygon* second_copy = dynamic_cast<const Polygon*>(&second);
  if (second_copy == nullptr
      || verticesCount() != second_copy->verticesCount()) {
    return false;
  }
  size_t size2 = second_copy->verticesCount();
  bool is_similar;
  Point vector1, vector2, vector3, vector4;
  double vector_prod1, vector_prod2;
  for (size_t i = 0; i < size2; ++i) {
    is_similar = true;
    for (size_t j = 0; j < size2; ++j) {
      vector1 = vertices_[j] - vertices_[(j + size2 - 1) % size2];
      vector2 = vertices_[(j + 1) % size2] - vertices_[j];
      vector3 = second_copy->vertices_[(i + j) % size2]
          - second_copy->vertices_[(i + j + size2 - 1) % size2];
      vector4 = second_copy->vertices_[(i + j + 1) % size2]
          - second_copy->vertices_[(i + j) % size2];
      vector_prod1 = (vector1).VectorProd(vector2);
      vector_prod2 = (vector3).VectorProd(vector4);
      if (!geometry::IsSame(vector_prod1
                                / ((hypot(vector1.x, vector1.y) * hypot(vector2.x, vector2.y))),
                            vector_prod2
                                / ((hypot(vector3.x, vector3.y) * hypot(vector4.x, vector4.y))))
          || !geometry::IsSame(hypot(vector1.x, vector1.y) / hypot(vector3.x, vector3.y),
                               hypot(vector2.x, vector2.y / hypot(vector4.x, vector4.y)))) {
        is_similar = false;
        break;
      }
    }
    if (is_similar) {
      return true;
    }
    is_similar = true;
    for (size_t j = 0; j < size2; ++j) {
      vector1 = vertices_[j] - vertices_[(j + size2 - 1) % size2];
      vector2 = vertices_[(j + 1) % size2] - vertices_[j];
      vector3 = second_copy->vertices_[(i + size2 - j) % size2]
          - second_copy->vertices_[(i + size2 - j + 1) % size2];
      vector4 = second_copy->vertices_[(i + size2 - j - 1) % size2]
          - second_copy->vertices_[(i + size2 - j) % size2];
      vector_prod1 = (vector1).VectorProd(vector2);
      vector_prod2 = (vector3).VectorProd(vector4);
      if (!geometry::IsSame(vector_prod1 / ((hypot(vector1.x, vector1.y)
                                * hypot(vector2.x, vector2.y))),
                            vector_prod2 / ((hypot(vector3.x, vector3.y)
                                * hypot(vector4.x, vector4.y))))
          || !geometry::IsSame(hypot(vector1.x, vector1.y) / hypot(vector3.x, vector3.y),
                               (hypot(vector2.x, vector2.y)
                                   / hypot(vector4.x, vector4.y)))) {
        is_similar = false;
        break;
      }
    }
    if (is_similar) {
      return true;
    }
  }
  return false;
}

bool Polygon::containsPoint(const Point& point) const {
  size_t size = verticesCount();
  size_t sides_index = 0;
  while (Line(vertices_[sides_index], vertices_[(sides_index + 1) % size])
      == Line(vertices_[sides_index], point)) {
    ++sides_index;
  }
  Point point_on_side =
      (vertices_[sides_index] + vertices_[(sides_index + 1) % size]) / 2;
  Line line_through_point = {point, point_on_side};
  Line side;
  Point intersection;
  bool line_goes_through_vertex;
  while (true) {
    line_goes_through_vertex = false;
    for (size_t i = 0; i < size; ++i) {
      side = {vertices_[i], vertices_[(i + 1) % size]};
      if (!side.IsParallel(line_through_point)) {
        intersection = side.Intersection(line_through_point);
        if (intersection == vertices_[i] || intersection == vertices_[i + 1]) {
          line_goes_through_vertex = true;
          break;
        }
      }
    }
    if (line_goes_through_vertex) {
      point_on_side = (vertices_[sides_index] + point_on_side) / 2;
      line_through_point = {point, point_on_side};
    } else {
      break;
    }
  }
  int counter = 0;
  for (size_t i = 0; i < size; ++i) {
    side = {vertices_[i], vertices_[(i + 1) % size]};
    if (!side.IsParallel(line_through_point)) {
      intersection = side.Intersection(line_through_point);
      if ((intersection - vertices_[i]).ScalarProd(
          intersection - vertices_[(i + 1) % size]) < 0
          && (intersection - point).ScalarProd(point_on_side - point) >= 0) {
        ++counter;
      }
    }
  }
  return counter % 2 == 1;
}

double Polygon::perimeter() const {
  double answer = 0;
  size_t size = vertices_.size();
  for (size_t i = 0; i < size - 1; ++i) {
    answer += std::hypot(vertices_[i + 1].x - vertices_[i].x,
                         vertices_[i + 1].y - vertices_[i].y);
  }
  if (size > 2) {
    answer += std::hypot(vertices_[0].x - vertices_[size - 1].x,
                         vertices_[0].y - vertices_[size - 1].y);
  }
  return answer;
}

double Polygon::area() const {
  double answer = 0;
  size_t size = vertices_.size();
  Point vector1, vector2;
  for (size_t i = 0; i < size - 2; ++i) {
    vector1 = vertices_[i + 1] - vertices_[0];
    vector2 = vertices_[i + 2] - vertices_[0];
    answer += (vector1.x * vector2.y - vector1.y * vector2.x) / 2;
  }
  return std::abs(answer);
}

class Ellipse : public Shape {
 public:
  Ellipse();

  Ellipse(const Point&, const Point&, double);

  std::pair<Point, Point> focuses() const;

  double SemiMajorAxis() const;

  std::pair<Line, Line> directrices() const;

  double eccentricity() const;

  Point center() const;

  Ellipse& rotate(const Point&, double) override;

  Ellipse& reflect(const Point&) override;

  Ellipse& reflect(const Line&) override;

  Ellipse& scale(const Point&, double) override;

  double perimeter() const override;

  double area() const override;

  bool isCongruentTo(const Shape&) const override;

  bool isSimilarTo(const Shape&) const override;

  bool containsPoint(const Point&) const override;

 protected:
  Point focus1_, focus2_;
  double semi_major_axis;
};

Ellipse::Ellipse() : focus1_(0, 0), focus2_(0, 0), semi_major_axis(1) {
}

Ellipse::Ellipse(const Point& focus1_, const Point& focus2_, double sum_len)
    : focus1_(focus1_), focus2_(focus2_), semi_major_axis(sum_len / 2) {
}

std::pair<Point, Point> Ellipse::focuses() const {
  return {focus1_, focus2_};
}

double Ellipse::SemiMajorAxis() const {
  return semi_major_axis;
}

std::pair<Line, Line> Ellipse::directrices() const {
  if (geometry::IsSame(focus1_.y, focus2_.y)) {
    std::pair<Line, Line> answer;
    Point center((focus1_.x + focus2_.x) / 2, focus1_.y);
    answer.first = Point(center.x + semi_major_axis * semi_major_axis * 2
        / (focus1_.x - focus2_.x), center.y);
    answer.second = Point(center.x - semi_major_axis * semi_major_axis * 2
        / (focus1_.x - focus2_.x), center.y);
    return answer;
  }
  std::pair<Line, Line> answer;
  double slope = (focus1_.x - focus2_.x) / (focus2_.y - focus1_.y);
  Point delta = (focus1_ - focus2_) / 2;
  double distance_from_focuses = hypot(delta.x, delta.y);
  double distance_from_dirrectrices = semi_major_axis / eccentricity();
  Point center = (focus1_ + focus2_) / 2;
  answer.first =
      {center + delta * (distance_from_dirrectrices / distance_from_focuses), slope};
  answer.second =
      {center - delta * (distance_from_dirrectrices / distance_from_focuses), slope};
  return answer;
}

double Ellipse::eccentricity() const {
  Point delta = (focus1_ - focus2_) / 2;
  return hypot(delta.x, delta.y) / semi_major_axis;
}

Point Ellipse::center() const {
  return (focus1_ + focus2_) / 2;
}

Ellipse& Ellipse::rotate(const Point& center_of_rotate, double angle) {
  focus1_.rotate(center_of_rotate, angle * M_PI / 180);
  focus2_.rotate(center_of_rotate, angle * M_PI / 180);
  return *this;
}

Ellipse& Ellipse::reflect(const Point& point) {
  focus1_.reflect(point);
  focus2_.reflect(point);
  return *this;
}

Ellipse& Ellipse::reflect(const Line& axis) {
  axis.reflect_point(focus1_);
  axis.reflect_point(focus2_);
  return *this;
}

Ellipse& Ellipse::scale(const Point& center_of_scale, double coefficient) {
  focus1_.scale(center_of_scale, coefficient);
  focus2_.scale(center_of_scale, coefficient);
  semi_major_axis *= coefficient;
  return *this;
}

double Ellipse::perimeter() const {
  return 4 * semi_major_axis * std::comp_ellint_2(eccentricity());
}

double Ellipse::area() const {
  double e = eccentricity();
  return M_PI * semi_major_axis * semi_major_axis
      * sqrt(1 - e * e);
}

bool Ellipse::isCongruentTo(const Shape& second) const {
  const Ellipse* second_copy = dynamic_cast<const Ellipse*>(&second);
  if (second_copy == nullptr) {
    return false;
  }
  Point delta1 = focus1_ - focus2_,
      delta2 = second_copy->focus1_ - second_copy->focus2_;
  return geometry::IsSame(hypot(delta1.x, delta1.y), hypot(delta2.x, delta2.y))
      && geometry::IsSame(semi_major_axis, second_copy->semi_major_axis);
}

bool Ellipse::isSimilarTo(const Shape& second) const {
  const Ellipse* second_copy = dynamic_cast<const Ellipse*>(&second);
  if (second_copy == nullptr) {
    return false;
  }
  Point delta1 = focus1_ - focus2_,
      delta2 = second_copy->focus1_ - second_copy->focus2_;
  if (!hypot(delta2.x, delta2.y)) {
    return !hypot(delta1.x, delta1.y);
  }
  return geometry::IsSame(hypot(delta1.x, delta1.y) / hypot(delta2.x, delta2.y),
                          semi_major_axis / second_copy->semi_major_axis);
}

bool Ellipse::containsPoint(const Point& point) const {
  Point vector1 = point - focus1_, vector2 = point - focus2_;
  return hypot(vector1.x, vector1.y) + hypot(vector2.x, vector2.y)
      <= 2 * semi_major_axis;
}

class Circle : public Ellipse {
 public:
  Circle() = default;

  Circle(const Point&, double);

  double radius() const;

  double perimeter() const override;

  double area() const override;
};

Circle::Circle(const Point& center, double radius) : Ellipse(center, center, 2 * radius) {
}

double Circle::radius() const {
  return semi_major_axis;
}

double Circle::perimeter() const {
  return 2 * M_PI * semi_major_axis;
}

double Circle::area() const {
  return M_PI * semi_major_axis * semi_major_axis;
}

class Rectangle : public Polygon {
 public:
  Rectangle() = default;

  Rectangle(const Point&, const Point&, double);

  Point center() const;

  std::pair<Line, Line> diagonals() const;

  double perimeter() const override;

  double area() const override;
};

Point Rectangle::center() const {
  return (vertices_[0] + vertices_[2]) / 2;
}

std::pair<Line, Line> Rectangle::diagonals() const {
  return {{vertices_[0], vertices_[2]}, {vertices_[1], vertices_[3]}};
}

Rectangle::Rectangle(const Point& first_dot,
                     const Point& second_dot,
                     double quotient) {
  vertices_.resize(4);
  vertices_[0] = first_dot;
  vertices_[2] = second_dot;
  if (quotient < 1) {
    quotient = 1 / quotient;
  }
  Point delta = center() - first_dot;
  delta.rotate({0, 0}, 2 * atan(quotient));
  vertices_[1] = center() + delta;
  vertices_[3] = center() - delta;
}

double Rectangle::perimeter() const {
  Point vector1 = vertices_[1] - vertices_[0],
      vector2 = vertices_[2] - vertices_[1];
  return 2 * (hypot(vector1.x, vector1.y) + hypot(vector2.x, vector2.y));
}

double Rectangle::area() const {
  Point vector1 = vertices_[1] - vertices_[0],
      vector2 = vertices_[2] - vertices_[1];
  return hypot(vector1.x, vector1.y) * hypot(vector2.x, vector2.y);
}

class Square : public Rectangle {
 public:
  Square() = default;

  Square(const Point&, const Point&);

  Circle circumscribedCircle();

  Circle inscribedCircle();
};

Circle Square::circumscribedCircle() {
  Point diagonal = vertices_[2] - vertices_[0];
  return {center(), hypot(diagonal.x, diagonal.y) / 2};
}

Circle Square::inscribedCircle() {
  Point side = vertices_[1] - vertices_[0];
  return {center(), hypot(side.x, side.y) / 2};
}

Square::Square(const Point& first_point, const Point& second_point) : Rectangle(
    first_point,
    second_point,
    1) {
}

class Triangle : public Polygon {
 public:
  Triangle(const Point&, const Point&, const Point&);

  Triangle() = default;

  Circle circumscribedCircle();

  Circle inscribedCircle();

  Point centroid();

  Point orthocenter();

  Line EulerLine();

  Circle ninePointsCircle();
};

Triangle::Triangle(const Point& first,
                   const Point& second,
                   const Point& third) : Polygon(first, second, third) {
}

Circle Triangle::circumscribedCircle() {
  Point mid1 = (vertices_[0] + vertices_[1]) / 2,
      mid2 = (vertices_[1] + vertices_[2]) / 2;
  Line
      midperpendicular1 = Line(vertices_[0], vertices_[1]).rotate(mid1, M_PI / 2);
  Line
      midperpendicular2 = Line(vertices_[1], vertices_[2]).rotate(mid2, M_PI / 2);
  Point center = midperpendicular1.Intersection(midperpendicular2);
  Point radius = center - vertices_[0];
  return {center, hypot(radius.x, radius.y)};
}

Circle Triangle::inscribedCircle() {
  Point vector1 = vertices_[1] - vertices_[0],
      vector2 = vertices_[2] - vertices_[0], vector3 = vertices_[2] - vertices_[1];
  double angle1 = asin(vector1.VectorProd(vector2)
                           / (hypot(vector1.x, vector1.y) * hypot(vector2.x, vector2.y)));
  double angle2 = asin(vector3.VectorProd(-vector1)
                           / (hypot(vector3.x, vector3.y) * hypot(vector1.x, vector1.y)));
  angle1 /= 2;
  angle2 /= 2;
  Line line1 = Line(vertices_[0], vertices_[1]).rotate(vertices_[0], angle1);
  Line line2 = Line(vertices_[1], vertices_[2]).rotate(vertices_[1], angle2);
  return {line1.Intersection(line2), 2 * area() / perimeter()};
}

Point Triangle::centroid() {
  return (vertices_[0] + vertices_[1] + vertices_[2]) / 3;
}

Point Triangle::orthocenter() {
  Line line1 =
      {vertices_[0], Line(vertices_[1], vertices_[2]).Height(vertices_[0])};
  Line line2 =
      {vertices_[1], Line(vertices_[2], vertices_[0]).Height(vertices_[1])};
  return line1.Intersection(line2);
}

Line Triangle::EulerLine() {
  return {centroid(), orthocenter()};
}

Circle Triangle::ninePointsCircle() {
  Point mid1 = (vertices_[0] + vertices_[1]) / 2;
  Point mid2 = (vertices_[1] + vertices_[2]) / 2;
  Point mid3 = (vertices_[0] + vertices_[2]) / 2;
  return Triangle(mid1, mid2, mid3).circumscribedCircle();
}

bool PolygonsEqual(const Polygon* this_copy, const Shape& second) {
  const Polygon* second_copy = dynamic_cast<const Polygon*> (&second);
  if (second_copy == nullptr
      || this_copy->verticesCount() != second_copy->verticesCount()) {
    return false;
  }
  size_t size2 = second_copy->verticesCount();
  bool is_same;
  for (size_t i = 0; i < size2; ++i) {
    is_same = true;
    for (size_t j = 0; j < size2; ++j) {
      if (this_copy->getVertices()[j]
          != second_copy->getVertices()[(i + j) % size2]) {
        is_same = false;
        break;
      }
    }
    if (is_same) {
      return true;
    }
    is_same = true;
    for (size_t j = 0; j < size2; ++j) {
      if (this_copy->getVertices()[j]
          != second_copy->getVertices()[(i + size2 - j) % size2]) {
        is_same = false;
        break;
      }
    }
    if (is_same) {
      return true;
    }
  }
  return false;
}

bool EllipsesEqual(const Ellipse* this_copy, const Shape& second) {
  const Ellipse* second_copy = dynamic_cast<const Ellipse*> (&second);
  if (second_copy == nullptr) {
    return false;
  }
  return this_copy->focuses().first == second_copy->focuses().first
      && this_copy->focuses().second == second_copy->focuses().second
      && geometry::IsSame(this_copy->SemiMajorAxis(), second_copy->SemiMajorAxis());
}

bool Shape::operator==(const Shape& second) const {
  const Polygon* this_copy = dynamic_cast<const Polygon*>(this);
  if (this_copy != nullptr) {
    return PolygonsEqual(this_copy, second);
  }
  const Ellipse* this_copy2 = dynamic_cast<const Ellipse*>(this);
  if (this_copy2 != nullptr) {
    return EllipsesEqual(this_copy2, second);
  }
  return false;
}

bool Shape::operator!=(const Shape& second) const {
  return !(*this == second);
}
