#!/bin/sh
#chmod u+x

tests="../Tests"

DIRS=`ls -l $tests | egrep '^d' | awk '{print $9}'`


ratio_file="$tests/ratios.txt"

if [ -e $ratio_file ]; then
  rm $ratio_file
else
  touch $ratio_file
fi

for dir in $DIRS 
do

  Sub_DIRS=`ls -l "$tests/$dir" | egrep '^d' | awk '{print $9}'`
  
# save reades of file
  echo "Ratio from Graph Class: $dir" >> $ratio_file
  echo  >> $ratio_file

  for sub_dir in $Sub_DIRS 
  do
    test_DIRS=`ls -l "$tests/$dir/$sub_dir" | egrep '^d' | awk '{print $9}'`
#   save reades of file
    echo "   Ratio from Graph $sub_dir" >> $ratio_file
    echo >> $ratio_file
    for test_dir in $test_DIRS 
    do
      path_test="$tests/$dir/$sub_dir/$test_dir/results_LP.txt"
      out_path="$tests/$dir/$sub_dir/$test_dir/results_LP_ration.txt"
      python3 calc_ratio_results.py $path_test

      avg_ratio=""
      min_ratio=""
      stop=1
      while IFS='' read -r line || [[ -n "$line" ]]; do
        if [ $stop -eq 1 ]
        then
          avg_ratio="$line"
          stop=$((stop+1))
        else
          min_ratio="$line"
          break
        fi
      done < "$out_path"
      printf "%s" "    $test_dir --> avg ratio: $avg_ratio  min ratio: $min_ratio   " >> $ratio_file

    done
    echo >> $ratio_file
    echo >> $ratio_file
  done
done
