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
// TODO (pauletti, Oct 9, 2014): this test is missing header
// TODO (pauletti, Oct 9, 2014): update the code style (its obsolete)
#include "../tests.h"

#include <igatools/basis_functions/nurbs.h>
#include <igatools/basis_functions/bspline_element.h>
#include <igatools/basis_functions/nurbs_element.h>



template < int dim, int range, int rank>
void serialize_deserialize(std::shared_ptr<NURBS<dim,range,rank>> basis_in)
{
  std::shared_ptr<ReferenceBasis<dim,range,rank>> basis = basis_in;
  out.begin_item("Original NURBS:");
  basis->print_info(out);
  out.end_item();


  std::string template_string_info = "_dim" + std::to_string(dim) +
                                     "_range" + std::to_string(range) +
                                     "_rank" + std::to_string(rank);
  std::string filename = "nurbs_basis" + template_string_info + ".xml";
  std::string tag_name = "NURBS" + template_string_info;
  {
    // serialize the NURBS object to an xml file
    std::ofstream xml_ostream(filename);
    OArchive xml_out(xml_ostream);
//    xml_out.template register_type<NRBBasis>();

    xml_out << basis;
//    xml_ostream.close();
  }

  basis.reset();
  {
    // de-serialize the NURBS object from an xml file
    std::ifstream xml_istream(filename);
    IArchive xml_in(xml_istream);
//    xml_in.template register_type<NRBBasis>();

    xml_in >> basis;
//    xml_istream.close();
  }
  out.begin_item("NURBS after serialize-deserialize:");
  basis->print_info(out);
  out.end_item();
  //*/
//*/
}

template< int dim, int range, int rank = 1>
void do_test()
{
  OUTSTART
  using iga::SafeSTLVector;
  SafeSTLVector<Real> coord_x {0,1,2,3,4};
  SafeSTLVector<Real> coord_y {5,6,7,8};
  SafeSTLVector<Real> coord_z {9, 10, 11};

  SafeSTLArray<SafeSTLVector<Real>, dim> coord;
  TensorIndex<dim> degree;

  if (dim == 1)
  {
    coord[0] = coord_x;
    degree[0] = 3;
  }
  else if (dim == 2)
  {
    coord[0] = coord_x;
    coord[1] = coord_y;

    degree[0] = 3;
    degree[1] = 2;
  }
  else if (dim == 3)
  {
    coord[0] = coord_x;
    coord[1] = coord_y;
    coord[2] = coord_z;

    degree[0] = 3;
    degree[1] = 2;
    degree[2] = 1;
  }




  using Basis = NURBS< dim, range, rank >;
  auto grid = Grid<dim>::create(coord);

  auto  bsp = BSpline<dim, range, rank >::create(
                SplineSpace<dim,range,rank>::create(degree,grid));

  using ScalarBSpline = BSpline<dim>;
  using WeightFunc = IgGridFunction<dim,1>;
  auto scalar_space = ScalarBSpline::create(
                        SplineSpace<dim,1,1>::create(degree,grid));
  const auto n_scalar_basis = scalar_space->get_num_basis();

  IgCoefficients weights;
  for (int dof = 0 ; dof < n_scalar_basis ; ++dof)
    weights[dof] = 1.0;

  auto w_func = WeightFunc::create(scalar_space,weights);

  auto nurbs_basis = Basis::create(bsp, w_func);
//    nurbs_basis->print_info(out);


  serialize_deserialize(nurbs_basis);


  OUTEND

}


int main()
{
  do_test<1, 1>();
  do_test<1, 2>();
  do_test<1, 3>();

  do_test<2, 1>();
  do_test<2, 2>();
  do_test<2, 3>();

  do_test<3, 1>();
  do_test<3, 3>();

  return 0;
}
