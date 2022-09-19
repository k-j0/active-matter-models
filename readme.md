
# Active Matter Models

This repository contains parallel C++ implementations of a handful of selected models of active matter considered at low Reynolds number in 2D and 3D.

## Building & running

Run the following, with a version of GCC compatible with C++17, where `<model-name>` is the name of the model to run.
```sh
$ make
$ ./amm -model <model-name>
```

Other command-line options can be viewed by running
```sh
$ ./amm help
```
or, to view options specific to a given model
```sh
$ ./amm help -model <model-name>
```

## Previewing results

When running the simulation, outputs are written to a simple binary file, [results/out.bin](results/out.bin) by default. Such files can be used to further analyze results in external programs (e.g. Python scripts), or previewed using the JavaScript viewer provided under [index.html](index.html). Simply open [index.html](index.html) in a web browser, and select the binary file to load.

For further information on what the binary file contains, please see the `Model::toBinary` method source code - each file will contain as many such entries as iterations made throughout the simulation.

## List of models

- Random walk (`-model random-walk`) (note: this is a passive model)
- Continuous run & tumble (`-model run-and-tumble`)
- Active Brownian motion (`-model active-brownian`)
- Vicsek model (`-model vicsek`)
- Boids (`-model boids`)
