#!/bin/bash
# Script for verifying output from the project.
echo "Script is running, please be patient"
#make clean
#make
echo "Runing for 4 processors..."
echo "MI"
./sim_trace -t traces/4proc_validation/ -p MI &> myoutput.txt
diff myoutput.txt traces/4proc_validation/MI_validation.txt
rm -rf myoutput.txt
####
echo "MSI"
./sim_trace -t traces/4proc_validation/ -p MSI &> myoutput.txt
diff myoutput.txt traces/4proc_validation/MSI_validation.txt
rm -rf myoutput.txt
# ####
echo "MOSI"
./sim_trace -t traces/4proc_validation/ -p MOSI &> myoutput.txt
diff myoutput.txt traces/4proc_validation/MOSI_validation.txt
rm -rf myoutput.txt
# ####
echo "MESI"
./sim_trace -t traces/4proc_validation/ -p MESI &> myoutput.txt
diff myoutput.txt traces/4proc_validation/MESI_validation.txt
rm -rf myoutput.txt
# ####
echo "MOESI"
./sim_trace -t traces/4proc_validation/ -p MOESI &> myoutput.txt
diff myoutput.txt traces/4proc_validation/MOESI_validation.txt
rm -rf myoutput.txt
####
#-----------------------#
echo "Runing for 8 processors..."
echo "MI"
./sim_trace -t traces/8proc_validation/ -p MI &> myoutput.txt
diff myoutput.txt traces/8proc_validation/MI_validation.txt
rm -rf myoutput.txt
####
echo "MSI"
./sim_trace -t traces/8proc_validation/ -p MSI &> myoutput.txt
diff myoutput.txt traces/8proc_validation/MSI_validation.txt
rm -rf myoutput.txt
# ####
echo "MOSI"
./sim_trace -t traces/8proc_validation/ -p MOSI &> myoutput.txt
diff myoutput.txt traces/8proc_validation/MOSI_validation.txt
rm -rf myoutput.txt
# ####
echo "MESI"
./sim_trace -t traces/8proc_validation/ -p MESI &> myoutput.txt
diff myoutput.txt traces/8proc_validation/MESI_validation.txt
rm -rf myoutput.txt
# ####
echo "MOESI"
./sim_trace -t traces/8proc_validation/ -p MOESI &> myoutput.txt
diff myoutput.txt traces/8proc_validation/MOESI_validation.txt
rm -rf myoutput.txt
####
#-----------------------#
echo "Runing for 16 processors..."
echo "MI"
./sim_trace -t traces/16proc_validation/ -p MI &> myoutput.txt
diff myoutput.txt traces/16proc_validation/MI_validation.txt
rm -rf myoutput.txt
####
echo "MSI"
./sim_trace -t traces/16proc_validation/ -p MSI &> myoutput.txt
diff myoutput.txt traces/16proc_validation/MSI_validation.txt
rm -rf myoutput.txt
# ####
echo "MOSI"
./sim_trace -t traces/16proc_validation/ -p MOSI &> myoutput.txt
diff myoutput.txt traces/16proc_validation/MOSI_validation.txt
rm -rf myoutput.txt
# ####
echo "MESI"
./sim_trace -t traces/16proc_validation/ -p MESI &> myoutput.txt
diff myoutput.txt traces/16proc_validation/MESI_validation.txt
rm -rf myoutput.txt
# ####
echo "MOESI"
./sim_trace -t traces/16proc_validation/ -p MOESI &> myoutput.txt
diff myoutput.txt traces/16proc_validation/MOESI_validation.txt
rm -rf myoutput.txt
####
echo "Script is finished. If you don't see any diff output: all correct!"
