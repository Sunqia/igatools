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

// QualityAssurance: martinelli, 31 Jan 2014


#ifndef STATIC_MULTI_ARRAY_INLINE_H_
#define STATIC_MULTI_ARRAY_INLINE_H_


#include<igatools/utils/static_multi_array.h>
#include <igatools/base/logstream.h>
#include <igatools/base/array_utils.h>

IGA_NAMESPACE_OPEN

template< class T, int dim, int rank >
StaticMultiArray<T,dim,rank>::
StaticMultiArray()
  :
  base_t(TensorSize<rank>(dim))
{
  Assert(this->flat_size() == n_entries, ExcDimensionMismatch(this->flat_size(),n_entries));
}


template< class T, int dim, int rank >
StaticMultiArray<T,dim,rank>::
StaticMultiArray(const T &val)
  :
  StaticMultiArray<T,dim,rank>()
{
  const_cast<SafeSTLArray<T,n_entries>&>(this->get_data()) = SafeSTLArray<T,n_entries>(val);
}


template< class T, int dim, int rank >
StaticMultiArray<T,dim,rank>::
StaticMultiArray(std::initializer_list<T> list)
  :
  StaticMultiArray<T,dim,rank>()
{
  Assert(list.size() == n_entries, ExcDimensionMismatch(list.size(),n_entries));

  for (int i = 0 ; i < n_entries  ; ++i)
    (*this)[i] = list.begin()[i] ;
}



IGA_NAMESPACE_CLOSE



#endif // #ifndef STATIC_MULTI_ARRAY_INLINE_H_
