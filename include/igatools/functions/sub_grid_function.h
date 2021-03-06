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

#ifndef __SUB_GRID_FUNCTION_H_
#define __SUB_GRID_FUNCTION_H_

#include <igatools/base/config.h>
#include <igatools/functions/grid_function.h>

IGA_NAMESPACE_OPEN



template <int,int,int> class SubGridFunctionElement;
template <int,int,int> class SubGridFunctionHandler;

template<int sdim,int dim,int range>
class SubGridFunction :
  public GridFunction<sdim,range>
{
private:
  using self_t = SubGridFunction<sdim,dim,range>;


public:
  using base_t  = GridFunction<sdim,range>;
  using SupFunc = GridFunction<dim,range>;

  using GridType = Grid<sdim>;
  using SuperGrid = Grid<dim>;

  using ElementAccessor = SubGridFunctionElement<sdim,dim,range>;
  using ElementIterator = GridIterator<ElementAccessor>;
  using Handler = SubGridFunctionHandler<sdim,dim,range>;

  using List = typename GridType::List;
  using ListIt = typename GridType::ListIt;


  //    template <int j>
  using SubGridMap = typename SuperGrid::template SubGridMap<sdim>;


public:

  SubGridFunction(const SharedPtrConstnessHandler<SupFunc> &sup_func,
                  const int sup_grid_func_s_id,
                  const SubGridMap &sub_grid_elem_map,
                  const SharedPtrConstnessHandler<GridType> &grid);

  virtual ~SubGridFunction() = default;

  virtual GridIterator<GridFunctionElement<sdim,range> >
  cbegin(const PropId &prop) const override;

  virtual GridIterator<GridFunctionElement<sdim,range> >
  cend(const PropId &prop) const override;


  static std::shared_ptr<const self_t>
  const_create(const std::shared_ptr<const SupFunc> &func,
               const int sup_grid_func_s_id,
               const SubGridMap &sub_grid_elem_map,
               const std::shared_ptr<const GridType> &grid);

  static std::shared_ptr<self_t>
  create(const std::shared_ptr<const SupFunc> &func,
         const int sup_grid_func_s_id,
         const SubGridMap &sub_grid_elem_map,
         const std::shared_ptr<GridType> &grid);

  virtual std::unique_ptr<GridFunctionHandler<sdim,range> >
  create_cache_handler() const override;

#if 0
  virtual std::unique_ptr<GridFunctionElement<sdim,range> >
  create_element(const ListIt &index, const PropId &prop) const override final;
#endif

  virtual std::unique_ptr<GridFunctionElement<sdim,range>>
                                                        create_element_begin(const PropId &prop) const override final;

  virtual std::unique_ptr<GridFunctionElement<sdim,range>>
                                                        create_element_end(const PropId &prop) const override final;


#ifdef IGATOOLS_WITH_MESH_REFINEMENT
  void rebuild_after_insert_knots(
    const SafeSTLArray<SafeSTLVector<double>, sdim> &new_knots,
    const GridType &old_grid) override final;
#endif

  void print_info(LogStream &out) const override final;


  std::shared_ptr<const SupFunc> get_sup_grid_function() const;


  const SafeSTLVector<typename Grid<sdim>::IndexType> &
  get_id_elems_sub_grid() const;

  const SafeSTLVector<typename Grid<dim>::IndexType> &
  get_id_elems_sup_grid() const;

  const typename Grid<dim>::IndexType &
  get_sup_element_id(const typename Grid<sdim>::IndexType &sub_elem_id) const;

  const SubGridMap &get_sub_grid_elem_map() const;



  /**
   * @brief This function compute the preimage of the points <tt>phys_points</tt>.
   *
   * @warning If an input point is not in the image of the Grid under the
   * action of the instantiated SubGridFunction,
   * an assertion will be raised (in Debug and Release mode)
   *
   */
  virtual ValueVector<Points<sdim>>
                                 evaluate_preimage(const ValueVector<Points<range>> &phys_points) const override final;

private:
  SharedPtrConstnessHandler<SupFunc> sup_func_;
  const int sup_grid_func_s_id_;



//  const SubGridMap sub_grid_elem_map_;

  SafeSTLVector<typename Grid<sdim>::IndexType> id_elems_sub_grid_;
  SafeSTLVector<typename Grid< dim>::IndexType> id_elems_sup_grid_;

  friend class SubGridFunctionHandler<sdim,dim,range>;
};


IGA_NAMESPACE_CLOSE

#endif

