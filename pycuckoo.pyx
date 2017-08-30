# distutils: language = c++
# distutils: sources = miner_api.cpp
# distutils: extra_compile_args = --std=c++11
# distutils: libraries = blake2b
# distutils: library_dirs = .
# distutils: define_macros = ATOMIC EDGEBITS=19

from cpython cimport array
import array

from libc.stdint cimport uint32_t
from libc.stdint cimport uint64_t

cdef extern from "miner_api.hpp":
    ctypedef struct solutions_t:
        uint64_t sols[8][42];
        uint32_t nsols;
    solutions_t mine(char* header, int nonce);
    int verifyhl(uint64_t solution[42], char* header, int nonce);

def cuckoo(nonce=0, headerstr=None):
    header = bytearray(80)
    if headerstr is not None:
        header[:len(headerstr)] = bytearray(headerstr, "ascii")

    cdef char* cheader = header
    cdef object solutions = mine(cheader, nonce)
    return solutions['sols'][:solutions['nsols']]

def verify(solution, header="", nonce=0):
    cdef array.array a = array.array ('Q', solution)
    cdef uint64_t[::1] sol = a
    return True if verifyhl(&sol[0], bytearray(header,"ascii"), nonce) == 1 else False
    