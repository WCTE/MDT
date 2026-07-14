#!/bin/bash


EXE=$MDTROOT/app/application/appIWCDSingleEvent

WCSIM_FILE=/disk01/usr5/kmtsui/hyperk_repo/WCSim/build/install/wcsim_output.root
CONFIG_FILE=$MDTROOT/parameter/MDTParamenter_IWCD.txt
OUT_FILE=out_appIWCDSingleEvent.root
SEED=65457869

args=(
    # input file
    -i $WCSIM_FILE
    # config file
	-p $CONFIG_FILE
	# output file
    -o $OUT_FILE
	# RNG seed 
	-s $SEED
	# use OD
	-od
	# number of events to run, -1 means all
	-n -1
)

# $EXE -i $WCSIM_FILE\
# 	 -p $CONFIG_FILE\
# 	 -o $OUT_FILE\
# 	 -s $SEED\
# 	 -d \
# 	 -n -1 # to run all events

$EXE "${args[@]}"