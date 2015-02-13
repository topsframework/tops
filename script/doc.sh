#!/usr/bin/env bash

current_banch=$(git symbolic-ref --short -q HEAD)
last_commit=$(git rev-parse HEAD)

git checkout gh-pages
rm -rf doc/api
cp -R doc/doxygen/html doc/api
git add doc/api
git commit -m "[skip ci] Add documentation of $last_commit"
git push origin gh-pages
git checkout $current_banch