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

#include <paraview_plugin/iga_vtk.h>

#include <vtkMultiBlockDataSet.h>
#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>
#include <vtkSmartPointer.h>
#include <vtkInformation.h>
#include <vtkCellArray.h>
#include <vtkHexahedron.h>
#include <vtkQuad.h>
#include <vtkLine.h>

#include <igatools/functions/functions_container.h>
#include <igatools/base/quadrature_lib.h>



using namespace iga;
using std::string;
using std::shared_ptr;
using std::pair;



IGAVTK::
IGAVTK ()
{};



void
IGAVTK::
set_file (const string& file_name, const string& file_path)
{
  file_name_ = file_name;
  file_path_ = file_path;
  // TODO: Check here that the file exists.
};



void
IGAVTK::
set_number_visualization_points (const int* const num_visualization_points)
{

  static const int dim = 3;
  for (int dir = 0; dir < dim; ++dir)
    num_visualization_points_[dir] = *(num_visualization_points + dir);
};



void
IGAVTK::
clear ()
{
  AssertThrow (false, ExcNotImplemented ());
};



void
IGAVTK::
generate_vtk_grids(const int& grid_type,
                   const bool& create_control_mesh,
                   const bool& create_parametric_mesh,
                   const bool& create_physical_mesh,
                   vtkMultiBlockDataSet* const mb) const
{
  Assert (file_name_ != "", ExcMessage ("Not specified file name."));
  Assert (file_path_ != "", ExcMessage ("Not specified file path."));
  Assert (num_visualization_points_[0] > 1,
          ExcMessage ("Number of visualization points must be > 1 in every direction."));
  Assert (num_visualization_points_[1] > 1,
          ExcMessage ("Number of visualization points must be > 1 in every direction."));
  Assert (num_visualization_points_[2] > 1,
          ExcMessage ("Number of visualization points must be > 1 in every direction."));

  const bool unstructured = grid_type == 0;

  const auto names = this->get_map_names ();

  unsigned int block_index = 0;
  if (create_control_mesh)
  {
    const auto& n = names[0];
    vtkMultiBlockDataSet* internal_mb =
      vtkMultiBlockDataSet::SafeDownCast(mb->GetBlock(block_index++));
    Assert (internal_mb != nullptr, ExcNullPtr ());
    internal_mb->SetNumberOfBlocks(n.size ());

    unsigned int internal_block_index = 0;
    for (const auto& name : n)
      internal_mb->GetMetaData(internal_block_index++)->Set(vtkCompositeDataSet::NAME(), name.c_str());

    internal_block_index = 0;
//     this->generate_control_mesh_grid<2, 0>(internal_mb, internal_block_index);
//     this->generate_control_mesh_grid<1, 1>(internal_mb, internal_block_index);
//     this->generate_control_mesh_grid<3, 0>(internal_mb, internal_block_index);
//     this->generate_control_mesh_grid<2, 1>(internal_mb, internal_block_index);
//     this->generate_control_mesh_grid<1, 2>(internal_mb, internal_block_index);
  }

  if (create_parametric_mesh)
  {
    const auto& n = names[1];
    vtkMultiBlockDataSet* internal_mb =
      vtkMultiBlockDataSet::SafeDownCast(mb->GetBlock(block_index++));
    Assert (internal_mb != nullptr, ExcNullPtr ());
    internal_mb->SetNumberOfBlocks(n.size ());
    const bool is_parametric = true;

    unsigned int internal_block_index = 0;
    for (const auto& name : n)
      internal_mb->GetMetaData(internal_block_index++)->Set(vtkCompositeDataSet::NAME(), name.c_str());

    internal_block_index = 0;
    this->generate_solid_mesh_grids<2, 0>(internal_mb, internal_block_index, unstructured, is_parametric);
    this->generate_solid_mesh_grids<1, 1>(internal_mb, internal_block_index, unstructured, is_parametric);
    this->generate_solid_mesh_grids<3, 0>(internal_mb, internal_block_index, unstructured, is_parametric);
    this->generate_solid_mesh_grids<2, 1>(internal_mb, internal_block_index, unstructured, is_parametric);
    this->generate_solid_mesh_grids<1, 2>(internal_mb, internal_block_index, unstructured, is_parametric);
  }

  if (create_physical_mesh)
  {
    const auto& n = names[2];
    vtkMultiBlockDataSet* internal_mb =
      vtkMultiBlockDataSet::SafeDownCast(mb->GetBlock(block_index++));
    Assert (internal_mb != nullptr, ExcNullPtr ());
    internal_mb->SetNumberOfBlocks(n.size ());
    const bool is_parametric = false;

    unsigned int internal_block_index = 0;
    for (const auto& name : n)
      internal_mb->GetMetaData(internal_block_index++)->Set(vtkCompositeDataSet::NAME(), name.c_str());

    internal_block_index = 0;
    this->generate_solid_mesh_grids<2, 0>(internal_mb, internal_block_index, unstructured, is_parametric);
    this->generate_solid_mesh_grids<1, 1>(internal_mb, internal_block_index, unstructured, is_parametric);
    this->generate_solid_mesh_grids<3, 0>(internal_mb, internal_block_index, unstructured, is_parametric);
    this->generate_solid_mesh_grids<2, 1>(internal_mb, internal_block_index, unstructured, is_parametric);
    this->generate_solid_mesh_grids<1, 2>(internal_mb, internal_block_index, unstructured, is_parametric);
  }
};



template <int dim, int codim>
void
IGAVTK::
generate_solid_mesh_grids(vtkMultiBlockDataSet* const mb,
                          unsigned int& id,
                          const bool unstructured,
                          const bool is_parametric) const
{
  const auto mappings = funcs_container_->template get_all_mappings<dim, codim>();

  for (const auto &m : mappings)
  {
      using Fun_ = Function<dim, 0, dim + codim, 1>;
      shared_ptr<Fun_> mapping = m.first;

      if (is_identity_mapping<dim, codim>(mapping) != is_parametric)
        continue;

      vtkSmartPointer<vtkPoints>   points = vtkSmartPointer<vtkPoints>::New();
      vtkSmartPointer<vtkCellArray> cellsArray = vtkSmartPointer<vtkCellArray>::New();

      // Generating visualization quadrature.
      // TODO: quadratures for 1D, 2D and 3D should be moved outside this
      // function.

      TensorSize<dim> n_points;
      for (int dir = 0; dir < dim; ++dir)
      {
        n_points[dir] = num_visualization_points_[dir];
        Assert (n_points[dir] > 1, ExcMessage ("Wrong number of visualization points."));
      }
      QUniform<dim> quad (n_points);

      const int n_bezier_elements = mapping->get_grid()->get_num_all_elems();
      const int n_points_per_bezier_element = quad.get_num_points();
      const int total_num_points = n_points_per_bezier_element * n_bezier_elements;

      // Setting the points --------------------------------------------------//
      points->SetNumberOfPoints (total_num_points);

      auto flag = ValueFlags::point | ValueFlags::value;
      mapping->reset(flag, quad);

      auto elem = mapping->begin();
      const auto end = mapping->end();

      const auto topology = Topology<dim>();
      mapping->init_cache(elem, Topology<dim>());

      const auto point_num_map =
        create_points_numbering_map(mapping->get_grid(),
                                    n_points, unstructured);

      Assert (point_num_map.size() == n_bezier_elements,
              ExcDimensionMismatch(point_num_map.size(), n_bezier_elements));
#ifndef NDEBUG
      for (const auto& it : point_num_map)
        Assert (it.size() == n_points_per_bezier_element,
              ExcDimensionMismatch(it.size(), n_points_per_bezier_element));
#endif

      double point_tmp[3] = {0.0, 0.0, 0.0};
      auto pnm_it = point_num_map.cbegin();
      for (; elem != end; ++elem, ++pnm_it)
      {
        mapping->fill_cache(elem, topology, 0);

        auto element_vertices_tmp = elem->template get_values<_Value, dim>(0);
        auto pnm = pnm_it->cbegin();
        for (const auto& p : element_vertices_tmp)
        {
          for (int dir = 0; dir < dim ; ++dir)
            point_tmp[dir] = p[dir];
          points->SetPoint (*pnm++, point_tmp);
        }
      }
      //----------------------------------------------------------------------//

      if (unstructured) // Creating an unstuctured grid.
      {
        // Setting the cells -------------------------------------------------//
        int n_cells_per_bezier = 1;
        for (int dir = 0; dir < dim; ++dir)
          n_cells_per_bezier *= num_visualization_points_[dir] - 1;
        const int n_total_cells = n_bezier_elements * n_cells_per_bezier;

        const auto cell_ids = IGAVTK::create_vtu_cell_ids<dim>
          (n_points, n_bezier_elements);
        cellsArray->SetCells (n_total_cells , cell_ids);

        //--------------------------------------------------------------------//
        vtkSmartPointer<vtkUnstructuredGrid> grid = vtkSmartPointer<vtkUnstructuredGrid>::New();
        grid->Allocate(cellsArray->GetNumberOfCells (), 0);
        grid->SetPoints(points);
        const int vtk_enum_type = dim == 3 ? VTK_HEXAHEDRON :
                                  dim == 2 ? VTK_QUAD :
                                            VTK_LINE;
        grid->SetCells(vtk_enum_type, cellsArray);
        mb->SetBlock (id, grid);
      }
      else  // Creating a stuctured grid.
      {
        vtkSmartPointer<vtkStructuredGrid> grid = vtkSmartPointer<vtkStructuredGrid>::New();

        const auto grid_elem = mapping->get_grid()->get_num_intervals ();
        if (dim == 1)
          grid->SetDimensions(num_visualization_points_[0] * grid_elem[0], 1, 1);
        else if (dim == 2)
          grid->SetDimensions(num_visualization_points_[0] * grid_elem[0],
                              num_visualization_points_[1] * grid_elem[1], 1);
        else if (dim == 3)
          grid->SetDimensions(num_visualization_points_[0] * grid_elem[0],
                              num_visualization_points_[1] * grid_elem[1],
                              num_visualization_points_[2] * grid_elem[2]);

        grid->SetPoints(points);
        mb->SetBlock (id, grid);
      }

      ++id;
  }
};



void
IGAVTK::
parse_file ()
{
  // TODO: this is a temporary solution for avoiding runtime error in deserialization.
//   ifstream xml_istream(file_name_);
//   IArchive xml_in(xml_istream);
//   xml_in >> BOOST_SERIALIZATION_NVP (funcs_container_);
//   xml_istream.close();
//   Assert here if funcs_container_ is void.
  this->template create_geometries<3>();
};

#include <igatools/io/reader.h>


template <int dim>
void
IGAVTK::
create_geometries ()
{
  using std::dynamic_pointer_cast;
  using std::const_pointer_cast;

  static const int codim = 0;
  static const int space_dim = dim + codim;
  static const int range = dim;
  static const int rank = 1;
  static const Transformation transf = Transformation::h_grad;

  using Fun_ = Function<dim, codim, range, rank>;
  using IgFun_ = IgFunction<dim, codim, range, rank>;
  using PhysSpace_ = PhysicalSpace<dim, range, rank, codim, transf>;
  using RefSpace_ = ReferenceSpace<dim, range, rank>;
  using IdFun_ = IdentityFunction<dim, space_dim>;

  // File names;
  const string fname_0 = "patch_0_" + std::to_string (dim) + "D.xml";
  const string fname_1 = "patch_1_" + std::to_string (dim) + "D.xml";
  const string fname_2 = "patch_2_" + std::to_string (dim) + "D.xml";
  const string fname_3 = "patch_3_" + std::to_string (dim) + "D.xml";

  // Reading maps
  const auto map_0 = get_mapping_from_file<dim, codim> (fname_0);
  const auto map_1 = get_mapping_from_file<dim, codim> (fname_1);
  const auto map_2 = get_mapping_from_file<dim, codim> (fname_2);
  const auto map_3 = get_mapping_from_file<dim, codim> (fname_3);

  // Transforming mapping to ig mapping.
  const auto ig_func_0 = dynamic_pointer_cast<IgFun_>(map_0);
  const auto ig_func_1 = dynamic_pointer_cast<IgFun_>(map_1);
  const auto ig_func_2 = dynamic_pointer_cast<IgFun_>(map_2);
  const auto ig_func_3 = dynamic_pointer_cast<IgFun_>(map_3);

  // Getting ig spaces.
  const auto space_0 = ig_func_0->get_ig_space ();
  const auto space_1 = ig_func_1->get_ig_space ();
  const auto space_2 = ig_func_2->get_ig_space ();
  const auto space_3 = ig_func_3->get_ig_space ();

  // Getting reference spaces.
  const auto ref_space_0 = const_pointer_cast<RefSpace_>(dynamic_pointer_cast<const RefSpace_>(space_0));
  const auto ref_space_1 = const_pointer_cast<RefSpace_>(dynamic_pointer_cast<const RefSpace_>(space_1));
  const auto ref_space_2 = const_pointer_cast<RefSpace_>(dynamic_pointer_cast<const RefSpace_>(space_2));
  const auto ref_space_3 = const_pointer_cast<RefSpace_>(dynamic_pointer_cast<const RefSpace_>(space_3));

  // Building physical spaces.
  const auto phys_space_0 = PhysSpace_::create (ref_space_0, map_0);
  const auto phys_space_1 = PhysSpace_::create (ref_space_1, map_1);
  const auto phys_space_2 = PhysSpace_::create (ref_space_2, map_2);
  const auto phys_space_3 = PhysSpace_::create (ref_space_3, map_3);

  // Getting grids;
  const auto grid_0 = ref_space_0->get_grid ();
  const auto grid_1 = ref_space_1->get_grid ();
  const auto grid_2 = ref_space_2->get_grid ();
  const auto grid_3 = ref_space_3->get_grid ();

  // Creating identity functions.
  auto id_map_0 = IdFun_::create (grid_0);
  auto id_map_1 = IdFun_::create (grid_1);
  auto id_map_2 = IdFun_::create (grid_2);
  auto id_map_3 = IdFun_::create (grid_3);

  Epetra_SerialComm comm;

  // Creating coefficients for ig physical space functions.
  auto phys_coeff_0 = EpetraTools::create_vector(
                        EpetraTools::create_map(phys_space_0, "active", comm));
  (*phys_coeff_0)[0] = 0.;
  auto phys_coeff_1 = EpetraTools::create_vector(
                        EpetraTools::create_map(phys_space_1, "active", comm));
  (*phys_coeff_1)[0] = 1.;
  auto phys_coeff_2 = EpetraTools::create_vector(
                        EpetraTools::create_map(phys_space_2, "active", comm));
  (*phys_coeff_2)[0] = 2.;
  auto phys_coeff_3 = EpetraTools::create_vector(
                        EpetraTools::create_map(phys_space_3, "active", comm));
  (*phys_coeff_3)[0] = 3.;


  // Creating coefficients for ig reference space functions.
  auto ref_coeff_0 = EpetraTools::create_vector(
                        EpetraTools::create_map(ref_space_0, "active", comm));
  (*ref_coeff_0)[0] = 4.;
  auto ref_coeff_1 = EpetraTools::create_vector(
                        EpetraTools::create_map(ref_space_1, "active", comm));
  (*ref_coeff_1)[0] = 5.;
  auto ref_coeff_2 = EpetraTools::create_vector(
                        EpetraTools::create_map(ref_space_2, "active", comm));
  (*ref_coeff_2)[0] = 6.;
  auto ref_coeff_3 = EpetraTools::create_vector(
                        EpetraTools::create_map(ref_space_3, "active", comm));
  (*ref_coeff_3)[0] = 7.;

  // Creating ig functions for physical spaces.
  auto ps_func_0 = dynamic_pointer_cast<Fun_>(IgFun_::create (phys_space_0, phys_coeff_0));
  auto ps_func_1 = dynamic_pointer_cast<Fun_>(IgFun_::create (phys_space_1, phys_coeff_1));
  auto ps_func_2 = dynamic_pointer_cast<Fun_>(IgFun_::create (phys_space_2, phys_coeff_2));
  auto ps_func_3 = dynamic_pointer_cast<Fun_>(IgFun_::create (phys_space_3, phys_coeff_3));

  // Creating ig functions for reference spaces.
  auto rf_func_0 = dynamic_pointer_cast<Fun_>(IgFun_::create (ref_space_0, ref_coeff_0));
  auto rf_func_1 = dynamic_pointer_cast<Fun_>(IgFun_::create (ref_space_1, ref_coeff_1));
  auto rf_func_2 = dynamic_pointer_cast<Fun_>(IgFun_::create (ref_space_2, ref_coeff_2));
  auto rf_func_3 = dynamic_pointer_cast<Fun_>(IgFun_::create (ref_space_3, ref_coeff_3));

  // Adding all the stuff to the functions container.
  funcs_container_ = std::make_shared<FunctionsContainer>();

  // Inserting geometries.
  funcs_container_->insert_mapping(map_0, "map_0");
  funcs_container_->insert_mapping(map_1, "map_1");
  funcs_container_->insert_mapping(map_2, "map_2");
  funcs_container_->insert_mapping(map_3, "map_3");

  funcs_container_->insert_mapping(id_map_0, "id_map_0");
  funcs_container_->insert_mapping(id_map_1, "id_map_1");
  funcs_container_->insert_mapping(id_map_2, "id_map_2");
  funcs_container_->insert_mapping(id_map_3, "id_map_3");

  // Inserting associated functions.
  funcs_container_->insert_function(map_0, ps_func_0, "phys_func_0");
  funcs_container_->insert_function(map_1, ps_func_1, "phys_func_1");
  funcs_container_->insert_function(map_2, ps_func_2, "phys_func_2");
  funcs_container_->insert_function(map_3, ps_func_3, "phys_func_3");

  funcs_container_->insert_function(id_map_0, rf_func_0, "ref_func_0");
  funcs_container_->insert_function(id_map_1, rf_func_1, "ref_func_1");
  funcs_container_->insert_function(id_map_2, rf_func_2, "ref_func_2");
  funcs_container_->insert_function(id_map_3, rf_func_3, "ref_func_3");
};


template <int dim, int codim>
bool
IGAVTK::
is_identity_mapping (shared_ptr<Function<dim, 0, dim+codim, 1>> map) const
{
  using IdFun_ = IdentityFunction<dim, dim+codim>;
  const auto id_func = std::dynamic_pointer_cast<IdFun_>(map);
  return (id_func != nullptr);
};




SafeSTLArray<SafeSTLVector<string>, 3>
IGAVTK::
get_map_names () const
{
  SafeSTLArray<SafeSTLVector<string>, 3> names;
  auto& all_names = names[0];
  auto& parametric_names = names[1];
  auto& mapped_names = names[2];

  for (const auto& m : funcs_container_->template get_all_mappings<2, 0>())
  {
    auto mapping = m.first;
    auto name    = m.second;
    if (this->is_identity_mapping<2, 0>(mapping))
      parametric_names.push_back(name);
    else
      mapped_names.push_back(name);
    all_names.push_back(name);
  }

  for (const auto& m : funcs_container_->template get_all_mappings<1, 1>())
  {
    auto mapping = m.first;
    auto name    = m.second;
    if (this->is_identity_mapping<1, 1>(mapping))
      parametric_names.push_back(name);
    else
      mapped_names.push_back(name);
    all_names.push_back(name);
  }

  for (const auto& m : funcs_container_->template get_all_mappings<3, 0>())
  {
    auto mapping = m.first;
    auto name    = m.second;
    if (this->is_identity_mapping<3, 0>(mapping))
      parametric_names.push_back(name);
    else
      mapped_names.push_back(name);
    all_names.push_back(name);
  }

  for (const auto& m : funcs_container_->template get_all_mappings<2, 1>())
  {
    auto mapping = m.first;
    auto name    = m.second;
    if (this->is_identity_mapping<2, 1>(mapping))
      parametric_names.push_back(name);
    else
      mapped_names.push_back(name);
    all_names.push_back(name);
  }

  for (const auto& m : funcs_container_->template get_all_mappings<1, 2>())
  {
    auto mapping = m.first;
    auto name    = m.second;
    if (this->is_identity_mapping<1, 2>(mapping))
      parametric_names.push_back(name);
    else
      mapped_names.push_back(name);
    all_names.push_back(name);
  }

  return names;
};



template <int dim>
vtkSmartPointer<vtkIdTypeArray>
IGAVTK::
create_vtu_cell_ids (const TensorSize<dim>& n_points_per_direction,
                     const Size& n_bezier_elements)
{
  vtkSmartPointer<vtkIdTypeArray> cell_ids = vtkSmartPointer<vtkIdTypeArray>::New();

  const auto grid = CartesianGrid<dim>::create (n_points_per_direction);

  static constexpr iga::Size n_points_per_single_cell = pow (2, dim);
  const int n_cells_per_bezier = grid->get_num_all_elems ();
  const int n_points_per_bezier_element = n_points_per_direction.flat_size ();
  const int n_total_cells = n_bezier_elements * n_cells_per_bezier;

  // Creating the connectivity base ------------------------------------------//
  Connectivity_t_<dim> connectivity_base (n_cells_per_bezier);

  // Building the offsets container. According to the vtk elements connectivity.
  using T_ = SafeSTLArray < SafeSTLArray<int, dim>, n_points_per_single_cell>;
  const  T_ delta_idx =
    dim == 1 ? T_({{0},       {1}})                          :   // dim = 1
    dim == 2 ? T_({{0, 0},    {1, 0},    {1, 1},    {0, 1}}) :   // dim = 2
               T_({{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0},
                   {0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}}); // dim = 3

  const TensorIndex<dim> weight_points =
    MultiArrayUtils< dim >::compute_weight(n_points_per_direction);

  TensorIndex<dim> vtk_vertex_tensor_idx;

  auto conn_el = connectivity_base.begin ();
  auto elem = grid->begin();
  const auto elem_end = grid->end();
  for (; elem != elem_end; ++elem, ++conn_el)
  {
    SafeSTLArray<Index,dim> vtk_elem_tensor_idx = elem->get_tensor_index();

    auto conn = conn_el->begin ();
    for (int iVertex = 0; iVertex < n_points_per_single_cell; ++iVertex, ++conn)
    {
        for (int i = 0; i < dim; ++i)
          vtk_vertex_tensor_idx[i] = vtk_elem_tensor_idx[i] + delta_idx[iVertex][i];

        *conn = MultiArrayUtils<dim>::tensor_to_flat_index(vtk_vertex_tensor_idx, weight_points);
    }
  }
  //--------------------------------------------------------------------------//

  cell_ids->SetNumberOfComponents (n_points_per_single_cell + 1);
  cell_ids->SetNumberOfTuples (n_total_cells);

  const int tuple_size = n_points_per_single_cell + 1;
  int cell_id = 0;
  for (int be = 0; be < n_bezier_elements; ++be)
  {
    const int vtk_vertex_id_offset = n_points_per_bezier_element * be;
    for (const auto& cc : connectivity_base)
    {
      vtkIdType* tuple = new vtkIdType[tuple_size];
      tuple[0] = n_points_per_single_cell;
      int point_id = 1;
      for (const auto& c : cc)
        tuple[point_id++] = c + vtk_vertex_id_offset;
      cell_ids->SetTupleValue(cell_id++, tuple);
    }
  }

  return cell_ids;
};



template <int dim>
SafeSTLVector<SafeSTLVector<Index>>
IGAVTK::
create_points_numbering_map (const shared_ptr<const CartesianGrid<dim>> grid,
                             const TensorSize<dim>& n_pts_per_vtk_elem,
                             const bool is_unstructured) const
{
  const Size num_elems = grid->get_num_all_elems();

  Size n_total_pts_per_element = 1;
  for (int dir = 0; dir < dim; ++dir)
    n_total_pts_per_element *= n_pts_per_vtk_elem[dir];

  SafeSTLVector<SafeSTLVector<Index>>
  points_map (num_elems, SafeSTLVector<Index> (n_total_pts_per_element));

  if (is_unstructured) // Unstructured grid.
  {
    Index point_id = 0;
    for (auto &npm_el : points_map)
      for (auto &npm : npm_el)
        npm = point_id++;
  }
  else // Structured grid.
  {
    TensorSize<dim>  n_pts_per_mesh;   // Number of points per direction of
                                       // vtk mesh.
    const auto grid_elems = grid->get_num_intervals();

    for (int dir = 0 ; dir < dim ; ++dir)
      n_pts_per_mesh[dir] = grid_elems[dir] * n_pts_per_vtk_elem[dir];

    TensorIndex<dim> elem_t_id;        // Tensorial index of the Bezier element.
    TensorIndex<dim> pt_mesh_t_offset; // Tensorial index of the first point in
                                       // Bezier element.
    TensorIndex<dim> pt_mesh_t_id;     // Tensorial index of the point.
    TensorIndex<dim> pt_elem_t_id;     // Tensorial index of the point referred
                                       // to the number of points in a
                                       // single element.

    const auto w_elem_pts = MultiArrayUtils<dim>::compute_weight(n_pts_per_vtk_elem);
    const auto w_mesh_pts = MultiArrayUtils<dim>::compute_weight(n_pts_per_mesh);

    Index i_el = 0;
    for (auto &nmp_el : points_map)
    {
      // Computing the tensor index of the first point of the element.
      elem_t_id = grid->flat_to_tensor(i_el++);
      for (int dir = 0 ; dir < dim ; ++dir)
        pt_mesh_t_offset[dir] = elem_t_id[dir] * n_pts_per_vtk_elem[dir];

      Index point_id = 0;
      for (auto &nmp : nmp_el) // Iterating along the points in a Bezier element.
      {
        // Computing the tensor index of the point referred to the number
        // of points into a single Bezier element.
        pt_elem_t_id = MultiArrayUtils<dim>::flat_to_tensor_index(point_id++, w_elem_pts);

        // Computing the tensor index of the point referred to the number
        // of points into the whole mesh.
        for (int dir = 0 ; dir < dim ; ++dir)
          pt_mesh_t_id[dir] = pt_mesh_t_offset[dir] + pt_elem_t_id[dir];

        nmp = MultiArrayUtils<dim>::tensor_to_flat_index(pt_mesh_t_id, w_mesh_pts);
      }
    }
  }

  return points_map;
};