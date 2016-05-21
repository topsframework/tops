
##############################################################################
##                        ToPS Makefile Configuration                       ##
##############################################################################

# Project settings
# ==================
PROJECT         := ToPS
VERSION         := 2.0.0-alpha
GIT_REMOTE_PATH := https://github.com/topsframework/tops.git
STD_NAMESPACE   := tops

# Project info
# ==============
COPYRIGHT       := ToPS
MAINTEINER_NAME := Ígor Bonadio,\
                   Renato Cordeiro Ferreira
MAINTEINER_MAIL := igorbonadio@gmail.com,\
                   renatocordeiroferreira@gmail.com
SYNOPSIS        := Toolkit of Probabilistic Sequences (ToPS)
DESCRIPTION     := ToPS is an objected-oriented framework that \
                   facilitates the integration of probabilistic \
                   models for sequences over a user defined alphabet.

# Program settings
# ==================
SHRLIB          := model
TESTBIN         := test
BENCHBIN        := bench

# Paths
# =======
CXXLIBS         += # C++ paths
LDLIBS          += # Linker paths

# Flags
# =======
CPPFLAGS        += # Precompiler Flags
CXXFLAGS        += -std=c++14 \
                   -Wall -Wextra -Wpedantic -Wshadow -Wold-style-cast \
                   -Wcast-align -Wmissing-include-dirs -Wredundant-decls \
                   -Werror -O2
LDFLAGS         += # Linker Flags

# Makeball list
# ===============
include conf/googletest.mk
include conf/googlebenchmark.mk
include conf/probability.mk
