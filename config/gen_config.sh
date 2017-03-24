#!/bin/bash
if [ ! $# == 1 ]; then
	echo "Usage: $0 UniversityNumber"
	echo "e.g.: $0 Caltech36"
	exit
fi

INSTANCE=$1
mkdir $INSTANCE
BaseAlgs=("MOD" "IM" "LC" "OSLOM")
Frameworks=("RemoveEdge" "ReduceEdge" "ReduceWeight")
LayersNum=("2" "3" "4" "5")

for BaseAlg in ${BaseAlgs[@]}; do
	echo "--$BaseAlg"
	mkdir $INSTANCE/$BaseAlg
	for Framework in ${Frameworks[@]}; do
		echo "----$Framework"
		mkdir $INSTANCE/$BaseAlg/$Framework
		for Num in ${LayersNum[@]}; do
			FileName="$INSTANCE/$BaseAlg/$Framework/${Num}.config"
			echo $FileName
			cp "$INSTANCE.config" $FileName
			#sed -i "s/Caltech36/$INSTANCE/g" $FileName
			Source="MOD\/++MAX\/2"
			Direct="$BaseAlg\/$Framework\/$Num"
			if [ "$Framework" == "RemoveEdge" ]; then
				FMethod="Remove"
				WGraph="FALSE"
			fi
			if [ "$Framework" == "ReduceEdge" ]; then
				FMethod="Reduce++"
				WGraph="FALSE"
			fi
			if [ "$Framework" == "ReduceWeight" ]; then
				FMethod="Reduce++"
				WGraph="TRUE"
			fi


			if [ "$BaseAlg" == "LC" ]; then
				SingleMethod="LinkCommunity"
			fi
			if [ "$BaseAlg" == "MOD" ]; then
				SingleMethod="Modularity"
			fi
			if [ "$BaseAlg" == "IM" ]; then
				SingleMethod="Infomap"
			fi
			if [ "$BaseAlg" == "OSLOM" ]; then
				SingleMethod="OSLOM"
			fi
			sed -i "s/$Source/$Direct/g;0,/Modularity/s//$SingleMethod/g;\
				0,/Number_Of_Layers\t2/s//Number_Of_Layers\t$Num/g;\
				0,/Reduce++/s//$FMethod/g;\
				s/TRUE/$WGraph/g" $FileName
			#sed -i "0,/Modularity/s//$BaseAlg/g" $FileName
			#sed -i "0,/Number_Of_Layers\t2/s//Numbers_Of_Layers\t$Num/g" $FileName
			#sed -i "0,/Reduce++/s//$FMethod/g" $FileName			   
			#sed -i "s/TRUR/$WGraph/g" $FileName
		done
	done

done

#cp ./Caltech36.config $INSTANCE/MOD/
