count=1
s_sum=0
p_sum=0
make -C serial
make -C parallel
cd serial || exit
for _ in $(seq $count); do
  s=$(./ImageFilters.out ut.bmp)
  s_sum=$(($s_sum + $s))
done
cd ..
echo "Average time for $count serial executions = $(($s_sum / $count)) ms"
cd parallel || exit
for _ in $(seq $count); do
  p=$(./ImageFilters.out ut.bmp)
  p_sum=$(($p_sum + $p))
done
cd ..
echo "Average time for $count parallel executions = $(($p_sum / $count)) ms"
echo "Speedup"
echo "$s_sum/$p_sum" | bc -l
