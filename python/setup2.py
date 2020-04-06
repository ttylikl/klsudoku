# coding:utf-8
from setuptools import setup, Extension
import os,sys

srcdir = "../nodejs/src/".replace("/", os.sep)
files = os.listdir(srcdir)
sources = ['klsudoku.cpp']
for f in files:
    if(f.endswith('.cpp') or f.endswith('.c')):
        sources.append(os.path.join(srcdir, f))

module1 = Extension('klsudoku', sources=sources)

setup(
        name='klsudoku',     # 包名字
        version='1.0',   # 包版本
        description='Sudoku solver package.',   # 简单描述
        author='ttylikl',  # 作者
        author_email='ttylikl@gmail.com',  # 作者邮箱
        url='https://github.com/ttylikl/klsudoku/',      # 包的主页
        #packages=['klsudoku'],                 # 包
        ext_modules=[module1],
)

#setup (name = 'klsudoku', version = '1.0', description = 'Sudoku solver package.', ext_modules = [module1])