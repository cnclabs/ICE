from distutils.core import setup, Extension
from Cython.Build import cythonize

ext_modules = Extension("pyICE",
                sources=["./pyICE.pyx"],
                libraries=["vvNet"],
                library_dirs=["./"],
                include_dirs=["./"],
                language="g++",
                extra_compile_args=["-std=c++11", "-O3", "-ffast-math", "-march=native", "-fopenmp", "-lpthread"],
                extra_link_args=["-std=c++11", "-fopenmp", "-lboost_random"]
                )

setup(
    name="pyICE",
    ext_modules=cythonize(ext_modules)
    )


