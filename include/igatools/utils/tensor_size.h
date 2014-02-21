//-+--------------------------------------------------------------------
// Igatools a general purpose Isogeometric analysis library.
// Copyright (C) 2012-2014  by the igatools authors (see authors.txt).
//
// This file is part of the igatools library.
//
// The igatools library is free software: you can use it, redistribute
// it and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation, either
// version 3 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//-+--------------------------------------------------------------------


#ifndef TENSOR_SIZE_H_
#define TENSOR_SIZE_H_

#include <igatools/base/config.h>

// QualityAssurance: martinelli, 21 Jan 2014

IGA_NAMESPACE_OPEN


/**
 * @brief Type for the size of a tensor-like container.
 *
 * It is a list of rank number of sizes.
 *
 * @author M. Martinelli
 * @date 21 Jan 2014
 */
template <int rank>
class TensorSize
{
public:

    /** @name Constructors */
    ///@{

    /** Default constructor. Initializes all the direction indices to zero. */
    TensorSize(const Size value = 0) noexcept ;

    /** Constructor using an std::array. */
    TensorSize(const std::array<Size,rank> &arr) noexcept;

    /** Constructor using an initializer-list. */
    TensorSize(std::initializer_list<Size> list) noexcept;

    /** Copy constructor. */
    TensorSize(const TensorSize<rank> &arr) = default;

    /** Move constructor. */
    TensorSize(TensorSize<rank> &&arr) = default;

    /** Destructor. */
    ~TensorSize() = default;
    ///@}

    /** @name Assignment operators */
    ///@{

    /**
     * Copy assignment operator
     */
    TensorSize<rank> &operator=(const TensorSize<rank> &arr) = default;

    /**
     * Move assignment operator
     */
    TensorSize<rank> &operator=(TensorSize<rank> &&arr) = default;

    ///@}


    /** @name Access (read/write) operators */
    ///@{
    /**
     * Read/write operator. Returns the reference to the i-th size.
     * @note In Debug mode the index @p i is checked in order to be
     * in the bounds of the array containing the different direction sizes.
     */
    Size &operator()(const Index i);

    /**
     * Read operator. Returns the const-reference to the i-th size.
     * @note In Debug mode the index @p i is checked in order to be
     * in the bounds of the array containing the different direction sizes.
     */
    const Size &operator()(const Index i) const;
    ///@}

    /**
     * Return the flat size, i.e. the multiplications of the sizes along each direction.
     */
    Size flat_size() const noexcept ;

private:

    /** Size along each direction. */
    std::array<Size,rank> size_directions_;
};


/**
 * Equality operator.
 * Returns true if @p size1 and @size2 are entry-by-entry equal.
 */
template <int rank>
bool
operator==(const TensorSize<rank> &size1,const TensorSize<rank> &size2);



IGA_NAMESPACE_CLOSE



#endif // #ifndef TENSOR_SIZE_H_
