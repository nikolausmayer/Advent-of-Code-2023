#!/bin/bash

set -eu -o pipefail;


set_power_sum=0;
while read l; do 
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
  
  set_power_sum=`expr $set_power_sum + $max_red_count \* $max_green_count \* $max_blue_count`;
done < $1;

echo "Sum of power of minimal cube sets: $set_power_sum";

