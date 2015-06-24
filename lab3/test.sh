chmod a+x run.sh

make > results_1.txt
make clean >> results_1.txt
ls >> results_1.txt
make >> results_1.txt

echo "====== run.sh process count = 3 =====" > results_2.txt
./run.sh 11 15 32 0.0007 0.13 4  >> results_2.txt
echo "====== run.sh process count = 7 =====" >> results_2.txt
./run.sh 11 15 32 0.0007 0.13 7  >> results_2.txt
echo "====== run.sh process count = 8 =====" >> results_2.txt
./run.sh 11 15 32 0.0007 0.13 8  >> results_2.txt
echo "====== run.sh process count = 13 =====" >> results_2.txt
./run.sh 11 15 32 0.0007 0.13 11  >> results_2.txt
echo "====== run.sh process count = 24 =====" >> results_2.txt
./run.sh 11 15 32 0.0007 0.13 21 >> results_2.txt
echo "Basic tests done"
