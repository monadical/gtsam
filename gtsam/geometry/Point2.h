/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation, 
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 * @file    Point2.h
 * @brief   2D Point
 * @author  Frank Dellaert
 */

#pragma once

#include <gtsam/base/VectorSpace.h>
#include <boost/serialization/nvp.hpp>

namespace gtsam {

/**
 * A 2D point
 * Complies with the Testable Concept
 * Functional, so no set functions: once created, a point is constant.
 * @addtogroup geometry
 * \nosubgrouping
 */
class GTSAM_EXPORT Point2 {
private:

  double x_, y_;

public:
  enum { dimension = 2 };
  /// @name Standard Constructors
  /// @{

  /// default constructor
  Point2(): x_(0), y_(0) {}

  /// construct from doubles
  Point2(double x, double y): x_(x), y_(y) {}

  /// @}
  /// @name Advanced Constructors
  /// @{

  /// construct from 2D vector
  Point2(const Vector2& v) {
    x_ = v(0);
    y_ = v(1);
  }

  /*
   * @brief Circle-circle intersection, given normalized radii.
   * Calculate f and h, respectively the parallel and perpendicular distance of
   * the intersections of two circles along and from the line connecting the centers.
   * Both are dimensionless fractions of the distance d between the circle centers.
   * If the circles do not intersect or they are identical, returns boost::none.
   * If one solution (touching circles, as determined by tol), h will be exactly zero.
   * h is a good measure for how accurate the intersection will be, as when circles touch
   * or nearly touch, the intersection is ill-defined with noisy radius measurements.
   * @param R_d : R/d, ratio of radius of first circle to distance between centers
   * @param r_d : r/d, ratio of radius of second circle to distance between centers
   * @param tol: absolute tolerance below which we consider touching circles
   * @return optional Point2 with f and h, boost::none if no solution.
   */
  static boost::optional<Point2> CircleCircleIntersection(double R_d, double r_d,
      double tol = 1e-9);

  /*
   * @brief Circle-circle intersection, from the normalized radii solution.
   * @param c1 center of first circle
   * @param c2 center of second circle
   * @return list of solutions (0,1, or 2). Identical circles will return empty list, as well.
   */
  static std::list<Point2> CircleCircleIntersection(Point2 c1, Point2 c2, boost::optional<Point2>);

  /**
   * @brief Intersect 2 circles
   * @param c1 center of first circle
   * @param r1 radius of first circle
   * @param c2 center of second circle
   * @param r2 radius of second circle
   * @param tol: absolute tolerance below which we consider touching circles
   * @return list of solutions (0,1, or 2). Identical circles will return empty list, as well.
   */
  static std::list<Point2> CircleCircleIntersection(Point2 c1, double r1,
      Point2 c2, double r2, double tol = 1e-9);

  /// @}
  /// @name Testable
  /// @{

  /// print with optional string
  void print(const std::string& s = "") const;

  /// equals with an tolerance, prints out message if unequal
  bool equals(const Point2& q, double tol = 1e-9) const;

  /// @}
  /// @name Group
  /// @{

  /// identity
  inline static Point2 identity() {
    return Point2();
  }

  /// "Inverse" - negates each coordinate such that compose(p,inverse(p)) == identity()
  inline Point2 inverse(OptionalJacobian<2,2> H=boost::none) const {
    if (H) *H = -I_2x2;
    return Point2(-x_, -y_);
  }

  /// syntactic sugar for inverse, i.e., -p == inverse(p)
  inline Point2 operator- () const {return Point2(-x_,-y_);}

  /// "Compose", just adds the coordinates of two points. With optional derivatives
  inline Point2 compose(const Point2& q,
      OptionalJacobian<2,2> H1=boost::none,
      OptionalJacobian<2,2> H2=boost::none) const {
    if(H1) *H1 = I_2x2;
    if(H2) *H2 = I_2x2;
    return *this + q;
  }

  /// syntactic sugar for adding two points, i.e., p+q == compose(p,q)
  inline Point2 operator + (const Point2& q) const {return Point2(x_+q.x_,y_+q.y_);}

  /// "Between", subtracts point coordinates. between(p,q) == compose(inverse(p),q)
  inline Point2 between(const Point2& q,
      OptionalJacobian<2,2> H1=boost::none,
      OptionalJacobian<2,2> H2=boost::none) const {
    if(H1) *H1 = -I_2x2;
    if(H2) *H2 = I_2x2;
    return q - (*this);
  }

  /// syntactic sugar for subtracting points, i.e., q-p == between(p,q)
  inline Point2 operator - (const Point2& q) const {return Point2(x_-q.x_,y_-q.y_);}

  /// @}
  /// @name Manifold
  /// @{

  /// dimension of the variable - used to autodetect sizes
  inline static size_t Dim() { return 2; }

  /// Dimensionality of tangent space = 2 DOF
  inline size_t dim() const { return 2; }

  /// Updates a with tangent space delta
  inline Point2 retract(const Vector& v) const {
    return *this + Point2(v);
  }

  /// Local coordinates of manifold neighborhood around current value
  inline Vector localCoordinates(const Point2& t2) const {
    Point2 dp = t2 - *this;
    return Vector2(dp.x(), dp.y());
  }

  /// @}
  /// @name Lie Group
  /// @{

  /// Exponential map around identity - just create a Point2 from a vector
  static Point2 Expmap(const Vector2& v, OptionalJacobian<2, 2> H) {
    if (H) *H = I_2x2;
    return Point2(v);
  }

  /// Logmap around identity
  static inline Vector2 Logmap(const Point2& dp, OptionalJacobian<2, 2> H) {
    if (H) *H = I_2x2;
    return Vector2(dp.x(), dp.y());
  }

  /// Left-trivialized derivative of the exponential map
  static Matrix2 dexpL(const Vector2& v) {
    return I_2x2;
  }

  /// Left-trivialized derivative inverse of the exponential map
  static Matrix2 dexpInvL(const Vector2& v) {
    return I_2x2;
  }

  /// Updates a with tangent space delta
  inline Point2 retract(const Vector& v, OptionalJacobian<2, 2> H1,
      OptionalJacobian<2, 2> H2 = boost::none) const {
    if (H1) *H1 = I_2x2;
    if (H2) *H2 = I_2x2;
    return *this + Point2(v);
  }

  /// Local coordinates of manifold neighborhood around current value
  inline Vector localCoordinates(const Point2& t2, OptionalJacobian<2, 2> H1,
      OptionalJacobian<2, 2> H2 = boost::none) const {
    if (H1) *H1 = - I_2x2;
    if (H2) *H2 = I_2x2;
    Point2 dp = t2 - *this;
    return Vector2(dp.x(), dp.y());
  }

  /// @}
  /// @name Vector Space
  /// @{

  /** creates a unit vector */
  Point2 unit() const { return *this/norm(); }

  /** norm of point, with derivative */
  double norm(OptionalJacobian<1,2> H = boost::none) const;

  /** distance between two points */
  double distance(const Point2& p2, OptionalJacobian<1,2> H1 = boost::none,
      OptionalJacobian<1,2> H2 = boost::none) const;

  /** @deprecated The following function has been deprecated, use distance above */
  inline double dist(const Point2& p2) const {
    return (p2 - *this).norm();
  }

  /// multiply with a scalar
  inline Point2 operator * (double s) const {return Point2(x_*s,y_*s);}

  /// divide by a scalar
  inline Point2 operator / (double q) const {return Point2(x_/q,y_/q);}

  /// @}
  /// @name Standard Interface
  /// @{

  /// equality
  inline bool operator ==(const Point2& q) const {return x_==q.x_ && q.y_==q.y_;}

  /// get x
  double x() const {return x_;}

  /// get y
  double y() const {return y_;}

  /// return vectorized form (column-wise). TODO: why does this function exist?
  Vector2 vector() const { return Vector2(x_, y_); }

  /// @}
  /// @name Deprecated (non-const, non-functional style. Do not use).
  /// @{
  inline void operator += (const Point2& q) {x_+=q.x_;y_+=q.y_;}
  inline void operator *= (double s) {x_*=s;y_*=s;}
  /// @}

  /// Streaming
  GTSAM_EXPORT friend std::ostream &operator<<(std::ostream &os, const Point2& p);

private:

  /// @name Advanced Interface
  /// @{

  /** Serialization function */
  friend class boost::serialization::access;
  template<class ARCHIVE>
  void serialize(ARCHIVE & ar, const unsigned int version)
  {
    ar & BOOST_SERIALIZATION_NVP(x_);
    ar & BOOST_SERIALIZATION_NVP(y_);
  }

  /// @}

};

/// multiply with scalar
inline Point2 operator*(double s, const Point2& p) {return p*s;}

template<>
struct traits_x<Point2> : public internal::LieGroup<Point2> {};

}

