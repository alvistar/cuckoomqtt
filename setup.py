from distutils.core import setup
from Cython.Build import cythonize
from distutils.command.build_clib import build_clib

libblake2b = ('blake2b', {'sources': ['blake2b-ref.c']})

setup(
    name = "cuckooapp",
    libraries = [libblake2b],
    ext_modules = cythonize('pycuckoo.pyx'),
)