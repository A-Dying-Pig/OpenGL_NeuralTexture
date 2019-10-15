import numpy
import os

output_directory = "../output/view_normal"
filename = "0000.npy"

ret = numpy.load(os.path.join(output_directory, filename))
max = numpy.amax(ret)
print(max)
