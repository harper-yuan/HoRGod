#!/usr/bin/env bash
prog=$1
args=${@:2}
($1 -p 0 --net-config ../net_config.json $args) >/dev/null & \
    ($1 -p 1 --net-config ../net_config.json $args) >/dev/null & \
    ($1 -p 2 --net-config ../net_config.json $args) >/dev/null & \
    ($1 -p 3 --net-config ../net_config.json $args) >/dev/null & \
    ($1 -p 4 --net-config ../net_config.json $args)