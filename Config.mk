
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
BIN             := Hello
TESTBIN         := test

# Dependencies
# ==============
GIT_DEPENDENCY  := # List of git dependencies in the form: 
                   # DEP_NAME => dep_path build_cmd        
WEB_DEPENDENCY  := # Same as above, but for URL downloads  
                   # with 'curl -o' (default) or 'wget -O' 

# Paths
# =======
ASLIBS          := # Assembly paths
CLIBS           := # C paths
CXXLIBS         := # C++ paths
LDLIBS          := # Linker paths

# Flags
# =======
CPPFLAGS        := # Precompiler Flags
ASFLAGS         := # Assembly Flags
CFLAGS          := # C Flags
CXXFLAGS        := -ansi -Wall -pedantic -O2 -std=c++11
LDFLAGS         := # Linker flags

# Makeball list
# ===============
include conf/googletest.mk
