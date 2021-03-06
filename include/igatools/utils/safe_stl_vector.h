//-+--------------------------------------------------------------------
// Igatools a general purpose Isogeometric analysis library.
// Copyright (C) 2012-2016  by the igatools authors (see authors.txt).
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

#ifndef __SAFE_STL_VECTOR_H_
#define __SAFE_STL_VECTOR_H_

#include <igatools/base/config.h>
#include <igatools/utils/safe_stl_container.h>
#include <vector>

IGA_NAMESPACE_OPEN

/**
 * @brief iga version of std::vector.
 * It can be used as a std::vector but in Debug mode
 * it provides bounds checking.
 *
 * @ingroup serializable
 */
template<class T>
class SafeSTLVector :
  public SafeSTLContainer<std::vector<T>>
{
  using base_t = SafeSTLContainer<std::vector<T>>;
public :
  /** Inherit the constructors of the base class. */
  using SafeSTLContainer<std::vector<T>>::SafeSTLContainer;

  /**
   * Returns a reference to the <tt>n</tt>-th entry of the container.
   * @note In Debug mode the value of <tt>n</tt> is checked if within the valid bounds of the container.
   */
  T &operator[](Size n);

  /**
   * Returns a const-reference to the <tt>n</tt>-th entry of the container.
   * @note In Debug mode the value of <tt>n</tt> is checked if within the valid bounds of the container.
   */
  const T &operator[](Size n) const;


private:
#ifdef IGATOOLS_WITH_SERIALIZATION
  /**
   * @name Functions needed for serialization
   * @see <a href="http://uscilab.github.io/cereal/serialization_functions.html">Cereal serialization</a>
   */
  ///@{
  friend class cereal::access;

  template<class Archive>
  void serialize(Archive &ar)
  {
    ar &make_nvp("SafeSTLContainer_Vector",
                 base_class<std::vector<T>>(this));
  }
  ///@}
#endif // IGATOOLS_WITH_SERIALIZATION

};

template <int> class TensorIndex;

IGA_NAMESPACE_CLOSE


#ifdef IGATOOLS_WITH_SERIALIZATION
using SafeSTLVectorReal = iga::SafeSTLVector<iga::Real>;
CEREAL_SPECIALIZE_FOR_ARCHIVE(IArchive,SafeSTLVectorReal,cereal::specialization::member_serialize)
CEREAL_SPECIALIZE_FOR_ARCHIVE(OArchive,SafeSTLVectorReal,cereal::specialization::member_serialize)

using SafeSTLVectorInt = iga::SafeSTLVector<int>;
CEREAL_SPECIALIZE_FOR_ARCHIVE(IArchive,SafeSTLVectorInt,cereal::specialization::member_serialize)
CEREAL_SPECIALIZE_FOR_ARCHIVE(OArchive,SafeSTLVectorInt,cereal::specialization::member_serialize)


// The next ones are used by SplineSpace
using SafeSTLVecTIAlias0 = iga::SafeSTLVector<iga::TensorIndex<0>>;
CEREAL_SPECIALIZE_FOR_ARCHIVE(IArchive,SafeSTLVecTIAlias0,cereal::specialization::member_serialize)
CEREAL_SPECIALIZE_FOR_ARCHIVE(OArchive,SafeSTLVecTIAlias0,cereal::specialization::member_serialize)
using SafeSTLVecTIAlias1 = iga::SafeSTLVector<iga::TensorIndex<1>>;
CEREAL_SPECIALIZE_FOR_ARCHIVE(IArchive,SafeSTLVecTIAlias1,cereal::specialization::member_serialize)
CEREAL_SPECIALIZE_FOR_ARCHIVE(OArchive,SafeSTLVecTIAlias1,cereal::specialization::member_serialize)
using SafeSTLVecTIAlias2 = iga::SafeSTLVector<iga::TensorIndex<2>>;
CEREAL_SPECIALIZE_FOR_ARCHIVE(IArchive,SafeSTLVecTIAlias2,cereal::specialization::member_serialize)
CEREAL_SPECIALIZE_FOR_ARCHIVE(OArchive,SafeSTLVecTIAlias2,cereal::specialization::member_serialize)
using SafeSTLVecTIAlias3 = iga::SafeSTLVector<iga::TensorIndex<3>>;
CEREAL_SPECIALIZE_FOR_ARCHIVE(IArchive,SafeSTLVecTIAlias3,cereal::specialization::member_serialize)
CEREAL_SPECIALIZE_FOR_ARCHIVE(OArchive,SafeSTLVecTIAlias3,cereal::specialization::member_serialize)


//#include <igatools/utils/safe_stl_vector.serialization>
#endif // IGATOOLS_WITH_SERIALIZATION


#endif // SAFE_STL_VECTOR_H_
