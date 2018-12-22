#!/bin/bash

cmake -LA 2>&1 | grep INKSCAPE_VERSION | cut -d : -f 2 | awk '{$1=$1};1'

