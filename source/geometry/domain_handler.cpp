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

#include <igatools/geometry/domain_handler.h>
#include <igatools/geometry/domain.h>
#include <igatools/geometry/domain_element.h>

IGA_NAMESPACE_OPEN

template<int dim_, int codim_>
DomainHandler<dim_, codim_>::
DomainHandler(std::shared_ptr<DomainType> domain)
  :
  domain_(domain),
  grid_func_handler_(domain->get_grid_function()->create_cache_handler()),
  flags_(CacheFlags::none)
{
  Assert(domain_ != nullptr, ExcNullPtr());
}



template<int dim_, int codim_>
auto
DomainHandler<dim_, codim_>::
get_domain() const -> std::shared_ptr<DomainType>
{
  return domain_;
}



template<int dim_, int codim_>
void
DomainHandler<dim_, codim_>::
set_flags(const topology_variant &sdim,
          const Flags &flag)
{
  using GridFuncFlags = typename GridFuncType::ElementHandler::Flags;
  using GridFlags = typename GridFuncType::GridType::ElementHandler::Flags;

  GridFlags  grid_flag = GridFlags::none;
  GridFuncFlags  grid_func_flag = GridFuncFlags::none;
  CacheFlags dom_flag = CacheFlags::none;

  for (auto &fl : domain_element::all_flags)
    if (contains(flag, fl))
    {
      grid_func_flag |= domain_element::activate::grid_func[fl];
      grid_flag |= domain_element::activate::grid[fl];
      dom_flag  |= domain_element::activate::domain[fl];
    }

  grid_func_handler_->set_flags(sdim, grid_func_flag);
  grid_func_handler_->get_grid_handler().set_flags(sdim, grid_flag);
  auto disp = SetFlagsDispatcher(dom_flag, flags_);
  boost::apply_visitor(disp, sdim);
}



template<int dim_, int codim_>
void
DomainHandler<dim_, codim_>::
set_element_flags(const Flags &flag)
{
  this->set_flags(Topology<dim_>(), flag);
}

template<int dim_, int codim_>
void
DomainHandler<dim_, codim_>::
init_cache(ElementAccessor &elem,
           const eval_pts_variant &quad) const
{
  grid_func_handler_->init_cache(*(elem.grid_func_elem_), quad);

  auto disp = InitCacheDispatcher(*this, elem, flags_);
  boost::apply_visitor(disp, quad);
}

template<int dim_, int codim_>
void
DomainHandler<dim_, codim_>::
init_cache(ElementIterator &elem,
           const eval_pts_variant &quad) const
{
  this->init_cache(*elem, quad);
}

template<int dim_, int codim_>
void
DomainHandler<dim_, codim_>::
init_element_cache(ElementIterator &elem,
                   const std::shared_ptr<Quadrature<dim_>> &quad) const
{
  this->init_cache(elem,quad);
}

template<int dim_, int codim_>
void
DomainHandler<dim_, codim_>::
init_element_cache(ElementAccessor &elem,
                   const std::shared_ptr<Quadrature<dim_>> &quad) const
{
  this->init_cache(elem,quad);
}


template<int dim_, int codim_>
void
DomainHandler<dim_, codim_>::
fill_cache(const topology_variant &sdim,
           ElementAccessor &elem,
           const int s_id) const
{
  grid_func_handler_->fill_cache(sdim, *(elem.grid_func_elem_), s_id);

  auto disp = FillCacheDispatcher(elem, s_id);
  boost::apply_visitor(disp, sdim);
}


template<int dim_, int codim_>
void
DomainHandler<dim_, codim_>::
fill_cache(const topology_variant &sdim,
           ElementIterator &elem,
           const int s_id) const
{
  this->fill_cache(sdim, *elem, s_id);
}



template<int dim_, int codim_>
void
DomainHandler<dim_, codim_>::
fill_element_cache(ElementAccessor &elem) const
{
  this->fill_cache(Topology<dim_>(), elem, 0);
}

template<int dim_, int codim_>
void
DomainHandler<dim_, codim_>::
fill_element_cache(ElementIterator &elem) const
{
  this->fill_cache(Topology<dim_>(), elem, 0);
}


//    if (flag_.fill_inv_hessians())
//    {
//        const auto &D1_F = elem.get_gradients();
//        const auto &D2_F = elem.get_hessians();
//        const auto &D1_invF = std::get<1>(cache->inv_derivatives_);
//        auto &D2_invF = std::get<2>(cache->inv_derivatives_);
//
//        for (int i=0; i<n_points; ++i)
//            for (int u=0; u<dim_; ++u)
//            {
//                const auto tmp_u = action(D2_F[i], D1_invF[i][u]);
//                for (int v=0; v<dim_; ++v)
//                {
//                    const auto tmp_u_v = action(tmp_u, D1_invF[i][v]);
//                    D2_invF[i][u][v] = - action(D1_invF[i], tmp_u_v);
//                }
//            }
//    }
//}

IGA_NAMESPACE_CLOSE

#include <igatools/geometry/domain_handler.inst>

