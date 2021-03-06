#-+--------------------------------------------------------------------
# Igatools a general purpose Isogeometric analysis library.
# Copyright (C) 2012-2016  by the igatools authors (see authors.txt).
#
# This file is part of the igatools library.
#
# The igatools library is free software: you can use it, redistribute
# it and/or modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation, either
# version 3 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#-+--------------------------------------------------------------------

from init_instantiation_data import *
include_files = ['utils/element_index.h']
data = Instantiation(include_files)
(f, inst) = (data.file_output, data.inst)

containers = []
for dim in inst.sub_domain_dims + inst.domain_dims:
    element_index = 'ElementIndex<%d>' %(dim)
    obj = 'SafeSTLVector<%s>' %(element_index)
    containers.append(obj)
    containers.append('SafeSTLMap<PropId,%s>' % obj)
    obj = 'PropertiesIdContainer<%s,SafeSTLVector>' %(element_index)
    containers.append(obj)
    obj = 'PropertiesElementID<%d>' %(dim)
    containers.append(obj)

obj = 'PropertiesIdContainer<int>'
containers.append(obj)

for obj in unique(containers):
    f.write('template class %s;\n' %(obj))


f.write('IGA_NAMESPACE_CLOSE\n')


#f.write('#ifdef IGATOOLS_WITH_SERIALIZATION\n')
#id = 0 
#for quadrature in unique(containers):
#    alias = 'PropertiesIdContainerAlias%d' %(id)
#    f.write('using %s = iga::%s; \n' % (alias, quadrature))
#    f.write('BOOST_CLASS_EXPORT_IMPLEMENT(%s) \n' %alias)
#    f.write('template void %s::serialize(OArchive &, const unsigned int);\n' % alias)
#    f.write('template void %s::serialize(IArchive &, const unsigned int);\n' % alias)
#    id += 1 
#f.write('#endif // IGATOOLS_WITH_SERIALIZATION\n')
#---------------------------------------------------
f.write('IGA_NAMESPACE_OPEN\n')
