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

#ifndef __EPETRA_VECTOR_H_
#define __EPETRA_VECTOR_H_

#include <igatools/base/config.h>
#include <igatools/linear_algebra/epetra_map.h>
#include <igatools/linear_algebra/dense_vector.h>
#include <igatools/utils/safe_stl_vector.h>
#include <igatools/base/properties.h>

#ifdef IGATOOLS_USES_TRILINOS
#include <Epetra_Vector.h>
#endif //IGATOOLS_USES_TRILINOS

IGA_NAMESPACE_OPEN

#ifdef IGATOOLS_USES_TRILINOS

namespace EpetraTools
{

class  Vector : public Epetra_Vector
{
public:
  using Epetra_Vector::Epetra_Vector;

  Size size() const;

  Vector &operator +=(const Vector &vec);

  void add_block(const SafeSTLVector<Index> &vec_id,
                 const DenseVector &local_vector);

  //TODO (pauletti, Apr 3, 2015): both SafeSTLVector<Real> and std::vector<Index>
  // should be replace by a typedef and a proper type for fast communication with LA
  SafeSTLVector<Real>
  get_local_coeffs(const std::vector<Index> &global_ids) const;

  void print_info(LogStream &out) const;

};

using VectorPtr = std::shared_ptr<Vector>;

VectorPtr create_vector(const Map &map);

template <class Basis>
VectorPtr
create_vector(const Basis &basis, const std::string &prop,const Comm &comm)
{
  return create_vector(*create_map(basis, prop, comm));
}

}

#endif //IGATOOLS_USES_TRILINOS

IGA_NAMESPACE_CLOSE

#endif
