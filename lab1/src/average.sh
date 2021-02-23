  #!/bin/bash
s=0
for param in "$@"
do
s=$(( $s + $param ))
done
echo "Количество чисел - $#"
echo "Сумма числел - $s"
echo "Среднее арифметическое - $(( $s / $# ))"