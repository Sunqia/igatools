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

#ifndef MULTI_PATCH_SPACE_H_
#define MULTI_PATCH_SPACE_H_

#include <igatools/base/config.h>
#include <igatools/base/logstream.h>


#include <memory>

IGA_NAMESPACE_OPEN

/**
 * @brief This class represents a space built upon several patches, where each patch is a different
 * instance of a given PhysicalSpace type.
 *
 * @note Some restrictions must be ensured for building a MultiPatchSpace:
 * - each patch is defined by one and only one PhysicalSpace object (i.e. a given PhysicalSpace object
 * cannot be used to define more than one patch).
 * - each PhysicalSpace object must be build by one and only one RefSpace object
 * (i.e. a given RefSpace object cannot be used to define more than one PhysicalSpace).
 * - each PhysicalSpace object must be build by one and only one PushFwd object
 * (i.e. a given PushFwd object cannot be used to define more than one PhysicalSpace).
 * - each PushFwd object must be build by one and only one Mapping object
 * (i.e. a given Mapping object cannot be used to define more than one PushFwd).
 */
template <class PhysicalSpace>
class MultiPatchSpace
{
public:
    /** @names Type aliases used within this class */
    ///@{
    /** Type alias for the reference space. */
    using RefSpace = typename PhysicalSpace::RefSpace;

    using PushForward = typename PhysicalSpace::PushForwardType;

    using Map = typename PushForward::Map;

    using Patch = std::shared_ptr<const PhysicalSpace>;

    static const int dim = PhysicalSpace::dim;
    ///@}


    /** @name Constructors */
    ///@{
    /** Default constructor. */
    MultiPatchSpace() = default;

    /** Copy constructor. */
    MultiPatchSpace(const MultiPatchSpace<PhysicalSpace> &multi_patch_space) = delete;

    /** Move constructor. */
    MultiPatchSpace(MultiPatchSpace<PhysicalSpace> &&multi_patch_space) = delete;

    /** Destructor. */
    ~MultiPatchSpace() = default;
    ///@}


    /** @name Assignment operators */
    ///@{
    /** Copy assignment operator.*/
    MultiPatchSpace<PhysicalSpace> &operator=(const MultiPatchSpace<PhysicalSpace> &multi_patch_space) = delete;

    /** Move assignment operator.*/
    MultiPatchSpace<PhysicalSpace> &operator=(MultiPatchSpace<PhysicalSpace> &&multi_patch_space) = delete;
    ///@}

    /** @name Functions for the management of the patches and/or interfaces addition. */
    ///@{
    void arrangement_open();
    void arrangement_close();

    /**
     * Adds a patch to the space.
     * @note In Debug mode, an assertion will be raised if the patch is already added.
     *
     */
    void add_patch(std::shared_ptr<const PhysicalSpace> patch);

    /**
     * Adds an interface between two different patches.
     * @note In Debug mode, an assertion will be raised if the interface is already added.
     */
    void add_interface(const InterfaceType &type,
                       Patch patch_0,const int side_id_patch_0,
                       Patch patch_1,const int side_id_patch_1);
    ///@}


    /** Returns the number of patches used to define this space. */
    int get_num_patches() const;


    /** Returns the number of interfaces used to define this space. */
    int get_num_interfaces() const;

    /**
     * Prints internal information about the space.
     * @note Mostly used for debugging and testing.
     */
    void print_info(LogStream &out) const;

private:


    bool is_arrangement_open_ = false;

    std::vector< std::shared_ptr<const PhysicalSpace> > patches_;

    /**
     * Renumber the dofs in the reference spaces in order to avoid same dof ids between different spaces.
     */
    void perform_ref_spaces_dofs_renumbering();


    class Interface
    {
    public:
        /** @name Constructors and destructor */
        ///@{
        Interface() = delete;
        Interface(const InterfaceType &type,
                  Patch patch_0,const int side_id_patch_0,
                  Patch patch_1,const int side_id_patch_1);
        Interface(const Interface &interface) = delete;
        Interface(Interface &&interface) = delete;
        ~Interface() = default;
        ///@}


        /** @name Assignment operators */
        ///@{
        Interface &operator=(const Interface &interface) = delete;
        Interface &operator=(Interface &&interface) = delete;
        ///@}


        /** @name Comparison operators */
        ///@{
        bool operator==(const Interface &interface_to_compare) const;
        bool operator!=(const Interface &interface_to_compare) const;
        ///@}

        /**
         * Prints internal information about the Interface.
         * @note Mostly used for debugging and testing.
         */
        void print_info(LogStream &out) const;

    private:
        InterfaceType type_;
        std::array<Patch,2> patch_;
        std::array<int,2> side_id_;
    };


    std::vector<std::unique_ptr<Interface> > interfaces_;
};


IGA_NAMESPACE_CLOSE


#endif // #ifndef MULTI_PATCH_SPACE_H_
