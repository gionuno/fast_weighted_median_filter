#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Apr  8 22:35:18 2020

@author: gionuno
"""
import setuptools;
from distutils.core import setup;
from distutils.extension import Extension;
from Cython.Build import cythonize;

e_modules = cythonize([Extension("libfwmf",["libfwmf.pyx"],extra_compile_args=['-std=c++11'],libraries=[],language="c++")]);
setup(name="libfwmf",ext_modules = e_modules,script_args=['build'])