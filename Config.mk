
########################################################################
##                     ToPS Makefile Configuration                    ##
########################################################################

# Project settings
# ==================
PROJECT         := ToPS
VERSION         := 2.0.0
GIT_REMOTE      := git@bitbucket.org:igorbonadio/topspp.git

# Project info
# ===============
COPYRIGHT       := ToPS
MAINTEINER_NAME := Renato Cordeiro Ferreira,\
                   Ígor Bonadio
MAINTEINER_MAIL := renatocordeiroferreira@gmail.com,\
                   igorbonadio@gmail.com
SYNOPSIS        := Toolkit of Probabilistic Sequences (ToPS)
DESCRIPTION     := ToPS is an objected-oriented framework that  \
                   facilitates the integration of probabilistic \
                   models for sequences over a user defined alphabet.

# Program settings
# ==================
BIN      := Hello
TESTBIN  := test

# Dependencies
# ==============
GIT_DEPENDENCY := \
    gmock       => http://git.chromium.org/external/googlemock.git\
                   cd make && make gmock.a && mv gmock.a libgmock.a,\
    gmock/gtest => http://git.chromium.org/external/googletest.git\
                   cd make && make gtest.a && mv gtest.a libgtest.a

# Flags
# =======
CXXFLAGS += -ansi -Wall -pedantic -O2 -std=c++11 -pthread
LDFLAGS  := -pthread -lgmock -lgtest
LDLIBS   := -L external/gmock/gtest/make/ -L external/gmock/make/ 
