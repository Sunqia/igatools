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


#ifndef __BSPLINE_ELEMENT_ACCESSOR_H_
#define __BSPLINE_ELEMENT_ACCESSOR_H_

#include <igatools/base/config.h>
#include <igatools/base/cache_status.h>
#include <igatools/base/value_flags_handler.h>
#include <igatools/base/quadrature.h>
#include <igatools/base/function.h>
#include <igatools/geometry/topology.h>
#include <igatools/geometry/cartesian_grid_element_accessor.h>
#include <igatools/utils/value_vector.h>
#include <igatools/utils/value_table.h>
#include <igatools/utils/static_multi_array.h>
#include <igatools/utils/cartesian_product_indexer.h>
#include <igatools/linear_algebra/dense_matrix.h>
#include <igatools/basis_functions/bernstein_basis.h>

IGA_NAMESPACE_OPEN




template <int dim, int range, int rank> class BSplineSpace;
template <typename Accessor> class GridForwardIterator;



class Values1DConstView
{
public:
    /** Type for the container of one dimensional values on a single interval for a single scalar function.*/
    using Values1D = typename DenseMatrix::MatrixRowType ;

    using const_iterator = typename Values1D::const_iterator;

    Values1DConstView() = default;

    Values1DConstView(const DenseMatrix &funcs,const Index func_id)
        :
        funcs_(&funcs),
        func_id_(func_id)
    {
        Assert(func_id >= 0 && func_id < Size(funcs_->size1()),
               ExcIndexRange(func_id,0,Size(funcs_->size1())))
    }


    Values1DConstView(const Values1DConstView &view) = default ;
    Values1DConstView(Values1DConstView &&view) = default ;

    Values1DConstView &operator=(const Values1DConstView &view) = default;
    Values1DConstView &operator=(Values1DConstView &&view) = default;
    /*
    {
        std::swap(funcs_,view.funcs_);
        func_id_ = view.func_id_;
        return (*this);
    }
    //*/

    Real operator()(const Index point_id) const;

private:
    const DenseMatrix *funcs_ = nullptr;
    Index func_id_;
};


inline
Real
Values1DConstView::
operator()(const Index point_id) const
{
    Assert(point_id >= 0 && point_id < Size(funcs_->size2()),
           ExcIndexRange(point_id,0,Size(funcs_->size2())));

    return (*funcs_)(func_id_,point_id);
}


template <int dim>
class
    BSplineElementScalarEvaluator
{
public:
    /** Type for the one dimensional values on a single interval for a single scalar function.*/
    using Values1D = typename DenseMatrix::MatrixRowType ;

    /**
     * Typedef for specifying the derivatives of the scalar basis function in the
     * reference domain.
     */
    template <int deriv_order>
    using Derivative = Derivatives<dim,1,1,deriv_order>;


    /** @name Constructors */
    ///@{
    BSplineElementScalarEvaluator() = delete;

    BSplineElementScalarEvaluator(const std::vector<std::array<Values1DConstView,dim>> &values1D);

    BSplineElementScalarEvaluator(const BSplineElementScalarEvaluator<dim> &bspline) = default;
    BSplineElementScalarEvaluator(BSplineElementScalarEvaluator<dim> &&bspline) = default;

    ~BSplineElementScalarEvaluator() = default;
    ///@}



    /** @name Assignment operators */
    ///@{
    BSplineElementScalarEvaluator<dim> &operator=(const BSplineElementScalarEvaluator<dim> &bspline) = default;
    BSplineElementScalarEvaluator<dim> &operator=(BSplineElementScalarEvaluator<dim> &&bspline) = default;
    ///@}



    Real evaluate_derivative(
        const TensorIndex<dim> &order_tensor_id,
        const TensorIndex<dim> &point_tensor_id) const;


private:

    /**
     * values[i][j] are the values at the n_qp evaluation points of the i-th function derivative
     * along the j-th direction.
     */
    std::vector<std::array<Values1DConstView,dim>> values1D_;
};


template <int dim>
inline
BSplineElementScalarEvaluator<dim>::
BSplineElementScalarEvaluator(const std::vector<std::array<Values1DConstView,dim>> &values1D)
    :
    values1D_(values1D)
{
    Assert(!values1D_.empty(),ExcEmptyObject());
}


/**
 * See module on \ref accessors_iterators for a general overview.
 * @ingroup accessors_iterators
 */
template <int dim, int range, int rank>
class BSplineElementAccessor : public CartesianGridElementAccessor<dim>
{
public:
    /** Type required by the GridForwardIterator templated iterator */
    using ContainerType = BSplineSpace<dim, range, rank> ;

    /** Type required for the generic algorithm on the spaces (plots??) */
    using Space_t = const BSplineSpace<dim, range, rank> ;

    /** Fill flags supported by this iterator */
    static const ValueFlags admisible_flag =
        ValueFlags::point|
        ValueFlags::measure |
        ValueFlags::w_measure |
        ValueFlags::face_point |
        ValueFlags::face_w_measure |
        ValueFlags::value |
        ValueFlags::gradient |
        ValueFlags::hessian |
        ValueFlags::divergence |
        ValueFlags::face_value |
        ValueFlags::face_gradient |
        ValueFlags::face_hessian |
        ValueFlags::face_divergence;

    static const Size n_faces = UnitElement<dim>::faces_per_element;

public:
    /** @name Constructors */
    ///@{
    /**
     * Default constructor. Not allowed to be used.
     */
    BSplineElementAccessor() = delete;

    /**
     * Constructs an accessor to element number index of a
     * BsplineSpace space.
     */
    BSplineElementAccessor(const Space_t &space, const int elem_index);

    /**
     * Copy constructor.
     * @note For the constructed object it
     * creates a new element cache, but it shares
     * the one dimensional cache with the copied element.
     */
    BSplineElementAccessor(const BSplineElementAccessor<dim, range, rank> &elem)
        = default;

    /**
     * Move constructor.
     */
    BSplineElementAccessor(BSplineElementAccessor<dim, range, rank> &&elem)
        = default;

    /**
     * Destructor.
     */
    ~BSplineElementAccessor() = default;
    ///@}


    /** @name Assignment operators */
    ///@{

    /**
     * Copy assignment operator.
     * @note Creates a new element cache, but it shares
     * the one dimensional cache with the copied element.
     */
    BSplineElementAccessor<dim, range, rank> &
    operator=(const BSplineElementAccessor<dim, range, rank> &elem)
        = default;

    /**
     * Move assignment operator.
     */
    BSplineElementAccessor<dim, range, rank> &
    operator=(BSplineElementAccessor<dim, range, rank> &&elem)
        = default;
    ///@}

    /** @name Query information without use of cache */
    ///@{
    /**
     *  Number of non zero basis functions over the current element.
     */
    Size get_num_basis() const;

    /**
     * Number of non-zero scalar basis functions associated
     * with the i-th space component on the element.
     * This makes sense as a reference B-spline space
     * is only allowed to be of the cartesian product type
     * V = V1 x V2 x ... X Vn.
     */
    int get_component_num_basis(const int i) const;

    /**
     * Returns the global dofs of the local (non zero) basis functions
     * on the element.
     * For example:
     * \code
       auto loc_to_glob = elem->get_local_to_global();
       // loc_to_glob[0] is the global id of the first element basis function
       // loc_to_glob[1] is the global id of the second element basis function
       // ...
      \endcode
     *
     */
    std::vector<Index> const &get_local_to_global() const;

    /**
     * Pointer to the BsplineSpace the accessor is iterating on.
     */
    const Space_t *get_space() const;

    ///@}

    /** @name Query information that requires the use of the cache */
    ///@{

    /**
     * Initializes the internal cache for the efficient
     * computation of the values requested in
     * the fill_flag on the given quadrature points.
     * This implies a uniform quadrature scheme
     * (i.e. the same for all elements).
     * @note This function should be called before fill_values()
     */
    void init_values(const ValueFlags fill_flag,
                     const Quadrature<dim> &quad);

    /**
     * For a given face quadrature.
     */
    void init_face_values(const Index face_id,
                          const ValueFlags fill_flag,
                          const Quadrature<dim-1> &quad);

    /**
     * Fills the element values cache according to the evaluation points
     * and fill flags specifies in init_values.
     */
    void fill_values();

    void fill_face_values(const Index face_id);

    /** Reset the global cache */
    void reset_global_cache();

protected:
    /**
     * Typedef for specifying the derivatives of the basis function in the
     * reference domain.
     */
    template <int deriv_order>
    using Derivative = Derivatives<dim, range, rank, deriv_order>;

    using Value = Values<dim, range, rank>;

    using Div = Values<dim, 1, 1>;

public:
    /**
     * Reference to a ValueTable with the values of all local basis function
     * at each evaluation point.
     */
    ValueTable<Value> const &get_basis_values(const TopologyId<dim> &topology_id = ElemTopology<dim>()) const;

    /**
     * Reference to a ValueTable with the values of all local basis function
     * at each evaluation point on the face specified by @p face_id.
     */
    ValueTable<Value> const &get_face_basis_values(const Index face_id) const;

    /**
     * Reference to a ValueTable with the values of all local basis function
     * at each evaluation point.
     */
    ValueTable<Div> const &get_basis_divergences(const TopologyId<dim> &topology_id = ElemTopology<dim>()) const;

    /**
     * Reference to a ValueTable with the gradients of all local basis function
     * evaluated at each evaluation point.
     */
    ValueTable<Derivative<1>> const &get_basis_gradients(const TopologyId<dim> &topology_id = ElemTopology<dim>()) const;

    /**
     * Reference to a ValueTable with hessians of all local basis function
     * at each evaluation point.
     */
    ValueTable<Derivative<2>> const &get_basis_hessians(const TopologyId<dim> &topology_id = ElemTopology<dim>()) const;

    typename ValueTable<Value>::const_view
    get_basis_values(const Index i,const TopologyId<dim> &topology_id = ElemTopology<dim>()) const;


    typename ValueTable<Div>::const_view
    get_basis_divergences(const Index i,const TopologyId<dim> &topology_id = ElemTopology<dim>()) const;


    typename ValueTable<Derivative<1> >::const_view
    get_basis_gradients(const Index i,const TopologyId<dim> &topology_id = ElemTopology<dim>()) const;


    typename ValueTable<Derivative<2> >::const_view
    get_basis_hessians(const Index i,const TopologyId<dim> &topology_id = ElemTopology<dim>()) const;


    /**
     * Reference to the value of a local basis function
     * at one evaluation point.
     */
    Value const &get_basis_value(const Index basis, const Index qp,const TopologyId<dim> &topology_id = ElemTopology<dim>()) const;

    /**
     * Reference to the divergence of a local basis function
     * at one evaluation point.
     */
    Div const &get_basis_divergence(const Index basis, const Index qp,const TopologyId<dim> &topology_id = ElemTopology<dim>()) const;

    /**
     * Reference to the gradient of a local basis function
     * at one evaluation point.
     */
    Derivative<1> const &get_basis_gradient(const Index basis, const Index qp,const TopologyId<dim> &topology_id = ElemTopology<dim>()) const;

    /**
     * Reference to the hessian of a local basis function
     * at one evaluation point.
     */
    Derivative<2> const &get_basis_hessian(const Index basis, const Index qp,const TopologyId<dim> &topology_id = ElemTopology<dim>()) const;

    //Fields related
    /**
     * Vector with the evaluation of the field @p local_coefs at the evaluation
     * points.
     *
     * @see get_local_coefs
     */
    ValueVector<Value>
    evaluate_field(const std::vector<Real> &local_coefs,const TopologyId<dim> &topology_id = ElemTopology<dim>()) const;

    /**
     * Vector with the evaluation of the gradient of the field @p local_coefs
     * at the evaluation points.
     *
     * @see get_local_coefs
     */
    ValueVector<Derivative<1> >
    evaluate_field_gradients(const std::vector<Real> &local_coefs,const TopologyId<dim> &topology_id = ElemTopology<dim>()) const;

    /**
     * Vector with the evaluation of the divergences of the field @p local_coefs
     * at the evaluation points.
     *
     * @see get_local_coefs
     */
    ValueVector<Div>
    evaluate_field_divergences(const std::vector<Real> &local_coefs, const TopologyId<dim> &topology_id = ElemTopology<dim>()) const;

    /**
     * Vector with the evaluation of the hessians of the field @p local_coefs
     * at the evaluation points.
     *
     * @see get_local_coefs
     */
    ValueVector<Derivative<2> >
    evaluate_field_hessians(const std::vector<Real> &local_coefs, const TopologyId<dim> &topology_id = ElemTopology<dim>()) const;
    ///@}


    /**
     * Prints internal information about the BSplineElementAccessor.
     * Its main use is for testing and debugging.
     */
    void print_info(LogStream &out, const VerbosityLevel verbosity_level = VerbosityLevel::normal) const;

private:
    /**
     * Initilizes (reserves memory) the
     * univariate basis values
     * and derivatives at quadrature points cache
     * for efficient use of computations.
     * This function implies a uniform quadrature schemes
     * (the same for each element).
     * The fill_flag provides what information to compute.
     */
    void reset_univariate_cache(const Quadrature<dim> &quad,
                                const int max_der);

    /**
     * Initializes the element cache according to
     * the quadrature number of point and the fill_flag.
     */
    void reset_element_cache(const ValueFlags fill_flag,
                             const Quadrature<dim> &quad);



    /**
     * @name Containers for the cache of the element values and for the
     * cache of the face values
     */
    ///@{
    class FuncPointSize
    {
    public:
        void reset(StaticMultiArray<TensorSize<dim>,range,rank> n_basis_direction,
                   TensorSize<dim> n_points_direction);

        StaticMultiArray<TensorSize<dim>,range,rank> n_basis_direction_;

        TensorSize<dim> n_points_direction_;

        std::shared_ptr<CartesianProductIndexer<dim>> points_indexer_;

        StaticMultiArray<std::shared_ptr<CartesianProductIndexer<dim>>,range,rank> basis_functions_indexer_;
    };


    /**
     * This type store the values, first second derivatives
     * of a 1D Bspline functions, i.e BasisValues1d[k]
     * stores the values of the k-th derivative of the (p+1) basis function on a given interval
     * at the quadrature points.
     * BasisValues1d[k] is a (p+1) x n_qp matrix
     */
    using BasisValues1d = std::vector<DenseMatrix>;

protected:

    /**
     * For each component gives a product array of the dimension
     */
    template<class T>
    using ComponentTable = StaticMultiArray<T,range,rank>;

    /**
     * For each component gives a product array of the dimension
     */
    template<class T>
    using ComponentDirectionTable =
        StaticMultiArray<CartesianProductArray<T,dim>, range, rank>;

    /**
     * Base class for the cache of the element values and for the cache of the face values.
     */
    class ValuesCache : public CacheStatus
    {
    public:
        /**
         * Allocate space for the values and derivatives
         * at quadrature points
         */
        void reset(const BasisElemValueFlagsHandler &flags_handler,
                   const StaticMultiArray<TensorSize<dim>,range,rank> &n_basis_direction,
                   const TensorSize<dim> &n_points_direction);

        /** Returns the values. */
        const ValueTable<Value> &get_values() const;

        /** Returns the gradients. */
        const ValueTable<Derivative<1>> &get_gradients() const;

        /** Returns the hessians. */
        const ValueTable<Derivative<2>> &get_hessians() const;

        /** Returns the divergences. */
        const ValueTable<Div> &get_divergences() const;


        using univariate_values_t = StaticMultiArray<std::array<const BasisValues1d *,dim>,range,rank>;

        /**
         * Fills the cache (accordingly with the flags_handler status)
         * from the univariate values (and derivatives up to the order
         * specified by @p max_deriv_order).
         *
         *
         * @note The BSplineElementAccessor @p elem is needed in order to call the function
         * elem.evaluate_bspline_derivatives<p>()
         * that computes the @p p-th order derivatives of a BSpline from the univariate values.
         */
        void fill_from_univariate(
            const int max_deriv_order,
            const univariate_values_t &values_1D,
            const BSplineElementAccessor<dim,range,rank> &elem);


        //TODO: the member variables should be private
    public:

        BasisElemValueFlagsHandler flags_handler_;


        ValueTable<Value> phi_hat_;

        //TODO (Sep 16, 2013, pauletti): D0phi_hat_ must be removed
        ValueTable<Derivative<0>> D0phi_hat_;
        ValueTable<Derivative<1>> D1phi_hat_;
        ValueTable<Derivative<2>> D2phi_hat_;

        ValueTable<Div> div_phi_hat_;

    public:

        FuncPointSize size_;


        ComponentTable<
        DynamicMultiArray<std::shared_ptr<BSplineElementScalarEvaluator<dim>>,dim>> scalar_evaluators_;

    };


    /**
     * Cache for the element values at quadrature points
     */
    class ElementValuesCache : public ValuesCache
    {
    public:
        /**
         * Allocate space for the values and derivatives
         * at quadrature points
         */
        void reset(const BasisElemValueFlagsHandler &flags_handler,
                   const StaticMultiArray<TensorSize<dim>, range, rank> &n_basis_direction,
                   const Quadrature<dim> &quad);

    };


    /**
     * Cache for the face values at quadrature points
     */
    class FaceValuesCache : public ValuesCache
    {
    public:
        /**
         * Allocate space for the values and derivatives
         * at quadrature points
         */
        void reset(const Index face_id,
                   const BasisFaceValueFlagsHandler &flags_handler,
                   const StaticMultiArray<TensorSize<dim>, range, rank> &n_basis_direction,
                   const Quadrature<dim> &quad);

        /**
         * Allocate space for the values and derivatives
         * at quadrature points for a specified face.
         */
        void reset(const Index face_id,
                   const BasisFaceValueFlagsHandler &flags_handler,
                   const StaticMultiArray<TensorSize<dim>, range, rank> &n_basis_direction,
                   const Quadrature<dim-1> &quad);

    };

    /**
     * @todo Document this function
     */
    const ValuesCache &get_values_cache(const TopologyId<dim> &topology_id) const;

    ///@}

public:
    /**
     * For a given flags input argument identifies the face quantities and
     * returns a new ValueFlags variable containing only face quantities.
     * The output flags does not contain the word face.
     */
    ValueFlags get_face_flags(const ValueFlags fill_flag) const ;


private:

    /**
     * Computes the k-th order derivative of the non-zero B-spline basis
     * functions over the current element,
     * at the evaluation points pre-allocated in the cache.
     *
     * \warning If the output result @p derivatives_phi_hat is not correctly pre-allocated,
     * an exception will be raised.
     */
    template <int deriv_order>
    void evaluate_bspline_derivatives(const FuncPointSize &size,
                                      const StaticMultiArray<std::array<const BasisValues1d *, dim>, range, rank> &elem_values,
                                      const ValuesCache &cache,
                                      ValueTable< Derivative<deriv_order> > &derivatives_phi_hat) const;





    class GlobalCache : public CacheStatus
    {
    public:
        int max_deriv_order_ = 0;
    };

    /**
     * Cache for the efficient use of Bspline basis on a uniform
     * quadrature scheme on all elements.
     */
    class GlobalElemCache : public GlobalCache
    {
    public:
        /**
         * Allocates space for and compute the values and
         * derivatives at quadrature points for one
         * dimensional B-splines for each component
         * and each direction of a component and each interval
         * of a direction.
         */
        void reset(const Space_t &space,
                   const Quadrature<dim> &quad,
                   const int max_der);

        /**
         * univariate B-splines values and derivatives at
         * quadrature points
         * splines1d_cache_data_[comp][dir][interval][order][function][point]
         */
        ComponentDirectionTable<BasisValues1d> splines1d_cache_data_;

        ComponentDirectionTable<const BasisValues1d *> splines1d_cache_;

    };

    class GlobalFaceCache : public GlobalCache
    {
    public:
        /**
         * Allocates space for and compute the values and
         * derivatives at quadrature points for one
         * dimensional B-splines for each component
         * and each direction of a component and each interval
         * of a direction.
         */
        void reset(const Space_t &space,
                   const Quadrature<dim> &quad1,
                   const Index face_id,
                   const int max_der);

        /**
         * univariate B-splines values and derivatives at
         * quadrature points
         * splines1d_cache_data_[comp][interval][order][function][point]
         */
        ComponentTable<BasisValues1d> splines1d_cache_data_;

        ComponentTable<const BasisValues1d *> splines1d_cache_;

    };

    /**
     * Tensor product style space sized cache for
     * storing the values and derivatives of the
     * one dimensional values of the basis
     * function at the quadrature points
     */
    std::shared_ptr< GlobalElemCache > values_1d_elem_ = nullptr;

    std::array<std::shared_ptr<GlobalFaceCache>, n_faces> values_1d_faces_;

protected:
    /**
     * Element cache to store the values and derivatives
     * of the B-spline basis functions
     */
    ElementValuesCache elem_values_;

    /**
     * Face cache to store the values and derivatives
     * of the B-spline basis functions on the faces of the element
     */
    std::array<FaceValuesCache, n_faces> face_values_;

private:
    /**
     * Space for which the BSplineElementAccessor refers to.
     */
    const Space_t *space_ = nullptr;


    template <typename Accessor> friend class GridForwardIterator;

public:
    /**
     * Container for the 1D values on the element.
     * @todo this variable should be private...
     * it is public only to quickly test the sum_factorization technique
     */
    StaticMultiArray<std::array<const BasisValues1d *,dim>,range,rank>
    elem_univariate_values_;



};

IGA_NAMESPACE_CLOSE

#endif
