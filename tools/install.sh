#!/bin/bash

set -ex

./waf distclean
./waf configure build install "$@"
./waf clean configure build --debug "$@"
./waf install --no-headers
./waf distclean
