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

#ifndef IDENTITY_MAPPING_H_
#define IDENTITY_MAPPING_H_

#include <igatools/geometry/mapping.h>

IGA_NAMESPACE_OPEN

/**
 * @brief The identity mapping.
 *
 * @note It is not a mapping to be used by user.
 * It is mostly used for testing and debugging and as
 * a special way of treating a reference space as
 * if it was a physical space (in unavoidable cases).
 *
 * @note Do not use this map if you don't need to
 */
template<int dim, int codim = 0>
class IdentityMapping : public Mapping <dim, codim>
{
private:
    using self_t = IdentityMapping<dim, codim>;
    using base_t = Mapping<dim, codim>;

    using typename base_t::PointType;
    using typename base_t::ValueType;
    using typename base_t::GradientType;
    using typename base_t::HessianType;
    using typename base_t::GradientFaceType;
    using typename base_t::HessianFaceType;
    using typename base_t::GridType;

public:
    IdentityMapping() = delete;

    IdentityMapping(const std::shared_ptr<GridType> grid);

    /** Copy constructor */
    IdentityMapping(const self_t &map);

    /** Copy assignment operator. Not allowed to be used. */
    self_t &operator=(const self_t &map) = delete;

    static std::shared_ptr<base_t> create(const std::shared_ptr<GridType> grid);

    ValueFlags required_flags() const;

    void set_element(const CartesianGridElementAccessor<dim> &elem);

    void evaluate(std::vector<ValueType> &values) const override;

    void evaluate_gradients(std::vector<GradientType> &gradients) const override;

    void evaluate_hessians(std::vector<HessianType> &hessians) const override;

    /**
     * Return a Mapping that is a deep copy of the caller object.
     */
    std::shared_ptr<base_t> clone() const;

    /**
     * Prints internal information about the mapping.
     * @note Mostly used for debugging and testing.
     */
    void print_info(LogStream &out) const override;

private:
    GradientType A_;
    std::vector<PointType> points_;
};

IGA_NAMESPACE_CLOSE

#endif /* MAPPING_H_ */