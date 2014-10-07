
############################################################
##               ToPS Makefile Configuration              ##
############################################################

# Project setting
PROJECT  := ToPS
VERSION  := 2.0.0

# Program settings
BIN      := Hello 
TESTBIN  := test

# SHRLIB   := # lib, all source files will make the library.

# Flags
CXXFLAGS += -pthread # C++ Flags
LDFLAGS  := -pthread -lgmock -lgtest
LDLIBS   := -L external/gmock/gtest/make/ -L external/gmock/make/ 

# Git
GIT_REMOTE     := git@bitbucket.org:igorbonadio/topspp.git
GIT_DEPENDENCY := \
    gmock       => http://git.chromium.org/external/googlemock.git\
                   cd make && make gmock.a && mv gmock.a libgmock.a,\
    gmock/gtest => http://git.chromium.org/external/googletest.git\
                   cd make && make gtest.a && mv gtest.a libgtest.a

# Package info
MAINTEINER_NAME := Ígor Bonadio e Renato Cordeiro
MAINTEINER_MAIL := igorbonadio@gmail.com, renatocordeiroferreira@gmail.com
SYNOPSIS        := Toolkit of Probabilistic Sequences (ToPS)
DESCRIPTION     := ToPS is an objected-oriented framework that \
                   facilitates the integration of probabilistic \
                   models for sequences over a user defined alphabet.
