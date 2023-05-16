#!/bin/bash


EXE=$MDTROOT/app/application/appWCTESingleEvent

WCSIM_FILE=/disk01/usr5/kmtsui/WCSim_output/wcsim_photocathode_spline_3.1_1.3_20.0_all.root 
CONFIG_FILE=$MDTROOT/parameter/MDTParamenter_WCTE.txt
OUT_FILE=out_appWCTESingleEvent.root
SEED=65457869

$EXE -i $WCSIM_FILE\
	 -p $CONFIG_FILE\
	 -o $OUT_FILE\
	 -s $SEED\
	 -n -1 # to run all events
