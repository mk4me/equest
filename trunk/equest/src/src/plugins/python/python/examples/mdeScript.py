from plugin_python import *
import sys
sys.path += ["C:\\Users\\Wojciech\\Anaconda\\Lib\\site-packages"]

from oct2py import octave

v = v3vector()

for x in range(1,2000):
	a = Vec3(x*0.01, x*x*0.001, x*x*x*0.0000001)
	v.append(a)

vc = mde.createVectorChannel()
vc.setData(v)
vc.setFrequency(100)
vc.setName("Test1")

mde.addVectorChannel(vc)
