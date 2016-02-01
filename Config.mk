
########################################################################
##                    ToPS Makefile Configuration                     ##
########################################################################

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
BIN             :=
SHRLIB          := model
TESTBIN         := test
BENCHBIN        := bench

# Dependencies
# ==============
GIT_DEPENDENCY  +=
WEB_DEPENDENCY  += # Same as above, but for URL downloads
                   # with 'curl -o' (default) or 'wget -O'

# Paths
# =======
ASLIBS          += # Assembly paths
CLIBS           += # C paths
CXXLIBS         +=
LDLIBS          +=

# Flags
# =======
CPPFLAGS        += # Precompiler Flags
ASFLAGS         += # Assembly Flags
CFLAGS          += # C Flags
CXXFLAGS        += -std=c++14 -Wall -Wextra -Wshadow -Wcast-align -Werror -Wold-style-cast -pedantic -O2
LDFLAGS         +=

# Makeball list
# ===============
include conf/googletest.mk
include conf/googlebenchmark.mk
