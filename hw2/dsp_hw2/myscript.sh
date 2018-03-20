#!/bin/bash
#PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:~/bin
bash ./00_clean_all.sh
bash ./01_run_HCopy.sh
bash ./02_run_HCompV.sh
bash ./03_training.sh
bash ./04_testing.sh
cat ./result/accuracy
