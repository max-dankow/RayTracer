#!/usr/bin/env bash
s=""
for n in $@
do
  s="$s "`find . -name "*.$n"`
done
wc -l ${s} | sort -b -g
