from distutils.core import setup, Extension

module1 = Extension('klsudoku', sources = ['klsudoku.cpp'])

setup (name = 'klsudoku', version = '1.0', description = 'Sudoku solver package.', ext_modules = [module1])