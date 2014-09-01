
############################################################
##               ToPS Makefile Configuration              ##
############################################################

# Project setting
PROJECT  := ToPS
VERSION  := 2.0.0

# Program settings
BIN := Hello test

# SHRLIB   := # lib, all source files will make the library.

# Flags
# CXXFLAGS := # C++ Flags
LDLIBS   :=
LDFLAGS  := lib/gmock/make/gmock.a lib/gmock/gtest/make/gtest.a

GIT_DEPENDENCY := \
    gmock => http://git.chromium.org/external/googlemock.git,\
    gmock/gtest => http://git.chromium.org/external/googletest.git

# Package info
MAINTEINER_NAME := Ígor Bonadio e Renato Cordeiro
MAINTEINER_MAIL := igorbonadio@gmail.com, renatocordeiroferreira@gmail.com
SYNOPSIS        := Toolkit of Probabilistic Sequences (ToPS)
DESCRIPTION     := ToPS is an objected-oriented framework that \
                   facilitates the integration of probabilistic \
                   models for sequences over a user defined alphabet.
