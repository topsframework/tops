
########################################################################
##                    ToPS Makefile Configuration                     ##
########################################################################

# Project settings
# ==================
PROJECT         := ToPS
VERSION         := 2.0.0
GIT_REMOTE_PATH := git@bitbucket.org:igorbonadio/topspp.git

# Project info
# ==============
COPYRIGHT       := ToPS
MAINTEINER_NAME := Ígor Bonadio,\
                   Renato Cordeiro Ferreira
MAINTEINER_MAIL := igorbonadio@gmail.com,\
                   renatocordeiroferreira@gmail.com
SYNOPSIS        := Toolkit of Probabilistic Sequences (ToPS)
DESCRIPTION     := ToPS is an objected-oriented framework that  \
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
GIT_DEPENDENCY  :=
WEB_DEPENDENCY  := # Same as above, but for URL downloads
                   # with 'curl -o' (default) or 'wget -O'

# Paths
# =======
ASLIBS          := # Assembly paths
CLIBS           := # C paths
CXXLIBS         :=
LDLIBS          :=

# Flags
# =======
CPPFLAGS        := # Precompiler Flags
ASFLAGS         := # Assembly Flags
CFLAGS          := # C Flags
CXXFLAGS        := -ansi -Wall -O2 -std=c++14 \
                   # -pedantic
LDFLAGS         :=

# Makeball list
# ===============
include conf/googletest.mk
include conf/googlebenchmark.mk
