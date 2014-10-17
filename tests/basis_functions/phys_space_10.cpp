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

/*
 *  Test for "New" physical space
 *
 *  author: pauletti
 *  date: Oct 08, 2014
 *
 */

#include "../tests.h"

#include <igatools/base/quadrature_lib.h>
#include <igatools/geometry/identity_mapping.h>

#include <igatools/basis_functions/bspline_element_accessor.h>
#include <igatools/geometry/push_forward_element_accessor.h>

#include <igatools/basis_functions/space_uniform_quad_cache.h>
#include <igatools/basis_functions/physical_space_element_accessor.h>




template <int dim, int range=1, int rank=1>
void cache_init(const ValueFlags flag,
                const int n_knots = 5, const int deg=1)
{
    OUTSTART

    using RefSpace = BSplineSpace<dim, range, rank>;
    using PF = PushForward<Transformation::h_grad,dim,0>;
    using Space = PhysicalSpace<RefSpace, PF>;

    auto grid  = CartesianGrid<dim>::create(n_knots);
    auto ref_space = RefSpace::create(deg, grid);
    auto map = IdentityMapping<dim>::create(grid);
    auto push_forward = PF::create(map);
    auto space = Space::create(ref_space, push_forward);
    auto quad = QGauss<dim>(2);

    SpaceUniformQuadCache<Space> cache(space, flag, quad);
    cache.print_info(out);

    OUTEND
}

#if 0

template <int dim, int range=1, int rank=1>
void cache_init_elem(const ValueFlags flag,
                     const int n_knots = 5, const int deg=1)
{
    OUTSTART

    using RefSpace = BSplineSpace<dim, range, rank>;
    using PF = PushForward<Transformation::h_grad,dim,0>;
    using Space = PhysicalSpace<RefSpace, PF>;

    auto grid  = CartesianGrid<dim>::create(n_knots);
    auto ref_space = RefSpace::create(deg, grid);
    auto map = IdentityMapping<dim>::create(grid);
    auto push_forward = PF::create(map);
    auto space = Space::create(ref_space, push_forward);
    auto quad = QGauss<dim>(2);

    SpaceUniformQuadCache<Space> cache(space, flag, quad);

    auto elem = space->begin();
    cache.init_element_cache(elem);
    elem->print_cache_info(out);

    OUTEND
}


template <int dim, int range=1, int rank=1>
void cache_fill_elem(const ValueFlags flag,
                     const int n_knots = 5, const int deg=1)
{
    OUTSTART

    using RefSpace = BSplineSpace<dim, range, rank>;
    using PF = PushForward<Transformation::h_grad,dim,0>;
    using Space = PhysicalSpace<RefSpace, PF>;

    auto grid  = CartesianGrid<dim>::create(n_knots);
    auto ref_space = RefSpace::create(deg, grid);
    auto map = IdentityMapping<dim>::create(grid);
    auto push_forward = PF::create(map);
    auto space = Space::create(ref_space, push_forward);
    auto quad = QGauss<dim>(2);

    SpaceUniformQuadCache<Space> cache(space, flag, quad);

    auto elem = space->begin();
    auto end = space->end();
    cache.init_element_cache(elem);
    for (; elem != end; ++elem)
    {
        cache.fill_element_cache(elem);
        elem->print_cache_info(out);
    }

    OUTEND
}



template <int dim, int range=1, int rank=1>
void cache_get_elem_values(const ValueFlags flag,
                           const int n_knots = 5, const int deg=1)
{
    OUTSTART

    using RefSpace = BSplineSpace<dim, range, rank>;
    using PF = PushForward<Transformation::h_grad,dim,0>;
    using Space = PhysicalSpace<RefSpace, PF>;

    auto grid  = CartesianGrid<dim>::create(n_knots);
    auto ref_space = RefSpace::create(deg, grid);
    auto map = IdentityMapping<dim>::create(grid);
    auto push_forward = PF::create(map);
    auto space = Space::create(ref_space, push_forward);
    auto quad = QGauss<dim>(2);

    SpaceUniformQuadCache<Space> cache(space, flag, quad);

    auto elem = space->begin();
    auto end = space->end();
    cache.init_element_cache(elem);
    for (; elem != end; ++elem)
    {
        cache.fill_element_cache(elem);
        elem->get_basis_values().print_info(out);
    }

    OUTEND
}

#endif

int main()
{
    out.depth_console(10);

    cache_init<1>(ValueFlags::value);
//    cache_init_elem<1>(ValueFlags::value);
//    cache_fill_elem<1>(ValueFlags::value);
//    cache_get_elem_values<1>(ValueFlags::value);

    return  0;
}