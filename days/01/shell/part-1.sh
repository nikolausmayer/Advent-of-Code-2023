#!/bin/bash

cat $1                         \
  | tr -d 'a-z'                \
  | sed -E 's/(.).*(.)/\1\2/'  \
  | sed -E 's/^(.)$/\1\1/'     \
  | awk '{sum += $1} END {printf sum"\n"}';

