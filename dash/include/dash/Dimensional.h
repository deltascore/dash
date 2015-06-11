#ifndef DASH__DIMENSIONAL_H_
#define DASH__DIMENSIONAL_H_

#include <assert.h>
#include <array>

#include <dash/Enums.h>
#include <dash/Cartesian.h>
#include <dash/Team.h>
#include <dash/Exception.h>

namespace dash {

/**
 * Base class for dimensional attributes, stores an
 * n-dimensional value with identical type all dimensions.
 *
 * \tparam  T  The contained value type
 * \tparam  NumDimensions  The number of dimensions
 */
template<typename T, size_t NumDimensions>
class Dimensional {
/* 
 * Concept Dimensional:
 *   + Dimensional<T,D>::Dimensional(values[D]);
 *   + Dimensional<T,D>::dim(d | 0 < d < D);
 *   + Dimensional<T,D>::[d](d | 0 < d < D);
 */
protected:
  T _values[NumDimensions];

public:
  /**
   * Constructor, expects one value for every dimension.
   */
  template<typename ... Values>
  Dimensional(Values ... values)
  : _values { T(values)... } {
    static_assert(
      sizeof...(values) == NumDimensions,
      "Invalid number of arguments");
  }

  /**
   * Copy-constructor.
   */
  Dimensional(const Dimensional<T, NumDimensions> & other) {
    for (int d = 0; d < NumDimensions; ++d) {
      _values[d] = other._values[d];
    }
  }

  /**
   * The value in the given dimension.
   *
   * \param  dimension  The dimension
   * \returns  The value in the given dimension
   */
  T dim(size_t dimension) const {
    if (dimension >= NumDimensions) {
      DASH_THROW(
        dash::exception::OutOfBounds,
        "Dimension for Dimensional::extent() must be lower than " <<
        NumDimensions);
    }
    return _values[dimension];
  }

  /**
   * Subscript operator, access to value in dimension given by index.
   * Alias for \c dim.
   *
   * \param  dimension  The dimension
   * \returns  The value in the given dimension
   */
  T operator[](size_t dimension) const {
    return dim(dimension);
  }

  /**
   * Subscript assignment operator, access to value in dimension given by index.
   * Alias for \c dim.
   *
   * \param  dimension  The dimension
   * \returns  A reference to the value in the given dimension
   */
  T & operator[](size_t dimension) {
    return _values[index];
  }

  /**
   * The number of dimensions of the value.
   */
  size_t rank() const {
    return NumDimensions;
  }

protected:
  /// Prevent default-construction.
  Dimensional() {
  }
};

/**
 * DistributionSpec describes distribution patterns of all dimensions,
 * \see dash::DistEnum.
 */
template<size_t NumDimensions>
class DistributionSpec : public Dimensional<DistEnum, NumDimensions> {
public:
  /**
   * Default constructor, initializes default blocked distribution 
   * (BLOCKED, NONE*).
   */
  DistributionSpec() {
    this->_values[0] = BLOCKED;
    for (size_t i = 1; i < NumDimensions; i++) {
      this->_values[i] = NONE;
    }
  }

  template<typename T_, typename ... values>
  DistributionSpec(T_ value, values ... Values)
  : Dimensional<DistEnum, NumDimensions>::Dimensional(value, Values...) {
  }
};

/** 
 * TeamSpec specifies the arrangement of team units in all dimensions.
 * Size of TeamSpec implies the size of the team.
 * 
 * Reoccurring units are currently not supported.
 *
 * \tparam  NumDimensions  Number of dimensions
 */
template<size_t MaxDimensions>
class TeamSpec : public Dimensional<size_t, MaxDimensions> {
public:
  TeamSpec(Team & team = dash::Team::All()) {
    for (size_t i = 0; i < MaxDimensions; i++) {
      this->_values[i] = 1;
    }
    _num_units      = team.size();
    _num_dimensions = 1;
    this->_values[MaxDimensions - 1] = _num_units;
  }

  template<typename ExtentType, typename ... values>
  TeamSpec(ExtentType value, values ... Values)
  : Dimensional<size_t, MaxDimensions>::Dimensional(value, Values...) {
  }

  size_t rank() const {
    return _num_dimensions;
  }

  size_t size() const {
    return _num_units;
  }

private:
  /// Actual number of dimensions of the team layout specification.
  size_t _num_dimensions;
  /// Number of units in the team layout specification.
  size_t _num_units;
};

/**
 * Specifies an cartesian extent in a specific number of dimensions.
 */
template<size_t NumDimensions>
class SizeSpec : public Dimensional<long long, NumDimensions> {
  /* 
   * TODO: Define concept MemoryLayout<MemArrange> : CartCoord<MemArrange>
   * Concept SizeSpec:
   *   + SizeSpec::SizeSpec(extents ...);
   *   + SizeSpec::extent(dim);
   *   + SizeSpec::size();
   */
public:
  template<typename ... Values>
  SizeSpec(Values ... values)
  : Dimensional<long long, NumDimensions>::Dimensional(values...),
    _size(1) {
    for (int d = 0; d < NumDimensions; ++d) {
      _size *= this->dim(d);
    }
    if (_size == 0) {
      DASH_THROW(
        dash::exception::InvalidArgument,
        "Extents for SizeSpec::SizeSpec() must be non-zero");
    }
  }

  /**
   * The size (extent) in the given dimension.
   *
   * \param  dimension  The dimension
   * \returns  The extent in the given dimension
   */
  long long extent(size_t dimension) const {
    return this->dim(dimension);
  }

  std::array<long long, NumDimensions> extents() const {
    std::array<long long, NumDimensions> ext { (long long)(this->_values) };
    return ext;
  }

  /**
   * The total size (volume) of all dimensions, e.g. \c (x * y * z).
   *
   * \returns  The total size of all dimensions.
   */
  size_t size() const {
    return _size;
  }

private:
  /// Prevent default-construction.
  SizeSpec() = delete;

private:
  size_t _size;
};

class ViewPair {
  long long offset;
  size_t extent;
};

/**
 * Specifies view parameters for implementing submat, rows and cols
 */
template<size_t NumDimensions>
class ViewSpec : public Dimensional<ViewPair, NumDimensions> {
#if 0
public:
  void update_size() {
    nelem = 1;

    for (size_t i = NumDimensions - view_dim; i < NumDimensions; i++) {
      assert(range[i] > 0);
      nelem *= range[i];
    }
  }
#endif
public:
  ViewSpec()
  : _size(0) {
    for (size_t i = 0; i < NumDimensions; i++) {
      _offset[i] = 0;
      _extent[i] = 0;
    }
  }

  ViewSpec(SizeSpec<NumDimensions> sizespec)
  : Dimensional<ViewPair, NumDimensions>(sizespec),
    _size(sizespec.size()) {
    for (size_t i = 0; i < NumDimensions; i++) {
      _offset[i] = 0;
      _extent[i] = sizespec.extent(i);
    }
  }

  template<typename T_, typename ... Values>
  ViewSpec(Values... values)
  : Dimensional<ViewPair, NumDimensions>::Dimensional(values...),
    _size(1) {
    for (size_t i = 0; i < NumDimensions; i++) {
      ViewPair vp = this->dim(i);
      _offset[i] = vp.offset;
      _extent[i] = vp.extent;
      _size *= vp.extent;
    }
  }

  /**
   * Change the view specification's extent in every dimension.
   */
  template<typename... Args>
  void resize(Args... args) {
    static_assert(
      sizeof...(Args) == NumDimensions,
      "Invalid number of arguments");
    std::array<size_t, NumDimensions> extents = { (size_t)(args)... };
    resize(extents);
  }

  /**
   * Change the view specification's extent in every dimension.
   */
  template<typename SizeType_>
  void resize(std::array<SizeType_, NumDimensions> extent) {
    _size = 1;
    for (size_t i = 0; i < NumDimensions; i++) {
      _extent[i] = extent[i];
      _size *= extent[i];
    }
    if (_size <= 0) {
      DASH_THROW(
        dash::exception::InvalidArgument,
        "Extents for ViewSpec::resize must be greater than 0");
    }
  }

  long long begin(size_t dimension) const {
    return _offset[dimension];
  }

  long long size() const {
    return _size;
  }

  long long size(size_t dimension) const {
    return _extent[dimension];
  }

private:
  long long _offset[NumDimensions];
  size_t _extent[NumDimensions];
  size_t _size;
};

} // namespace dash

#endif // DASH__DIMENSIONAL_H_
