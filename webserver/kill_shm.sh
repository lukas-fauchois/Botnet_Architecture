#!/bin/bash
for i in {3130..3151};
do
	    echo "ipcrm -M $i"
	        ipcrm -M $i
	done
