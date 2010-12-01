#!bin/bash
if [ ! -d src/boost_1_45 ];then
    mkdir src/boost_1_45
    echo "extracting boost files..."
    tar xf boost_1_45_BanBot.tar.gz -C src/boost_1_45/
fi

if [ ! -d src/boost_1_45/BanbotLibs ];then
    cd src/boost_1_45/
    ./bootstrap.sh --with-toolset=gcc --with-libraries=filesystem
    ./bjam -d0 --stagedir=BanbotLibs link=static
else
    echo "boost library already built. Skipping.."
fi