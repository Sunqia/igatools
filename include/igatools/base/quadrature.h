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

#ifndef QUADRATURE_H_
#define QUADRATURE_H_

#include <igatools/base/config.h>
#include <igatools/utils/tensor_product_array.h>

IGA_NAMESPACE_OPEN


template <int dim_>
class EvaluationPoints
{
public:

    /**
     * Returns the total number of evaluation points.
     */
    virtual int get_num_points() const = 0;

    /**
     * Returns TRUE if the evaluation points have a tensor-product structure.
     */
    virtual bool is_tensor_product_struct() const = 0;

protected:

    /**
     * @name Constructors.
     */
    ///@{
    /**
     * Default constructor.
     */
    EvaluationPoints() = default;

    /**
     * Copy constructor.
     */
    EvaluationPoints(const EvaluationPoints<dim_> &pts) = default;

    /**
     * Move constructor.
     */
    EvaluationPoints(EvaluationPoints<dim_> &&pts) = default;

    /**
     * Destructor.
     */
    virtual ~EvaluationPoints() = default;
    ///@}


    /**
     * @name Assignment operators.
     */
    ///@{
    /**
     * Copy assignment operator.
     */
    EvaluationPoints<dim_> &operator=(const EvaluationPoints<dim_> &pts) = default;

    /**
     * Move assignment operator.
     */
    EvaluationPoints<dim_> &operator=(EvaluationPoints<dim_> &&pts) = default;
    ///@}

private:

//    vector<TensorIndex<dim_>> map_point_id_to_coords_id_;
};


/**
 * @brief Base class for tensor product quadrature formulas with tensor-product structure,
 *  in @p dim dimensions.
 *
 * This class stores quadrature points and weights on a dim-dimensional unit
 * hypercube
 * \f$ [0,1]^\text{dim} \f$.
 *
 * It also provides methods for projecting the points on the face and for building
 * quadrature extension in the hypercube
 * \f$ [0,1]^\text{dim+1} \f$.
 *
 */
template<int dim_>
class QuadratureTensorProduct
    : public EvaluationPoints<dim_>
{
private:
    using self_t = QuadratureTensorProduct<dim_>;
public:
    static const int dim = dim_;
    using WeigthArray = TensorProductArray<dim>;
    using PointArray  = CartesianProductArray<Real, dim>;
public:
    ///@name Constructors
    ///@{
    /**
     * Default constructor. It does nothing.
     */
    QuadratureTensorProduct() = default;

    /**
     * Creates a tensor product quadrature rule
     * on the unit d-dimensional hypercube \f$ [0,1]^d \f$,
     * with @p num_points[i] number of points in the i-th dimension.
     */
    explicit QuadratureTensorProduct(const TensorSize<dim> num_points);

    /**
     * Creates a tensor product quadrature rule
     * on the unit d-dimensional hypercube \f$ [0,1]^d \f$,
     * with @p num_points points in each direction.
     */
    explicit QuadratureTensorProduct(const Index num_points);

    /**
     * Creates a tensor product quadrature rule with the points, the
     * weights and the domain coordinates of the d-dimensional hypercube
     * upon which the quadrature is referred to.
     */
    explicit QuadratureTensorProduct(const PointArray &points,
                                     const WeigthArray &weights);

    /**
     * Destructor.
     */
    ~QuadratureTensorProduct() = default;

    /**
     * Copy constructor.
     * It performs a deep copy of the Quadrature object.
     */
    QuadratureTensorProduct(const self_t &quad_scheme) = default;

    /**
    * Move constructor.
    */
    QuadratureTensorProduct(self_t &&quad_scheme) = default;
    ///@}

    ///@name Assignment operators
    ///@{
    /**
     * Copy assignment operator.
     * It performs a deep copy of the Quadrature object.
     */
    QuadratureTensorProduct<dim> &operator=(const self_t &quad_scheme) = default;

    /**
     * Move assignment operator.
     */
    QuadratureTensorProduct<dim> &operator=(self_t  &&quad_scheme) = default;
    ///@}

    ///@name Getting informations about the points and the domain.
    ///@{
    /**
     * Returns the total number of quadrature points.
     */
    virtual Size get_num_points() const noexcept override final;

    /**
     * Returns TRUE.
     */
    virtual bool is_tensor_product_struct() const override final;

    /**
     * Returns the number of quadrature points along each coordinate direction.
     */
    TensorSize<dim> get_num_points_direction() const noexcept;
    ///@}

    ///@name Getting a copy of the internal variables.
    ///@{
    /**
     * Return all quadrature weights in
     * a tensor-product structure.
     */
    WeigthArray get_weights() const noexcept ;

    /**
     * Return all quadrature points in
     * a tensor-product structure.
     */
    PointArray get_points() const noexcept;

    ///@}

    /**
     * Returns a dim dimensional quadrature obtained by using
     * a single point on the active face direction.
     * @todo write example
     * Usually use for face values
     */
    template<int k>
    self_t collapse_to_sub_element(const int id) const;

    /**
     * Prints internal information about the quadrature scheme.
     * \note Mostly used for debugging and testing.
     */
    void print_info(LogStream &out) const;

protected:
    /**
     * Quadrature points.
     */
    PointArray points_;

    /**
     * Quadrature weights.
     */
    WeigthArray weights_;
};



/**
 * Given a quadrature rule on a dim dimensional face, of a dim+1
 * domain, this functions creates an extended dimension
 * version of this quadrature applicable to the volume.
 *
 * @relates Quadrature
 */
template<int k, int dim>
QuadratureTensorProduct<dim>
extend_sub_elem_quad(const QuadratureTensorProduct<k> &quad, const int sub_elem_id);


IGA_NAMESPACE_CLOSE

#endif // #ifndef QUADRATURE_H_
