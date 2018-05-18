g++ -fopenmp A1P1_main.cpp prefixsum.cpp
rm -r Results
rm -r Diff
for((i=1;i<=9;i++))
do
	echo $i
	./a.out < 'Tests/TestFiles/Test'$i'.txt'
	# diff 'Results/result'$i'.txt' '../../ResultsFile/result'$i'.txt' > 'Diff/diff'$i'.txt'
done
