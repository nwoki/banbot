#!bin/bash
if [ ! -d src/.obj ]; then
    echo "creating '.obj' dir.."
    cd src/
    mkdir .obj
fi

if [ ! -d boost_1_45/BanbotLibs ];then
    cd boost_1_45/
    mkdir BanbotLibs
fi
