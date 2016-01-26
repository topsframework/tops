[![Tops](https://raw.githubusercontent.com/topsframework/topsframework.github.io/master/tops-cover.png)](http://topsframework.github.io/)

[Website](http://topsframework.github.io/) |
[Documentation](http://topsframework.github.io/tops/doc/api/) |
[Tutorial](http://topsframework.github.io/tutorial.pdf)

[![Build Status](https://img.shields.io/travis/topsframework/tops.svg)](https://travis-ci.org/topsframework/tops)
[![Coverage Status](https://img.shields.io/coveralls/topsframework/tops.svg)](https://coveralls.io/r/topsframework/tops)
[![Coverity Scan Build Status](https://img.shields.io/coverity/scan/4131.svg)](https://scan.coverity.com/projects/topsframework-tops)
[![Release](https://img.shields.io/github/tag/topsframework/tops.svg)](https://github.com/topsframework/tops/releases/latest)
[![License](https://img.shields.io/github/license/topsframework/tops.svg)](https://github.com/topsframework/tops/blob/master/LICENSE)

ToPS is an objected-oriented framework implemented with C++ which 
facilitates the integration of probabilistic models for sequences 
over an user defined alphabet. ToPS contains the implementation of 
eight distinct models to analyze discrete sequences:

1. Independent and identically distributed model (IID)
2. Variable-Length Markov Chain (VLMC)
3. Inhomogeneous Markov Chain (IMC)
4. Hidden Markov Model (HMM)
5. Pair Hidden Markov Model
6. Profile Hidden Markov Model
7. Similarity Based Sequence Weighting (SBSW)
8. Generalized Hidden Markov Model (GHMM)

Users can implement models either by manual description of the 
probability values in a configuration file, or by using training 
algorithms provided by the system. ToPS framework also includes 
a set of programs that implement bayesian classifiers, sequence 
samplers, and sequence decoders. Finally, ToPS is an extensible and 
portable system that facilitates the implementation of other 
probabilistic models, and the development of new programs.

Git Repository
===============

You can download the development version of ToPS by executing the 
command below:

```bash
git clone --recursive https://github.com/topsframework/tops.git
```

Platforms
==========

ToPS was designed to run on Unix/Linux operating systems. 
Tested platforms include: MacOS X, Ubuntu Linux and Arch Linux.

Software Requirement
=====================

ToPS was written in C++. It was compiled and tested using g++ 4.9.2 
and clang 3.6.0. Besides one of the compilers, it requires:

- Git
- GNU Make

Installing ToPS
================

1. Download ToPS from GitHub  

   ```bash
   git clone --recursive https://github.com/topsframework/tops.git
   ```

   This will create a directory named tops

2. Go to the tops directory:

   ```bash
   cd tops
   ```

3. Run make

   ```bash
   make
   ```

5. Run make install

   ```bash
   sudo make install
   ```

6. If you are using linux run ldconfig

   ```bash
   sudo ldconfig
   ```
