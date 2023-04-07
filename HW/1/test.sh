#!/bin/bash
set -x
./launcher ./sandbox.so config.txt cat /etc/passwd
./launcher ./sandbox.so config.txt cat /etc/hosts
./launcher ./sandbox.so config.txt cat /etc/ssl/certs/Amazon_Root_CA_1.pem
./launcher ./sandbox.so config.txt wget http://google.com -t 1
./launcher ./sandbox.so config.txt wget https://www.nycu.edu.tw -t 1
./launcher ./sandbox.so config.txt wget http://www.google.com -q -t 1
./launcher ./sandbox.so config.txt python3 -c 'import os;os.system("wget http://www.google.com -q -t 1")'
