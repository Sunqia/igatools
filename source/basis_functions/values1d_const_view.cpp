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

#include <igatools/basis_functions/values1d_const_view.h>
#include <igatools/base/exceptions.h>


IGA_NAMESPACE_OPEN


BasisValues1d::
BasisValues1d(const int n_func, const int n_points)
  :
  values_(DenseMatrix(n_func,n_points))
{}

Size
BasisValues1d::
get_num_points() const
{
  return values_[0].size2();
}

Size
BasisValues1d::
get_num_functions() const
{
  return values_[0].size1();
}

void
BasisValues1d::
resize(const int n_funcs, const int n_points)
{
  if (n_funcs  != this->get_num_functions() ||
      n_points != this->get_num_points())
  {
    for (auto &matrix: values_)
    {
      matrix.resize(n_funcs,n_points);
      matrix = 0.0;
    }
  }
}

void
BasisValues1d::
print_info(LogStream &out) const
{
  if (this->get_num_functions() > 0 &&
      this->get_num_points() > 0)
  {
    for (int der_order = 0 ; der_order < MAX_NUM_DERIVATIVES ; ++der_order)
    {
      out.begin_item("Derivative order: " + std::to_string(der_order));
      values_[der_order].print_info(out);
      out.end_item();
    }
  }
  else
  {
    out << "Zero-size object." << std::endl;
  }
}

DenseMatrix &
BasisValues1d::
get_derivative(const int order)
{
  return values_[order];
}

const DenseMatrix &
BasisValues1d::
get_derivative(const int order) const
{
  return values_[order];
}


#if 0
const BasisValues1d *
BasisValues1dConstView::
operator->() const
{
  return funcs_;
}
#endif


Values1DConstView::
Values1DConstView(const DenseMatrix &funcs,const Index func_id)
  :
  funcs_(&funcs),
  func_id_(func_id)
{
  Assert(func_id >= 0 && func_id < Size(funcs_->size1()),
         ExcIndexRange(func_id,0,Size(funcs_->size1())))
}


Size
Values1DConstView::
get_num_points() const
{
  Assert(funcs_ != nullptr,ExcNullPtr());
  return funcs_->size2();
}

Real
Values1DConstView::
operator()(const Index point_id) const
{
  Assert(funcs_ != nullptr,ExcNullPtr());
  Assert(point_id >= 0 && point_id < this->get_num_points(),
         ExcIndexRange(point_id,0,this->get_num_points()));

  return (*funcs_)(func_id_,point_id);
}



IGA_NAMESPACE_CLOSE

