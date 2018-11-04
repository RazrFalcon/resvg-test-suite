#!/usr/bin/env bash

set -x

./gen-page.py

asciidoctor -o site/index.html site/index.adoc

cp site/index.html .
cp site/svg-support-table.html .
cp site/favicon.ico .
cp site/fontello-embedded.css .
cp -a site/images .
rm -rf site

git config --global user.email "travis@travis-ci.org"
git config --global user.name "Travis CI"
git checkout --orphan gh-pages
git rm -rf .
git add *
git commit --message "Travis build: $TRAVIS_BUILD_NUMBER"
git remote add origin-pages https://${GH_TOKEN}@github.com/RazrFalcon/resvg-test-suite.git > /dev/null 2>&1
git push --force --quiet --set-upstream origin-pages gh-pages
