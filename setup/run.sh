#!/bin/bash

fs_game="main"
fs_basepath="/home/$USER/cod2"
fs_homepath=$fs_basepath
config="libcod.cfg"
port="28960"

LD_PRELOAD=$fs_basepath/../zk_libcod/code/bin/libcod2.so $fs_basepath/../zk_libcod/cod2_lnxded +set net_ip 0.0.0.0 +set net_port $port +set $fs_game $fs_game +set fs_basepath $fs_basepath +set fs_homepath $fs_homepath +set com_HunkMegs 384 +exec $config +map mp_toujane