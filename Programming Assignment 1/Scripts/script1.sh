g++ -fopenmp A1P1_main.cpp prefixsum.cpp
rm -r Results
rm -r Diff
mkdir Results
mkdir Diff
for((i=1;i<=9;i++))
do
	echo $i
	./a.out < 'Tests/TestFiles/Test'$i'.txt' > 'Results/result'$i'.txt'
	diff 'Results/result'$i'.txt' '../../ResultsFile/result'$i'.txt' > 'Diff/diff'$i'.txt'
done
ls -l Diff