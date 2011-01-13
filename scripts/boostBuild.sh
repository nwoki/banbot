#!bin/bash
if [ ! -d boost_1_45 ];then
    mkdir boost_1_45
    echo "extracting boost files..."
    tar xf boost_1_45_BanBot.tar.gz -C boost_1_45/
fi

if [ ! -d boost_1_45/BanbotLibs ];then
    cd boost_1_45/
    mkdir BanbotLibs
    echo "Building needed Boost libraries..."
	make all
	echo ""
	echo "Boost libraries compiled."
else
    echo "boost library already built. Skipping.."
fi
