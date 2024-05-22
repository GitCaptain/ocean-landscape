# Ocean landscape generator: Procedural generation of the underwater relief

This is an iterative algorithm for generating underwater landscapes based on a model of underwater natural processes developed during my master's thesis. It is able to generate the main types of oceanic relief, such as: deep-sea basins with guyots, taking into account the process of abrasion, continental margins, taking into account the process of subduction and mid-oceanic ridges due to rifting.

The text of the master's thesis can be found [here](https://github.com/GitCaptain/ocean-landscape/blob/master/data/thesis.pdf)

## Examples

There are two examples of the generated landscape:

![landscape1](https://github.com/GitCaptain/ocean-landscape/tree/master/data/examples/landscape.png)

![landscape2](https://github.com/GitCaptain/ocean-landscape/blob/master/data/examples/noise_relief.png)

## Build and run

How to build:
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release  ../src/
make LandscapeGenerator
```

Usage:
```
./build/LandscapeGenerator --sizex=X --sizey=Y --years=N [ --output=file ] [ --mor-cnt=cnt ] [ --basin-cnt=cnt ] [ --margin-cnt=cnt ]
```

example:
```
./LandscapeGenerator --sizex=500 --sizey=500 --years=300000 --output=out.vox
```

## Preview results

The result of the algorithm is the file in the `.vox` format, explained [here](https://github.com/ephtracy/voxel-model/tree/master)

To preview this file you need to download free voxel editor "Magical Voxel", from the [official website](https://ephtracy.github.io) then open generated file here.
