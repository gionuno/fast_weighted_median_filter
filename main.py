#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Apr  8 22:57:17 2020

@author: gionuno
"""

import numpy as np;
import numpy.random as rd;

from libfwmf import fast_weighted_median_filter;

import matplotlib.pyplot as plt;
import matplotlib.image  as img;

im = img.imread("zebras.jpg");

I = 256;

H = np.zeros((I,3));
for x in range(im.shape[0]):
    for y in range(im.shape[1]):
        for k in range(3):
            H[im[x,y,k]] += 1.0;

sig = 5.0*I;

D = np.zeros((I,I));
for i in range(I):
    for j in range(I):
        D[i,j] = np.exp(-0.5*((i-j)/sig)**2);
D[i,:] /= np.max(D);

jm = np.zeros(im.shape,dtype=np.uint8);
for k in range(3):
    jm[:,:,k] = fast_weighted_median_filter(im[:,:,k],im[:,:,k],D,I,I,3);


fig, ax = plt.subplots(1,3);

ax[0].imshow(im/255.0);
ax[1].imshow(jm/255.0);
ax[2].imshow(np.abs(im/255.0-jm/255.0));
plt.show();