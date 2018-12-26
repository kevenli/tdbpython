#!/bin/bash
CURRENT_DIR=$(cd `dirname $0` && pwd -P)
cd $CURRENT_DIR
cp lib/linux/*.so /usr/local/lib -f
ldconfig
python setup.py install