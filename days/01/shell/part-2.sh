#!/bin/bash

# sed: keep only first and last occurence of digits or digit-words
# sed: translate digit-words to digits
# tr: delete leftover letters
# sed: duplicate single digits (1 -> 11)
# awk: sum up all numbers

cat $1                         \
  | sed -E 's/([0-9]|zero|one|two|three|four|five|six|seven|eight|nine).*([0-9]|zero|one|two|three|four|five|six|seven|eight|nine)/\1\2/'  \
  | sed -e 's/zero/0/g'        \
        -e 's/one/1/g'         \
        -e 's/two/2/g'         \
        -e 's/three/3/g'       \
        -e 's/four/4/g'        \
        -e 's/five/5/g'        \
        -e 's/six/6/g'         \
        -e 's/seven/7/g'       \
        -e 's/eight/8/g'       \
        -e 's/nine/9/g'        \
  | tr -d 'a-z'                \
  | sed -E 's/^(.)$/\1\1/'     \
  | awk '{sum += $1} END {printf sum"\n"}';

