rm results_perf1.txt
rm results_perf2.txt
rm results_perf3.txt
rm results_perf4.txt

make

echo "     Test #1"
echo "====== Test #1 =====" >> results_perf1.txt
./run.sh 27 21 211 0.0005 1 8 >> results_perf1.txt
echo "====== Test #1 =====" >> results_perf1.txt
./run.sh 27 21 211 0.0005 1 8 >> results_perf1.txt
echo "====== Test #1 =====" >> results_perf1.txt
./run.sh 27 21 211 0.0005 1 8 >> results_perf1.txt

echo "     Test #2"
echo "====== Test #2 =====" >> results_perf2.txt
./run.sh 218 229 4 0.0005 1 24 >> results_perf2.txt
echo "====== Test #2 =====" >> results_perf2.txt
./run.sh 218 229 4 0.0005 1 24 >> results_perf2.txt
echo "====== Test #2 =====" >> results_perf2.txt
./run.sh 218 229 4 0.0005 1 24 >> results_perf2.txt

echo "     Test #3"
echo "====== Test #3 =====" >> results_perf3.txt
./run.sh 423 14 28 0.0005 1 11 >> results_perf3.txt
echo "====== Test #3 =====" >> results_perf3.txt
./run.sh 423 14 28 0.0005 1 11 >> results_perf3.txt
echo "====== Test #3 =====" >> results_perf3.txt
./run.sh 423 14 28 0.0005 1 11 >> results_perf3.txt

echo "     Test #4"
echo "====== Test #4 =====" >> results_perf4.txt
./run.sh 19 311 39 0.0005 1 20 >> results_perf4.txt
echo "====== Test #4 =====" >> results_perf4.txt
./run.sh 19 311 39 0.0005 1 20 >> results_perf4.txt
echo "====== Test #4 =====" >> results_perf4.txt
./run.sh 19 311 39 0.0005 1 20 >> results_perf4.txt

echo "Performance tests done"
