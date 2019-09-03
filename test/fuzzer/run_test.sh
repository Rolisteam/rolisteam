#!/bin/sh

rm -rf build
mkdir build
cd build
QMAKE_CC=afl-gcc QMAKE_CXX=afl-g++ CC=afl-gcc CXX=afl-g++ qmake ../ && make CC=afl-gcc CXX=afl-g++ LINK=afl-g++
#export LD_LIBRARY_PATH="/usr/local/lib:$LD_LIBRARY_PATH"


#./fuzzTestCase ../testcase_dir/gooddata.txt
# afl-fuzz -m 2G -i ../testcase_dir -o ../findings_dir ./fuzzTestCase @@
afl-fuzz -m 2G -i ../testcase_dir -o ../sync_dir ./fuzzed_formula @@ 
echo "afl-fuzz  -m 2G -i ../testcase_dir -o ../sync_dir -S fuzzer02 ./fuzzTestCase @@ 
afl-fuzz -m 2G -i ../testcase_dir -o ../sync_dir -S fuzzer03 ./fuzzTestCase @@ 
afl-fuzz -m 2G -i ../testcase_dir -o ../sync_dir -S fuzzer04 ./fuzzTestCase @@ 
afl-fuzz -m 2G -i ../testcase_dir -o ../sync_dir -S fuzzer05 ./fuzzTestCase @@ 
afl-fuzz -m 2G -i ../testcase_dir -o ../sync_dir -S fuzzer06 ./fuzzTestCase @@"



# as root

# echo core >/proc/sys/kernel/core_pattern

# echo performance | tee cpu*/cpufreq/scaling_governor

# echo  ondemand | tee cpu*/cpufreq/scaling_governor
