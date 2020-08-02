#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Apr  8 22:29:56 2020

@author: gionuno
"""

import cython;
from libcpp.vector cimport vector;
import numpy as np;

cdef extern from "libfwmf.hpp":
    cdef vector[vector[int]] fwmf_cpp(vector[vector[int]] &,vector[vector[int]] &,vector[vector[double]] &,int,int,int);

def fast_weighted_median_filter(X,Y,D,nX,nY,R):
    return np.array(fwmf_cpp(X,Y,D,nX,nY,R));

