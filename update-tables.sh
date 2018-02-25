#!/bin/bash

./gen-table.py elements > site/elements-table.adoc
./gen-table.py presentation-attributes > site/presentation-attributes-table.adoc
./gen-table.py attributes > site/attributes-table.adoc
