from distutils.core import setup, Extension
import os,sys

srcdir = "../nodejs/src/".replace("/", os.sep)
files = os.listdir(srcdir)
sources = ['klsudoku.cpp']
for f in files:
    if(f.endswith('.cpp') or f.endswith('.c')):
        sources.append(os.path.join(srcdir, f))

module1 = Extension('klsudoku', sources=sources)

setup (name = 'klsudoku', version = '1.0', description = 'Sudoku solver package.', ext_modules = [module1])