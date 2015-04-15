//-+--------------------------------------------------------------------
// Igatools a general purpose Isogeometric analysis library.
// Copyright (C) 2012-2015  by the igatools authors (see authors.txt).
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


#include <igatools/base/function_element.h>


IGA_NAMESPACE_OPEN


template<int dim, int codim, int range, int rank>
FunctionElement<dim, codim, range, rank>::
FunctionElement(const FunctionElement<dim,codim,range,rank> &elem,
                const CopyPolicy &copy_policy)
    :
    CartesianGridElement<dim>(elem,copy_policy),
    func_(elem.func_)
{
    if (copy_policy == CopyPolicy::shallow)
        local_cache_ = elem.local_cache_;
    else
        local_cache_ = std::shared_ptr<LocalCache<Cache>>(new LocalCache<Cache>(*elem.local_cache_));
}


IGA_NAMESPACE_CLOSE

