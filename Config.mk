
############################################################
##               ToPS Makefile Configuration              ##
############################################################

# Project setting
PROJECT  := ToPS
VERSION  := 2.0.0

# Program settings
BIN := Hello

# SHRLIB   := # lib, all source files will make the library.

# Flags
# CXXFLAGS := # C++ Flags
# LDFLAGS  := # Linker flags

GIT_DEPENDENCY := \
    googletest => http://git.chromium.org/external/googletest.git

# Package info
MAINTEINER_NAME := Ígor Bonadio e Renato Cordeiro
MAINTEINER_MAIL := igorbonadio@gmail.com, renatocordeiroferreira@gmail.com
SYNOPSIS        := Toolkit of Probabilistic Sequences (ToPS)
DESCRIPTION     := ToPS is an objected-oriented framework that \
                   facilitates the integration of probabilistic \
                   models for sequences over a user defined alphabet.
