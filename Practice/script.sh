
filename='a.out'

rm filename
gcc -fopenmp area.c
# `(expr $i \* 10)`
for((i=1;i<=10;i++))
do
	echo $i
	./a.out $i
done