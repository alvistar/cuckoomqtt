from distutils.core import setup
from Cython.Build import cythonize

setup(
    name = "cuckooapp",
    ext_modules = cythonize('pycuckoo.pyx'),
)