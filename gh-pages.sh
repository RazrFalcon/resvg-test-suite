#!/usr/bin/env bash

set -e

cd site

python3.6 gen-page.py
asciidoctor -o index.html index.adoc

cp index.html ..
cp svg-support-table.html ..
cp -a images ..
cd ..
rm -rf site

git config --global user.email "travis@travis-ci.org"
git config --global user.name "Travis CI"
git checkout --orphan gh-pages
git rm -rf .
git add *
git commit --message "Travis build: $TRAVIS_BUILD_NUMBER"
git remote add origin-pages https://${GH_TOKEN}@github.com/RazrFalcon/resvg-test-suite.git > /dev/null 2>&1
git push --force --quiet --set-upstream origin-pages gh-pages
