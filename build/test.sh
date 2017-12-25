#!/bin/sh

time for r in {0..100}
do
./raytracer $1
done
