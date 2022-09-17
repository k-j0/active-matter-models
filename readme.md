
# Active Matter Models

This repository contains parallel C++ implementations of a handful of selected models of active matter considered at low Reynolds number in 2D.

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

## List of models

- Continuous run & tumble (`-model run-and-tumble`)
- Active Brownian motion (`-model active-brownian`)
