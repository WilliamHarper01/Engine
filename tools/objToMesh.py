from pywavefront import Wavefront
import sys

# Create an empty MTL file
empty_mtl = "temp.mtl"
with open(empty_mtl, "w") as mtl_file:
    mtl_file.write("")

# Load OBJ file with the empty MTL file
obj_path = sys.argv[1]
obj = Wavefront(obj_path, empty_mtl)

# Print vertices
print("Vertices:")
for vertex in obj.vertices:
    print(vertex)

