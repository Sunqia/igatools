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

#ifndef DOF_DISTRIBUTION_H_
#define DOF_DISTRIBUTION_H_

#include <igatools/base/config.h>
#include <igatools/basis_functions/spline_space.h>
#include <igatools/utils/concatenated_iterator.h>



IGA_NAMESPACE_OPEN




/**
 *
 * @brief Class to handle the distribution of the basis function
 * indices that are defined on a single patch space,
 * storing what is known as the local to global map.
 *
 * It is responsible to assign to each basis function an index
 * and knows which of them are non zero on each element.
 *
 * It allows to store different properties on subset of
 * the basis functions.
 *
 *
 * @author pauletti, 2014, 2015
 * @author martinelli, 2014, 2015
 *
 * @ingroup serializable
 */
template<int dim, int range = 1, int rank = 1>
class DofDistribution
{
public:
  using Space = SplineSpace<dim, range, rank>;
  using DegreeTable = typename Space::DegreeTable;
  using PeriodicityTable = typename Space::PeriodicityTable;
  using TensorSizeTable = typename Space::TensorSizeTable;
//  using OffsetTable = typename Space::template ComponentContainer<Size>;
  using OffsetTable = SafeSTLArray<int,Space::n_components>;
  using IndexDistributionTable =
    StaticMultiArray<DynamicMultiArray<Index,dim>,range,rank>;


  /** Type alias for the dofs container used in each scalar component of a single-patch space. */
  using DofsComponentContainer = SafeSTLVector<Index>;

  /** Type alias for the View on the dofs in each scalar component of a single-patch space. */
  using DofsComponentView = ContainerView<DofsComponentContainer>;

  /** Type alias for the ConstView on the dofs in each scalar component of a single-patch space. */
  using DofsComponentConstView = ConstContainerView<DofsComponentContainer>;

  /** Type alias for a concatenated iterator defined on several component views. */
  using DofsIterator = ConcatenatedIterator<DofsComponentView>;

  /** Type alias for a concatenated const-iterator defined on several component views. */
  using DofsConstIterator = ConcatenatedConstIterator<DofsComponentView,DofsComponentConstView>;

  /** Type alias for the View on the dofs held by the single-patch space. */
  using DofsView = NonConstView<DofsIterator,DofsConstIterator>;

  /** Type alias for the ConstView on the dofs held by the single-patch space. */
  using DofsConstView = ConstView<DofsIterator,DofsConstIterator>;



  /** @name Constructors */
  ///@{
  /**
   * Default constructor. It does nothing but it is needed for the serialization.
   */
  DofDistribution() = default;

  //TODO: document this constructor
  DofDistribution(const TensorSizeTable &n_basis,
                  const DegreeTable &degree_table,
                  const PeriodicityTable &periodic);

  /** Copy constructor.*/
  DofDistribution(const DofDistribution &dof_ditribution) = delete;

  /** Move constructor.*/
  DofDistribution(DofDistribution &&dof_ditribution) = default;

  /** Destructor. */
  ~DofDistribution() = default;
  //@}


  /** Assignment operators */
  ///@{
  /** Copy assignment operator. Not allowed to be used. */
  DofDistribution &operator=(const DofDistribution &dof_distribution) = delete;

  /** Move assignment operator.*/
  DofDistribution &operator=(DofDistribution &&dof_distribution) = default;
  ///@}



  /**
   * This function looks for a @p dof_id and (if found) gives back its
   * component id @p comp
   * and its flat index @p dof_id_in_component within the component.
   *
   * @returns TRUE if the @p dof_id is found in the DofDistribution.
   *
   * @warning If the @p dof_id is NOT found in the DofDistribution the values
   *  @p comp_id and
   * @p tensor_id are UNDETERMINED.
   */
  bool find_dof_id(const Index dof_id, int &comp_id, Index &dof_id_in_component) const;

  /**
   * Print the class content
   */
  void print_info(LogStream &out) const;


  /** Add an @p offset to the dofs. */
  void add_dofs_offset(const Index offset);

  /** Returns the minimum dof id. */
  Index get_min_dof_id() const;

  /** Returns the maximum dof id. */
  Index get_max_dof_id() const;


  /**
   * Returns the number of dofs with the property specified by @p dofs_property.
   *
   * @note If @p dofs_property is equal to the default value DofProperties::none then the
   * value returned is the total number of dofs in the object.
   */
  Size get_num_dofs(const std::string &dofs_property) const;


  /**
   * Returns the container used to store the dofs ids of each component
   * of a single patch space.
   *
   * @warning This object can have a BIG memory footprint, therefore
   * its copy is discouraged: please use the associated View instead!
   */
  const IndexDistributionTable &get_index_table() const;



  /**
   * This function returns the global dof id corresponding to the dof
   * with tensor index <p>tensor_index</p> on the @p comp component of the space.
   */
  Index get_global_dof_id(const TensorIndex<dim> &tensor_index,
                          const Index comp) const;


  Size get_num_dofs_comp(const int comp) const;

  Size get_num_dofs_comp(const int comp, const int dir) const;


  /**
   * Component table with the offset of unique dofs
   * in each component.
   */
  OffsetTable get_dofs_offset() const;


  /**
   * Returns a const view of the dofs ids (without taking into accounts any property) on a given single-patch space
   */
  DofsConstView get_dofs_const_view() const;

  /**
   * Returns a non-const view of the dofs ids (without taking into accounts any property) on a given single-patch space
   */
  DofsView get_dofs_view();

  /**
   * Return the container holding the number of unique dofs,
   * component-by-component and direction-by-direction.
   */
  const TensorSizeTable &get_num_dofs_table() const;

  /**
   * Converts a @p global_dof_id into the correspondent local (patch) representation.
   *
   * @note The @p global_dof_id must be in the DofDistribution, otherwise in DEBUG mode
   * an assertion will be raised.
   */
  Index global_to_patch_local(const Index global_dof_id) const;



  /**
   * Converts a @p global_dof_id into the correspondent "component" representation,
   * i.e. the component id @p comp and the flat index @p dof_id within the component.
   *
   * @note The @p global_dof_id must be in the DofDistribution, otherwise in DEBUG mode
   * an assertion will be raised.
   */
  void global_to_comp_local(const Index global_dof_id,int &comp,int &dof_id) const;

  /**
   * Returns the dofs that are on the interior of the <tt>dim</tt>-dimensional hypercube
   * (i.e. the dofs that are not on the boundary).
   */
  std::set<Index> get_interior_dofs() const;


  /**
   * Global indices of the basis functions whose support intersect the
   * boundary.
   */
  std::set<Index>
  get_boundary_dofs(const int s_id, const TopologyVariants<dim> &topology) const
  {
    std::set<Index> dofs;
    GetBoundaryDofsDispatcher boundary_dofs_impl(
      s_id,index_table_,num_dofs_table_,index_table_size_,dofs);

    boost::apply_visitor(boundary_dofs_impl, topology);
    return dofs;
  }

private:
  struct GetBoundaryDofsDispatcher : boost::static_visitor<void>
  {
    GetBoundaryDofsDispatcher(const int s_id,
                              const IndexDistributionTable &index_table,
                              const TensorSizeTable &num_dofs_table,
                              const TensorSizeTable &index_table_size,
                              std::set<Index> &dofs)
      :
      s_id_(s_id),
      index_table_(index_table),
      num_dofs_table_(num_dofs_table),
      index_table_size_(index_table_size),
      dofs_(dofs)
    {}

    template<int sub_elem_dim>
    void operator()(const Topology<sub_elem_dim> &sub_elem)
    {
      auto &k_elem = UnitElement<dim>::template get_elem<sub_elem_dim>(s_id_);
      const auto &active_dirs = k_elem.active_directions;

      const int n_dir = k_elem.constant_directions.size();

#ifndef NDEBUG
      for (int comp = 0 ; comp < IndexDistributionTable::n_entries ; ++comp)
        for (const auto const_dir : k_elem.constant_directions)
        {
          Assert(index_table_size_[comp][const_dir] == num_dofs_table_[comp][const_dir],
                 ExcNotImplemented());
        }
#endif

      TensorIndex<dim> first;
      TensorIndex<dim> last;

      for (int comp = 0 ; comp < IndexDistributionTable::n_entries ; ++comp)
      {
        for (int j = 0 ; j < sub_elem_dim ; ++j)
        {
          first[active_dirs[j]] = 0;
          last[active_dirs[j]] = num_dofs_table_[comp][active_dirs[j]];
        }

        for (int j = 0 ; j < n_dir ; ++j)
        {
          const auto dir = k_elem.constant_directions[j];
          const auto val = k_elem.constant_values[j];
          const int fixed_id = val * (num_dofs_table_[comp][dir] - 1);
          first[dir] = fixed_id;
          last[dir] = fixed_id + 1;
        }
        auto tensor_ind = el_tensor_range(first, last);


        const auto &elem_global_indices = index_table_[comp];

        for (auto &tensor_index : tensor_ind)
          dofs_.insert(elem_global_indices(tensor_index));
      }


    }
  private:
    const int s_id_;
    const IndexDistributionTable &index_table_;
    const TensorSizeTable &num_dofs_table_;
    const TensorSizeTable &index_table_size_;
    std::set<Index> &dofs_;
  };

public:
  /**
   * @name Functions related to the management/query of the dof properties.
   */
  ///@{
  /**
   * Returns TRUE if the @p property is defined.
   *
   * @warning Returns true also if the @p property is defined but no dofs have the given @p property.
   */
  bool is_property_defined(const std::string &property) const;


  /**
   * Returns true if the dof with id @p dof_id has the asked @p property.
   */
  bool test_if_dof_has_property(const Index dof_id, const std::string &property) const;


  /**
   * Adds a new <tt>property</tt> definition for the dofs in the DofDistribution.
   *
   * @note If the <tt>property</tt> is already present, n assertion will be raised (in Debug mode).
   */
  void add_dofs_property(const std::string &property);

  /**
   * Returns the id of the dofs having a certain @p property (non-const version).
   */
  std::set<Index> &get_global_dofs(const std::string &property = DofProperties::active);

  /**
   * Returns the id of the dofs having a certain @p property (const version).
   */
  const std::set<Index> &get_global_dofs(const std::string &property = DofProperties::active) const;


  /**
   * Sets the @p status of the given @p property for the dof with the specified @p dof_id.
   */
  void set_dof_property_status(const std::string &property, const Index dof_id, const bool status);

  void set_dof_property_status(const std::string &property,
                               const std::set<Index> ids,
                               const bool status);
  /**
   * Sets the @p status of the given @p property for all the dofs in the
   * DofDistribution.
   */
  void set_all_dofs_property_status(const std::string &property, const bool status);
  ///@}

private:

  /**
   * Container used to store the dofs ids of each component of a single patch space.
   *
   * @warning This object can have a BIG memory footprint, therefore its copy is discouraged: please
   * use the associated View instead!
   */
  IndexDistributionTable index_table_;


  /**
   * Number of unique dofs, component-by-component and direction-by-direction.
   */
  TensorSizeTable num_dofs_table_;



  /**
   * Size of the index_table_, component-by-component and direction-by-direction.
   *
   * @note The size of the <tt>index_table_</tt> can be different to the number of dofs
   * (e.g for dofs associated to periodic spaces).
   */
  TensorSizeTable index_table_size_;



  /**
   * Container for the dofs having a certain property.
   *
   * The property name is the key of the std::map.
   */
  PropertiesDofs properties_dofs_;

#ifdef IGATOOLS_WITH_SERIALIZATION
  /**
   * @name Functions needed for serialization.
   * @see <a href="http://uscilab.github.io/cereal/serialization_functions.html">Cereal serialization</a>
   */
  ///@{
  friend class cereal::access;

  template<class Archive>
  void
  serialize(Archive &ar);
  ///@}
#endif // IGATOOLS_WITH_SERIALIZATION
};

IGA_NAMESPACE_CLOSE


#ifdef IGATOOLS_WITH_SERIALIZATION

#include <igatools/basis_functions/dof_distribution.serial>

#endif // IGATOOLS_WITH_SERIALIZATION


#endif
