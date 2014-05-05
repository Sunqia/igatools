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
#include <igatools/linear_algebra/distributed_vector.h>
#include <igatools/base/exceptions.h>

using std::make_shared;

#ifdef USE_TRILINOS
using Teuchos::rcp;
#endif

#ifdef USE_PETSC
#include <petscsnes.h>
#endif

IGA_NAMESPACE_OPEN

namespace
{
DeclException3(ExcVectorAccessToNonLocalElement,
               Index, Index, Index,
               << "You tried to access element (" << arg1 << ")"
               << " of a distributed vector, but only rows "
               << arg2 << " through " << arg2
               << " are stored locally and can be accessed.");

};


#ifdef USE_TRILINOS

Vector<LinearAlgebraPackage::trilinos>::
Vector(const Index num_global_dofs)
    :
    comm_(Teuchos::createSerialComm<int>()),
    vector_(Tpetra::createMultiVector<Real,Index,Index>(
                Tpetra::createUniformContigMap<Index,Index>(
                    num_global_dofs,
                    comm_),
                1))
{}



Vector<LinearAlgebraPackage::trilinos>::
Vector(const std::vector<Index> &dofs_id)
    :
    comm_(Teuchos::createSerialComm<int>()),
    vector_(Tpetra::createMultiVector<Real,Index,Index>(
                Tpetra::createNonContigMap<Index,Index>(
                    dofs_id,
                    comm_),
                1))
{}


auto
Vector<LinearAlgebraPackage::trilinos>::
create(const Index size) -> std::shared_ptr<self_t>
{
    return make_shared<self_t>(self_t(size));
}

auto
Vector<LinearAlgebraPackage::trilinos>::
create(const std::vector<Index> &dof_ids) -> std::shared_ptr<self_t>
{
    return make_shared<self_t>(self_t(dof_ids));
}



void
Vector<LinearAlgebraPackage::trilinos>::
add_entry(const Index i, const Real value)
{
    Assert(!std::isnan(value),ExcNotANumber());
    Assert(!std::isinf(value),ExcNumberNotFinite());

    vector_->sumIntoGlobalValue(i,0,value);
};


const Real &
Vector<LinearAlgebraPackage::trilinos>::
operator()(const Index global_id) const
{
    Assert(global_id < Index(vector_->getGlobalLength()),
           ExcIndexRange(global_id,0,Index(vector_->getGlobalLength()))) ;

    const auto map = vector_->getMap();
    const auto local_id = map->getLocalElement(global_id) ;

    Assert(local_id != Teuchos::OrdinalTraits<Index>::invalid(),
           ExcVectorAccessToNonLocalElement(
               global_id,
               map->getMinGlobalIndex(),
               map->getMaxGlobalIndex()));

    return (vector_->get2dView()[0][local_id]) ;
}



Real &
Vector<LinearAlgebraPackage::trilinos>::
operator()(const Index global_id)
{
    Assert(global_id < Index(vector_->getGlobalLength()),
           ExcIndexRange(global_id,0,Index(vector_->getGlobalLength()))) ;

    const auto map = vector_->getMap();
    const auto local_id = map->getLocalElement(global_id) ;

    Assert(local_id != Teuchos::OrdinalTraits<Index>::invalid(),
           ExcVectorAccessToNonLocalElement(
               global_id,
               map->getMinGlobalIndex(),
               map->getMaxGlobalIndex()));

    return (vector_->get2dViewNonConst()[0][local_id]) ;
}


Index
Vector<LinearAlgebraPackage::trilinos>::
size() const
{
    return vector_->getGlobalLength() ;
}

auto
Vector<LinearAlgebraPackage::trilinos>::
get_trilinos_vector() const -> Teuchos::RCP<const WrappedVectorType>
{
    return vector_ ;
}

auto
Vector<LinearAlgebraPackage::trilinos>::
get_trilinos_vector() -> Teuchos::RCP<WrappedVectorType>
{
    return vector_ ;
}

void
Vector<LinearAlgebraPackage::trilinos>::
add_block(
    const std::vector< Index > &local_to_global,
    const DenseVector &local_vector)
{
    Assert(!local_to_global.empty(), ExcEmptyObject()) ;
    const Index num_dofs = local_to_global.size() ;

    Assert(Index(local_vector.size()) == num_dofs,
           ExcDimensionMismatch(local_vector.size(), num_dofs)) ;

    for (Index i = 0 ; i < num_dofs ; ++i)
    {
        Assert(!std::isnan(local_vector(i)),ExcNotANumber());
        Assert(!std::isinf(local_vector(i)),ExcNumberNotFinite());
        vector_->sumIntoGlobalValue(local_to_global[i],0,local_vector(i)) ;
    }
}


std::vector<Real>
Vector<LinearAlgebraPackage::trilinos>::
get_local_coefs(const std::vector<Index> &local_to_global_ids) const
{
    std::vector<Real> local_coefs;
    for (const auto &global_id : local_to_global_ids)
        local_coefs.emplace_back((*this)(global_id));

    return local_coefs;
}


void
Vector<LinearAlgebraPackage::trilinos>::
print(LogStream &out) const
{
    using std::endl;

    out << "-----------------------------" << endl;
    // Commented as different trilinos version show different information here
    // out << *vector_ << endl;

    Teuchos::ArrayRCP<const Real> vec = vector_->getData(0) ;

    const Index n_entries = vec.size();

    out << "Global_ID        Value" << std::endl;
    for (Index i = 0 ; i < n_entries ; ++i)
        out << i << "        " << vec[i] << endl ;
    out << "-----------------------------" << endl;
}

#endif // #ifdef USE_TRILINOS



#ifdef USE_PETSC

Vector<LinearAlgebraPackage::petsc>::
Vector(const Index num_global_dofs)
{
    PetscErrorCode ierr;
    comm_ = PETSC_COMM_WORLD;
    ierr = VecCreate(comm_, &vector_);  // CHKERRQ(ierr);
    ierr = VecSetSizes(vector_, PETSC_DECIDE, num_global_dofs); // CHKERRQ(ierr);
    ierr = VecSetFromOptions(vector_); // CHKERRQ(ierr);
    ierr = VecZeroEntries(vector_); // CHKERRQ(ierr);
}



Vector<LinearAlgebraPackage::petsc>::
Vector(const std::vector<Index> &dofs_id)
{
    Assert(false,ExcNotImplemented());
    AssertThrow(false,ExcNotImplemented());
}


auto
Vector<LinearAlgebraPackage::petsc>::
create(const Index size) -> std::shared_ptr<self_t>
{
    return make_shared<self_t>(self_t(size));
}

auto
Vector<LinearAlgebraPackage::petsc>::
create(const std::vector<Index> &dof_ids) -> std::shared_ptr<self_t>
{
    return make_shared<self_t>(self_t(dof_ids));
}



void
Vector<LinearAlgebraPackage::petsc>::
add_entry(const Index i, const Real value)
{
    Assert(!std::isnan(value),ExcNotANumber());
    Assert(!std::isinf(value),ExcNumberNotFinite());

    PetscErrorCode ierr;
    ierr = VecSetValue(vector_, i, value, ADD_VALUES); // CHKERRQ(ierr);
};


const Real &
Vector<LinearAlgebraPackage::petsc>::
operator()(const Index global_id) const
{
    Assert(false,ExcNotImplemented());
    AssertThrow(false,ExcNotImplemented());
    /*
        Assert(global_id < Index(vector_->getGlobalLength()),
               ExcIndexRange(global_id,0,Index(vector_->getGlobalLength()))) ;

        const auto map = vector_->getMap();
        const auto local_id = map->getLocalElement(global_id) ;

        Assert(local_id != Teuchos::OrdinalTraits<Index>::invalid(),
               ExcVectorAccessToNonLocalElement(
                   global_id,
                   map->getMinGlobalIndex(),
                   map->getMaxGlobalIndex()));

        return (vector_->get2dView()[0][local_id]) ;
        //*/
    return real_tmp_;
}



Real &
Vector<LinearAlgebraPackage::petsc>::
operator()(const Index global_id)
{
    Assert(false,ExcNotImplemented());
    AssertThrow(false,ExcNotImplemented());
    /*
    Assert(global_id < Index(vector_->getGlobalLength()),
           ExcIndexRange(global_id,0,Index(vector_->getGlobalLength()))) ;

    const auto map = vector_->getMap();
    const auto local_id = map->getLocalElement(global_id) ;

    Assert(local_id != Teuchos::OrdinalTraits<Index>::invalid(),
           ExcVectorAccessToNonLocalElement(
               global_id,
               map->getMinGlobalIndex(),
               map->getMaxGlobalIndex()));

    return (vector_->get2dViewNonConst()[0][local_id]) ;
    //*/
    return real_tmp_;
}


Index
Vector<LinearAlgebraPackage::petsc>::
size() const
{
    PetscErrorCode ierr;
	PetscInt vector_size;
	ierr = VecGetSize(vector_, &vector_size); CHKERRQ(ierr);
	return vector_size;
}

/*
auto
Vector<LinearAlgebraPackage::petsc>::
get_petsc_vector() const -> Teuchos::RCP<const WrappedVectorType>
{
    Assert(false,ExcNotImplemented());
    AssertThrow(false,ExcNotImplemented());
//    return vector_ ;
}

auto
Vector<LinearAlgebraPackage::petsc>::
get_petsc_vector() -> Teuchos::RCP<WrappedVectorType>
{
    Assert(false,ExcNotImplemented());
    AssertThrow(false,ExcNotImplemented());
//    return vector_ ;
}
//*/

void
Vector<LinearAlgebraPackage::petsc>::
add_block(
    const std::vector< Index > &local_to_global,
    const DenseVector &local_vector)
{
    PetscErrorCode ierr;

    Assert(!local_to_global.empty(), ExcEmptyObject()) ;
    const Index num_dofs = local_to_global.size() ;

    Assert(Index(local_vector.size()) == num_dofs,
           ExcDimensionMismatch(local_vector.size(), num_dofs)) ;

    PetscScalar value;

    for (Index i = 0 ; i < num_dofs ; ++i)
    {
        Assert(!std::isnan(local_vector(i)),ExcNotANumber());
        Assert(!std::isinf(local_vector(i)),ExcNumberNotFinite());
        value = local_vector(i);
        ierr = VecSetValue(vector_, local_to_global[i], value, ADD_VALUES); //CHKERRQ(ierr);
    }



}


std::vector<Real>
Vector<LinearAlgebraPackage::petsc>::
get_local_coefs(const std::vector<Index> &local_to_global_ids) const
{
    std::vector<Real> local_coefs;
    PetscScalar *values;
    int num_local_dofs = local_to_global_ids.size();

    VecGetValues(vector_, num_local_dofs, local_to_global_ids.data(), values);

    local_coefs.assign (*values, *values+num_local_dofs);
    return local_coefs;

}


void
Vector<LinearAlgebraPackage::petsc>::
print(LogStream &out) const
{
    Assert(false,ExcNotImplemented());
    AssertThrow(false,ExcNotImplemented());
    /*
        using std::endl;

        out << "-----------------------------" << endl;
        // Commented as different petsc version show different information here
        // out << *vector_ << endl;

        Teuchos::ArrayRCP<const Real> vec = vector_->getData(0) ;

        const Index n_entries = vec.size();

        out << "Global_ID        Value" << std::endl;
        for (Index i = 0 ; i < n_entries ; ++i)
            out << i << "        " << vec[i] << endl ;
        out << "-----------------------------" << endl;
        //*/
}

#endif // #ifdef USE_PETSC

IGA_NAMESPACE_CLOSE

