/***********************************************************************/
/*  Copyright 2015 ToPS                                                */
/*                                                                     */
/*  This program is free software; you can redistribute it and/or      */
/*  modify it under the terms of the GNU  General Public License as    */
/*  published by the Free Software Foundation; either version 3 of     */
/*  the License, or (at your option) any later version.                */
/*                                                                     */
/*  This program is distributed in the hope that it will be useful,    */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of     */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      */
/*  GNU General Public License for more details.                       */
/*                                                                     */
/*  You should have received a copy of the GNU General Public License  */
/*  along with this program; if not, write to the Free Software        */
/*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,         */
/*  MA 02110-1301, USA.                                                */
/***********************************************************************/

#ifndef TOPS_MODEL_RANGE_
#define TOPS_MODEL_RANGE_

// Standard headers
#include <memory>

namespace tops {
namespace model {

// Forward declaration
struct Range;

/**
 * @typedef RangePtr
 * @brief Alias of pointer to Range.
 */
using RangePtr = std::shared_ptr<Range>;

/**
 * @class Range
 * @brief TODO
 */
struct Range {
  // Inner classes
  struct iterator_type {
    // Aliases
    using value_type = size_t;
    using difference_type = size_t;
    using iterator_category = std::forward_iterator_tag;
    using pointer = const value_type *;
    using reference = const value_type &;

    // Instance variables
    mutable value_type value_;
    const difference_type increment_;

    // Constructors
    explicit iterator_type(value_type value, difference_type increment = 0)
      : value_(value), increment_(increment) {}

    // Concrete methods
    const iterator_type& operator++() const {
      value_ += increment_;
      return *this;
    }
    const iterator_type operator++(int) const {
      iterator_type tmp(*this);
      operator++();
      return tmp;
    }

    value_type& operator*() const { return value_; }

    bool operator==(const iterator_type &rhs) const {
      return value_ == rhs.value_;
    }
    bool operator!=(const iterator_type &rhs) const {
      return !operator==(rhs);
    }
  };

  // Aliases
  using iterator = iterator_type;
  using const_iterator = const iterator_type;

  // Instance variables
  size_t first_, last_, increment_;

  // Constructors
  explicit Range(size_t last)
    : first_(0), last_(last), increment_(1) {}
  Range(size_t first, size_t last, size_t increment = 1)
    : first_(first), last_(last), increment_(increment) {}

  // Concrete methods
  iterator begin() const { return iterator(first_, increment_); }
  iterator end() const { return iterator(last_); }
};

}  // namespace model
}  // namespace tops

#endif  // TOPS_MODEL_RANGE_
