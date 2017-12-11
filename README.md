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

# Shapes

Source code: [shapes.c](shapes.c)

Examples of various shapes defined by SDF:

![ ](shapes.png)

![ ](m.png)

# Reflection

Source code: [reflection.c](reflection.c)

![ ](vector_reflect.png)

Test scene with two boxes:

![ ](reflection_boxscene.png)

Visualization of SDF gradient, which is approximated by central difference:

![ ](reflection_boxgradient.png)

Reflection via recursive tracing:

![ ](reflection_box.png)

Concave mirror scene generates caustics effect:

![ ](reflection_concavemirror.png)

# Refraction

Source code: [refraction.c](refraction.c)

Applying Snell's law to compute refraction direction. Total internal reflection is also handled.

![ ](vector_refract.png)

Test scenes:

![ ](refraction_box.png)

![ ](refraction_concavelens.png)

![ ](refraction_convexlens.png)

![ ](refraction_semicircular.png)

![ ](m2.png)

# Fresnel Reflectance

Source code: [fresnel.c](fresnel.c)

Applying Fresnel equation to compute reflectance of dielectric medium.

![ ](vector_fresnel.png)

Without Fresnel:

![ ](refraction_montage.png)

With Fresnel term:

![ ](fresnel_montage.png)

# Beer-Lambert

Source code: [beerlambert.c](beerlambert.c) [beerlambert_color.c](beerlambert_color.c)

Applying Beer-Lambert law to simulate absorption of light in medimum.

![ ](beerlambert_montage.png)

![ ](beerlambert_color.png)

![ ](heart.png)
