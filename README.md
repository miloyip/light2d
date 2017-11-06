# light2d
This project illustrates light rendering in 2D with C.

All samples output PNGs with [svpng](https://github.com/miloyip/svpng).

License: public domain.

# Basic

Use Monte Carol integration and ray marching of signed distance field (SDF) to render a emissive circle.

Source code: [basic.c](basic.c)

Uniform sampling (64 samples per pixel):

![ ](basic_a64.png)

Uniform sampling with different number of samples per pixel:

![ ](basic_varyingn.png)

Stratified sampling (64 samples per pixel):

![ ](basic_b64.png)

Jittered sampling (64 samples per pixel):

![ ](basic_c64.png)

Various sampling method side-by-side comparison (64 samples per pixel)::

![ ](basic_varyingsampling.png)

# Constructive Solid Geometry

Source code: [csg.c](csg.c)

Use union operation for creating multiple shapes:

![ ](csg_scene.png)

Various CSG operations on two circles:

![ ](csg_ops.png)
