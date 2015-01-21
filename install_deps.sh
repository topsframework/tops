#!/usr/bin/env bash

if [ "$1" == "ubuntu" ]; then
  apt-get update
  apt-get install -y build-essential
  apt-get install -y git-core
  apt-get install -y libboost-dev
  apt-get install -y libboost-system-dev
  apt-get install -y libboost-filesystem-dev
  apt-get install -y libboost-program-options-dev
  apt-get install -y libboost-regex-dev
  apt-get install -y cmake
fi
