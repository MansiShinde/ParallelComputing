#!/bin/bash

mpic++ -o bucketsort_p1 bucketsort_p1.cpp
echo "complied successfully"

echo "Case 1"
for ((i=1024000; i<=102400000; i*=10))
do
	mpirun -np 2 ./bucketsort_p1 $i
	mpirun -np 4 ./bucketsort_p1 $i
	mpirun -np 6 ./bucketsort_p1 $i
	mpirun -np 8 ./bucketsort_p1 $i
	mpirun -np 10 ./bucketsort_p1 $i
	mpirun -np 12 ./bucketsort_p1 $i
	mpirun -np 14 ./bucketsort_p1 $i
	mpirun -np 16 ./bucketsort_p1 $i
	mpirun -np 18 ./bucketsort_p1 $i
	mpirun -np 20 ./bucketsort_p1 $i
done


echo "Case 2"
p=2
for ((i=1000000; i<=10000000; i+=1000000))
do
        mpirun -np $p ./bucketsort_p1 $i
	p=$((p+2)) 
done


echo "Case 3"
p=2
for n in {997920,7998480,26999280,63997920,125002080,215999280,343002240,511998480,729000720,1000001520}
do
        mpirun -np $p ./bucketsort_p1 $n
        p=$((p+2))
done
