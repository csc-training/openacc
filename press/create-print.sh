#!/bin/bash

# output files
OUTPUT=openacc2019-lectures.pdf
# slide of an empty page
EMPTY=empty-page.pdf
# prefix for temporary files 
#   beware: all files starting with this will be removed in the clean-up
TMP=nup-tmpfile

# filenames of lecture slides
# ??  note: 1st slide won't to used for content slides, all others will
names=(
title-openacc.pdf
../docs/01-a-GPU-intro.pdf       
../docs/01-b-parallel-concepts.pdf
../docs/02-OpenACC-intro.pdf  
../docs/03-OpenACC-data.pdf
../docs/04-Profiling-GPUs.pdf
../docs/05-async-mpi-routine.pdf
../docs/06-interoperability.pdf
)


# prepare a A4 empty page
empty=$TMP-$EMPTY
pdfjam $EMPTY --a4paper --outfile $empty

# PRODUCE LECTURE SLIDES
queue=""
for ((i=0; i<${#names[@]}; i++))
do
	tmp=$TMP-$i.pdf
	x=$(echo ${names[$i]} | egrep '^(title-|preface.pdf|epilogue.pdf)')
	if [ "$x" != "" ]
	then
		pdfjam ${names[i]} --a4paper --outfile $tmp
	else
		# pdfjam ${names[i]} 1- --nup 2x4 --a4paper --delta '0.05cm 1.5cm' \
                pdfjam ${names[i]} --nup 2x4 --a4paper --delta '0.05cm 1.5cm' \
			--scale 0.95 --frame true --outfile $tmp

	fi
	queue="${queue} ${tmp}"
	
	# add an empty page if needed to break even
	pagecount=$(pdfinfo ${tmp} | grep Pages | cut -c8- | tr -d ' ')
	if (( (pagecount % 2) == 1 ))
	then
		queue="${queue} $empty"
	fi
done
#pdftk $queue output $OUTPUT

pdfjam --outfile $OUTPUT $queue


# remove temporary files
rm $TMP-*

