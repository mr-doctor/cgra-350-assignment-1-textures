#!/bin/bash

cd build && cmake ../work && make -j3 && cd .. && ./build/bin/a1
