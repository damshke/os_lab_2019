#!/bin/bash
count=0
for param in "$@"
do
count=$(( $count + $param ))
done
echo "Количество чисел - $#"
echo "Сумма чисел - $count"
echo "Среднее арифметическое - $(( $count / $# ))"
