#!/usr/bin/env bash
g++ main.cpp `wx-config --cxxflags --libs` -lwxsvg -o wxsvgrender
