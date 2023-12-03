#!/bin/bash

set -eu -o pipefail;


good_IDs_sum=0;
while read l; do 
  game_ID=`echo "$l" \
    | cut -d':' -f1  \
    | cut -d' ' -f2`;

  ## Red
  max_red_count=`echo "$l"                 \
    | grep --only-matching -E '[0-9]+ red' \
    | cut -d' ' -f1                        \
    | sort -rg                             \
    | head -n1`;

  ## Green
  max_green_count=`echo "$l"                 \
    | grep --only-matching -E '[0-9]+ green' \
    | cut -d' ' -f1                          \
    | sort -rg                               \
    | head -n1`;

  ## Blue
  max_blue_count=`echo "$l"                 \
    | grep --only-matching -E '[0-9]+ blue' \
    | cut -d' ' -f1                         \
    | sort -rg                              \
    | head -n1`;
  
  if test $max_red_count   -le 12 -a \
          $max_green_count -le 13 -a \
          $max_blue_count  -le 14; then
    good_IDs_sum=`expr $good_IDs_sum + $game_ID`;
  fi
done < $1;

echo "Sum of possible games' IDs: $good_IDs_sum";

