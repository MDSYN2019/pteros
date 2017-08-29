#!/usr/bin/python

from pteros11 import *
import numpy as np
import sys

def cb(s,fr):
    print "callback with {} atoms".format(s.num_atoms())
    return True

s=System('/home/semen/work/current/Projects/Squalene/Dox_self_org/after_free1.gro')
s.load('/home/semen/work/current/Projects/Squalene/Dox_self_org/after_free1.gro',on_frame=cb)
print s.num_atoms()
print s.num_frames()
print s.getXYZ(0,0)
s.setXYZ( s.getXYZ(1,0) ,0,0)
print s.getXYZ(0,0)

sel = s()
print sel.size()

sel = s('resname LIG')
print sel.size()

sel = s(1,10)
print sel.size()


a = sel.get_index()
print type(a)
print a

sel = s([1,10,30,31,45])
print sel.size()
print sel.get_index()
sel.translate((1,1,1))

coor =  sel.get_xyz()
print coor
sel.set_xyz(coor)

b = s.getBox(0)
print "Box vectors:"
print b.get_vector(0)
print b.get_vector(1)
print b.get_vector(2)

print "Box matrix:"
print b.get_matrix()

b.scale_vectors([2,2,2])
print "Box vectors scaled x2:"
print b.get_vector(0)
print b.get_vector(1)
print b.get_vector(2)

print b.distance([1,1,1],[30,30,30],dims=[1,1,1])

v,a = b.to_vectors_angles()
print v,a
b.from_vectors_angles(v,a)

print "frame:"
fr = s.getFrame_data(0)
print fr.time
print fr.box.get_matrix()
c = fr.coord
print c[0:3]
c[0] = [1,1,1]
print c[0:3]
fr.coord = c
print fr.coord[0:3]

print "Inertia"
sel = s('resname LIG')
m,a = sel.inertia()
print m,a

t = sel.principal_transform(False)
print t

#for a in sel:
#    a.resid += 1
#    print a.name, a.resid, a.xyz

print sel[45].resname

sel = s('resid 1')
vec = np.array([],dtype=np.float32)
pairs,dist = search_contacts(0.6,sel,True,True,True)
print pairs[0]

ptr,b = pairs.__array_interface__['data']
print hex(ptr)

print dist[0:10]


sel2 = s('not resid 1')

res = search_within(0.5,sel2,sel)
print res[:10]

opt,tasks = parse_command_line(sys.argv,"task")
print opt('f').as_strings()
print tasks[0]('opt1').as_float()
