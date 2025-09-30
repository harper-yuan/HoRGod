# SemiHoRGod

This directory contains the implementation of the SemiHoRGod protocol. Please refer to the implementation at [quadsquad](https://github.com/cris-iisc/quadsquad) for comparison.

 **At the same time, we extend our sincere gratitude to Aditya Hegde et al. for providing the code framework in [quadsquad](https://github.com/cris-iisc/quadsquad), which has greatly facilitated our implementation of the paper's code. We are truly thankful.**

## External Dependencies
The following libraries need to be installed separately and should be available to the build system and compiler.

- C++17 and [CMake](https://cmake.org/)
- [GMP](https://gmplib.org/)
- [NTL](https://www.shoup.net/ntl/) (11.0.0 or later)
- [Boost](https://www.boost.org/) (1.72.0 or later)
- [Nlohmann JSON](https://github.com/nlohmann/json)
- [EMP Tool](https://github.com/emp-toolkit/emp-tool)
- [EMP OT](https://github.com/emp-toolkit/emp-ot/)

### Docker
All required dependencies to compile and run the project are available through the docker image.
To build and run the docker image, execute the following commands from the root directory of the repository:

```sh
# Build the SemiHoRGod Docker image.
#
# Building the Docker image requires at least 4GB RAM. This needs to be set 
# explicitly in case of Windows and MacOS.
docker build -t horgod_env .

# or you can download the the compressed Docker image by our url
gunzip HoRGod_env.tar.gz # decompress
docker load -i HoRGod_env.tar # load the images
docker tag quadsquad_harper:latest horgod_env:latest

# Create and run a container.
#
# This should start the shell from within the container.
docker run -it -v $PWD:/code --name horgod_container1 horgod_env 
docker run -it \
    --name horgod_container1 \
    --network host \
    -v $PWD:/code \
    horgod_env

# The following command changes the working directory to the one containing the 
# source code and should be run on the shell started using the previous command.
cd /code

# If you exit the container, you can enter the container using the following command:
docker start -ai horgod_container1
docker exec -it horgod_container1 /bin/bash
```

## Compilation
The project uses [CMake](https://cmake.org/) for building the source code. 
To compile, run the following commands from the root directory of the repository:

```sh
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release .. 
# cmake -DCMAKE_BUILD_TYPE=Debug .. #debug mode
make -j
```

## Usage
A short description of the compiled programs is given below.
All of them provide detailed usage description on using the `--help` option.

- `benchmarks/online_mpc`: Benchmark the performance of the SemiHoRGod online phase by evaluating a circuit with a given depth and number of multiplication gates at each depth.
- `benchmarks/online_nn`: Benchmark the performance of the SemiHoRGod online phase for neural network inference on the FCN and LeNet models.
- `benchmarks/online_perm`: Benchmark the performance of the SemiHoRGod online phase for oblivious permutation.
- `benchmarks/offline_mpc_tp`: Benchmark the performance of the SemiHoRGod offline phase for a circuit with given number of multiplication gates.
- `benchmarks/offline_mpc_sub`: Benchmark the performance of the subprotocols used in the SemiHoRGod offline phase.
- `benchmarks/offline_nn`: Benchmark the performance of the SemiHoRGod offline phase for neural network inference on the FCN and LeNet models.
- `benchmarks/offline_perm`: Benchmark the performance of the SemiHoRGod offline phase for oblivious permutation.
- `tests/*`: These programs contain unit tests for various parts of the codebase. The test coverage is currently incomplete. However, the protocols have been manually verified for correctness.



### Test and how to run locally

Execute the following commands from the `build` directory created during compilation to run the programs:
```bash
# Run unit tests. Can be skipped.
ctest

# The `run.sh` script in the repository root can be used to run the programs 
# for all parties from the same terminal.
# For example, the previous benchmark can be run using the script as shown
# below.
../run.sh ./benchmarks/online_mpc -g 100 -d 10 -t 1

# All other benchmark programs have similar options and behaviour. The '-h'
# option can be used for detailed usage information.

###############################################neural network inference###############################################
# Benchmark online phase for neural network inference.
../run.sh ./benchmarks/online_nn -n fcn
../run.sh ./benchmarks/online_nn -n lenet

# Benchmark offline phase for neural network inference.
../run.sh ./benchmarks/offline_nn -n fcn
../run.sh ./benchmarks/offline_nn -n lenet

###############################################   MPC multiplication   ###############################################
# Benchmark online phase for MPC multiplication.
../run.sh ./benchmarks/offline_mpc_sub -g 1000 -d 1000 -t 1 #-g number of multiplication || -d depth of multiplication

# Benchmark offline phase for MPC multiplication.
../run.sh ./benchmarks/online_mpc_sub -g 1000 -d 1000 -t 1 #-g number of multiplication || -d depth of multiplication


###############################################     MPC permutation    ###############################################
# Benchmark online phase for MPC oblivious permutation.
../run.sh ./benchmarks/online_perm -g 10000

# Benchmark offline phase for MPC oblivious permutation.
../run.sh ./benchmarks/offline_perm -g 10000
```
### How to run in different machines

```bash
# Benchmark online phase for MPC.
#
# The command below should be run on four different terminals with $PID set to
# 0, 1, 2, and 3 i.e., one instance corresponding to each party.
#
# The number of threads can be set using the '-t' option. '-g' denotes the 
# number of gates at each level and '-d' denotes the depth of the circuit.
#
# The program can be run on different machines by replacing the `--localhost`
# option with '--net-config <net_config.json>' where 'net_config.json' is a
# JSON file containing the IPs of the parties. A template is given in the
# repository root.
./benchmarks/online_mpc -p $PID --localhost -g 1000 -d 1000 -t 1 #command example


./benchmarks/online_mpc -p 10 --net-config ../net_config.json -g 1000 -d 1000 -t 1
./benchmarks/online_mpc -p 11 --net-config ../net_config.json -g 1000 -d 1000 -t 1
./benchmarks/online_mpc -p 12 --net-config ../net_config.json -g 1000 -d 1000 -t 1
./benchmarks/online_mpc -p 13 --net-config ../net_config.json -g 1000 -d 1000 -t 1
./benchmarks/online_mpc -p 14 --net-config ../net_config.json -g 1000 -d 1000 -t 1
```

./benchmarks/online_mpc --net-config ../net_config.json -g 1000 -d 1000 -t 1 -p 10
./benchmarks/online_perm  --localhost -g 100 -d 10 -t 1 -p 0

## How to Debug

Make sure compiling the code using command `cmake -DCMAKE_BUILD_TYPE=Debug ..` at **debug mode**.

```sh
# observe the thread tree
pstree -pul PID

# run the code and using gdb to debug
./benchmarks/online_mpc -p 0 --localhost -g 100 -d 10 >/dev/null &
./benchmarks/online_mpc -p 1 --localhost -g 100 -d 10 >/dev/null &
./benchmarks/online_mpc -p 2 --localhost -g 100 -d 10 >/dev/null &
./benchmarks/online_mpc -p 3 --localhost -g 100 -d 10 >/dev/null &
./benchmarks/online_mpc -p 4 --localhost -g 100 -d 10
gdb --args ./benchmarks/online_mpc -p 4 --localhost -g 100 -d 10  #using gdb with parameters

# If you run the test code, the following command used to fix the thread for debugging
(gdb) set scheduler-locking on/step/off
```


## All test command for both 5PC and 7PC
We note that every machine only runs one command

### Multiplication online phase

#### Running time and communication
```sh
# multiplication online

## depth 1 and 100w multiplication per level
./benchmarks/online_mpc --net-config ../net_config.json -g 1000000 -d 1  -t 1 -r 10 -o online_mpc_100w_d1_t1_r10 -p 0
./benchmarks/online_mpc --net-config ../net_config.json -g 1000000 -d 1  -t 1 -r 10 -o online_mpc_100w_d1_t1_r10 -p 1
./benchmarks/online_mpc --net-config ../net_config.json -g 1000000 -d 1  -t 1 -r 10 -o online_mpc_100w_d1_t1_r10 -p 2
./benchmarks/online_mpc --net-config ../net_config.json -g 1000000 -d 1  -t 1 -r 10 -o online_mpc_100w_d1_t1_r10 -p 3
./benchmarks/online_mpc --net-config ../net_config.json -g 1000000 -d 1  -t 1 -r 10 -o online_mpc_100w_d1_t1_r10 -p 4
./benchmarks/online_mpc --net-config ../net_config.json -g 1000000 -d 1  -t 1 -r 10 -o online_mpc_100w_d1_t1_r10 -p 5
./benchmarks/online_mpc --net-config ../net_config.json -g 1000000 -d 1  -t 1 -r 10 -o online_mpc_100w_d1_t1_r10 -p 6

## depth 20 and 5w multiplication per level
./benchmarks/online_mpc --net-config ../net_config.json -g 50000 -d 20  -t 1 -r 10 -o online_mpc_5w_d20_t1_r10 -p 0
./benchmarks/online_mpc --net-config ../net_config.json -g 50000 -d 20  -t 1 -r 10 -o online_mpc_5w_d20_t1_r10 -p 1
./benchmarks/online_mpc --net-config ../net_config.json -g 50000 -d 20  -t 1 -r 10 -o online_mpc_5w_d20_t1_r10 -p 2
./benchmarks/online_mpc --net-config ../net_config.json -g 50000 -d 20  -t 1 -r 10 -o online_mpc_5w_d20_t1_r10 -p 3
./benchmarks/online_mpc --net-config ../net_config.json -g 50000 -d 20  -t 1 -r 10 -o online_mpc_5w_d20_t1_r10 -p 4
./benchmarks/online_mpc --net-config ../net_config.json -g 50000 -d 20  -t 1 -r 10 -o online_mpc_5w_d20_t1_r10 -p 5
./benchmarks/online_mpc --net-config ../net_config.json -g 50000 -d 20  -t 1 -r 10 -o online_mpc_5w_d20_t1_r10 -p 6

## depth 100 and 1w multiplication per level
./benchmarks/online_mpc --net-config ../net_config.json -g 10000 -d 100  -t 1 -r 10 -o online_mpc_1w_d100_t1_r10 -p 0
./benchmarks/online_mpc --net-config ../net_config.json -g 10000 -d 100  -t 1 -r 10 -o online_mpc_1w_d100_t1_r10 -p 1
./benchmarks/online_mpc --net-config ../net_config.json -g 10000 -d 100  -t 1 -r 10 -o online_mpc_1w_d100_t1_r10 -p 2
./benchmarks/online_mpc --net-config ../net_config.json -g 10000 -d 100  -t 1 -r 10 -o online_mpc_1w_d100_t1_r10 -p 3
./benchmarks/online_mpc --net-config ../net_config.json -g 10000 -d 100  -t 1 -r 10 -o online_mpc_1w_d100_t1_r10 -p 4
./benchmarks/online_mpc --net-config ../net_config.json -g 10000 -d 100  -t 1 -r 10 -o online_mpc_1w_d100_t1_r10 -p 5
./benchmarks/online_mpc --net-config ../net_config.json -g 10000 -d 100  -t 1 -r 10 -o online_mpc_1w_d100_t1_r10 -p 6

## depth 1000 and 1k multiplication per level
./benchmarks/online_mpc --net-config ../net_config.json -g 1000 -d 1000  -t 1 -r 10 -o online_mpc_1k_d1000_t1_r10 -p 0
./benchmarks/online_mpc --net-config ../net_config.json -g 1000 -d 1000  -t 1 -r 10 -o online_mpc_1k_d1000_t1_r10 -p 1
./benchmarks/online_mpc --net-config ../net_config.json -g 1000 -d 1000  -t 1 -r 10 -o online_mpc_1k_d1000_t1_r10 -p 2
./benchmarks/online_mpc --net-config ../net_config.json -g 1000 -d 1000  -t 1 -r 10 -o online_mpc_1k_d1000_t1_r10 -p 3
./benchmarks/online_mpc --net-config ../net_config.json -g 1000 -d 1000  -t 1 -r 10 -o online_mpc_1k_d1000_t1_r10 -p 4
./benchmarks/online_mpc --net-config ../net_config.json -g 1000 -d 1000  -t 1 -r 10 -o online_mpc_1k_d1000_t1_r10 -p 5
./benchmarks/online_mpc --net-config ../net_config.json -g 1000 -d 1000  -t 1 -r 10 -o online_mpc_1k_d1000_t1_r10 -p 6
```

#### Throughput
```sh
# multiplication online

## depth 1 and 100w multiplication per level
./benchmarks/online_mpc --net-config ../net_config.json -g 1000000 -d 1  -t 1 -r 10 -o online_mpc_100w_d1_t1_r10 -p 0
./benchmarks/online_mpc --net-config ../net_config.json -g 1000000 -d 1  -t 1 -r 10 -o online_mpc_100w_d1_t1_r10 -p 1
./benchmarks/online_mpc --net-config ../net_config.json -g 1000000 -d 1  -t 1 -r 10 -o online_mpc_100w_d1_t1_r10 -p 2
./benchmarks/online_mpc --net-config ../net_config.json -g 1000000 -d 1  -t 1 -r 10 -o online_mpc_100w_d1_t1_r10 -p 3
./benchmarks/online_mpc --net-config ../net_config.json -g 1000000 -d 1  -t 1 -r 10 -o online_mpc_100w_d1_t1_r10 -p 4
./benchmarks/online_mpc --net-config ../net_config.json -g 1000000 -d 1  -t 1 -r 10 -o online_mpc_100w_d1_t1_r10 -p 5
./benchmarks/online_mpc --net-config ../net_config.json -g 1000000 -d 1  -t 1 -r 10 -o online_mpc_100w_d1_t1_r10 -p 6

## depth 20 and 5w multiplication per level
./benchmarks/online_mpc --net-config ../net_config.json -g 50000 -d 20  -t 1 -r 10 -o online_mpc_5w_d20_t1_r10 -p 0
./benchmarks/online_mpc --net-config ../net_config.json -g 50000 -d 20  -t 1 -r 10 -o online_mpc_5w_d20_t1_r10 -p 1
./benchmarks/online_mpc --net-config ../net_config.json -g 50000 -d 20  -t 1 -r 10 -o online_mpc_5w_d20_t1_r10 -p 2
./benchmarks/online_mpc --net-config ../net_config.json -g 50000 -d 20  -t 1 -r 10 -o online_mpc_5w_d20_t1_r10 -p 3
./benchmarks/online_mpc --net-config ../net_config.json -g 50000 -d 20  -t 1 -r 10 -o online_mpc_5w_d20_t1_r10 -p 4
./benchmarks/online_mpc --net-config ../net_config.json -g 50000 -d 20  -t 1 -r 10 -o online_mpc_5w_d20_t1_r10 -p 5
./benchmarks/online_mpc --net-config ../net_config.json -g 50000 -d 20  -t 1 -r 10 -o online_mpc_5w_d20_t1_r10 -p 6

## depth 100 and 1w multiplication per level
./benchmarks/online_mpc --net-config ../net_config.json -g 10000 -d 100  -t 1 -r 10 -o online_mpc_1w_d100_t1_r10 -p 0
./benchmarks/online_mpc --net-config ../net_config.json -g 10000 -d 100  -t 1 -r 10 -o online_mpc_1w_d100_t1_r10 -p 1
./benchmarks/online_mpc --net-config ../net_config.json -g 10000 -d 100  -t 1 -r 10 -o online_mpc_1w_d100_t1_r10 -p 2
./benchmarks/online_mpc --net-config ../net_config.json -g 10000 -d 100  -t 1 -r 10 -o online_mpc_1w_d100_t1_r10 -p 3
./benchmarks/online_mpc --net-config ../net_config.json -g 10000 -d 100  -t 1 -r 10 -o online_mpc_1w_d100_t1_r10 -p 4
./benchmarks/online_mpc --net-config ../net_config.json -g 10000 -d 100  -t 1 -r 10 -o online_mpc_1w_d100_t1_r10 -p 5
./benchmarks/online_mpc --net-config ../net_config.json -g 10000 -d 100  -t 1 -r 10 -o online_mpc_1w_d100_t1_r10 -p 6

## depth 1000 and 1k multiplication per level
./benchmarks/online_mpc --net-config ../net_config.json -g 1000 -d 1000  -t 1 -r 10 -o online_mpc_1k_d1000_t1_r10 -p 0
./benchmarks/online_mpc --net-config ../net_config.json -g 1000 -d 1000  -t 1 -r 10 -o online_mpc_1k_d1000_t1_r10 -p 1
./benchmarks/online_mpc --net-config ../net_config.json -g 1000 -d 1000  -t 1 -r 10 -o online_mpc_1k_d1000_t1_r10 -p 2
./benchmarks/online_mpc --net-config ../net_config.json -g 1000 -d 1000  -t 1 -r 10 -o online_mpc_1k_d1000_t1_r10 -p 3
./benchmarks/online_mpc --net-config ../net_config.json -g 1000 -d 1000  -t 1 -r 10 -o online_mpc_1k_d1000_t1_r10 -p 4
./benchmarks/online_mpc --net-config ../net_config.json -g 1000 -d 1000  -t 1 -r 10 -o online_mpc_1k_d1000_t1_r10 -p 5
./benchmarks/online_mpc --net-config ../net_config.json -g 1000 -d 1000  -t 1 -r 10 -o online_mpc_1k_d1000_t1_r10 -p 6
```

### Multiplication offline phase
```sh
# multiplication offline

## depth 1 and 100w multiplication per level
./benchmarks/online_mpc --net-config ../net_config.json -g 1000000 -d 1  -t 1 -r 10 -o online_mpc_100w_d1_t1_r10 -p 0
./benchmarks/online_mpc --net-config ../net_config.json -g 1000000 -d 1  -t 1 -r 10 -o online_mpc_100w_d1_t1_r10 -p 1
./benchmarks/online_mpc --net-config ../net_config.json -g 1000000 -d 1  -t 1 -r 10 -o online_mpc_100w_d1_t1_r10 -p 2
./benchmarks/online_mpc --net-config ../net_config.json -g 1000000 -d 1  -t 1 -r 10 -o online_mpc_100w_d1_t1_r10 -p 3
./benchmarks/online_mpc --net-config ../net_config.json -g 1000000 -d 1  -t 1 -r 10 -o online_mpc_100w_d1_t1_r10 -p 4
./benchmarks/online_mpc --net-config ../net_config.json -g 1000000 -d 1  -t 1 -r 10 -o online_mpc_100w_d1_t1_r10 -p 5
./benchmarks/online_mpc --net-config ../net_config.json -g 1000000 -d 1  -t 1 -r 10 -o online_mpc_100w_d1_t1_r10 -p 6

## depth 20 and 5w multiplication per level
./benchmarks/online_mpc --net-config ../net_config.json -g 50000 -d 20  -t 1 -r 10 -o online_mpc_5w_d20_t1_r10 -p 0
./benchmarks/online_mpc --net-config ../net_config.json -g 50000 -d 20  -t 1 -r 10 -o online_mpc_5w_d20_t1_r10 -p 1
./benchmarks/online_mpc --net-config ../net_config.json -g 50000 -d 20  -t 1 -r 10 -o online_mpc_5w_d20_t1_r10 -p 2
./benchmarks/online_mpc --net-config ../net_config.json -g 50000 -d 20  -t 1 -r 10 -o online_mpc_5w_d20_t1_r10 -p 3
./benchmarks/online_mpc --net-config ../net_config.json -g 50000 -d 20  -t 1 -r 10 -o online_mpc_5w_d20_t1_r10 -p 4
./benchmarks/online_mpc --net-config ../net_config.json -g 50000 -d 20  -t 1 -r 10 -o online_mpc_5w_d20_t1_r10 -p 5
./benchmarks/online_mpc --net-config ../net_config.json -g 50000 -d 20  -t 1 -r 10 -o online_mpc_5w_d20_t1_r10 -p 6

## depth 100 and 1w multiplication per level
./benchmarks/online_mpc --net-config ../net_config.json -g 10000 -d 100  -t 1 -r 10 -o online_mpc_1w_d100_t1_r10 -p 0
./benchmarks/online_mpc --net-config ../net_config.json -g 10000 -d 100  -t 1 -r 10 -o online_mpc_1w_d100_t1_r10 -p 1
./benchmarks/online_mpc --net-config ../net_config.json -g 10000 -d 100  -t 1 -r 10 -o online_mpc_1w_d100_t1_r10 -p 2
./benchmarks/online_mpc --net-config ../net_config.json -g 10000 -d 100  -t 1 -r 10 -o online_mpc_1w_d100_t1_r10 -p 3
./benchmarks/online_mpc --net-config ../net_config.json -g 10000 -d 100  -t 1 -r 10 -o online_mpc_1w_d100_t1_r10 -p 4
./benchmarks/online_mpc --net-config ../net_config.json -g 10000 -d 100  -t 1 -r 10 -o online_mpc_1w_d100_t1_r10 -p 5
./benchmarks/online_mpc --net-config ../net_config.json -g 10000 -d 100  -t 1 -r 10 -o online_mpc_1w_d100_t1_r10 -p 6

## depth 1000 and 1k multiplication per level
./benchmarks/online_mpc --net-config ../net_config.json -g 1000 -d 1000  -t 1 -r 10 -o online_mpc_1k_d1000_t1_r10 -p 0
./benchmarks/online_mpc --net-config ../net_config.json -g 1000 -d 1000  -t 1 -r 10 -o online_mpc_1k_d1000_t1_r10 -p 1
./benchmarks/online_mpc --net-config ../net_config.json -g 1000 -d 1000  -t 1 -r 10 -o online_mpc_1k_d1000_t1_r10 -p 2
./benchmarks/online_mpc --net-config ../net_config.json -g 1000 -d 1000  -t 1 -r 10 -o online_mpc_1k_d1000_t1_r10 -p 3
./benchmarks/online_mpc --net-config ../net_config.json -g 1000 -d 1000  -t 1 -r 10 -o online_mpc_1k_d1000_t1_r10 -p 4
./benchmarks/online_mpc --net-config ../net_config.json -g 1000 -d 1000  -t 1 -r 10 -o online_mpc_1k_d1000_t1_r10 -p 5
./benchmarks/online_mpc --net-config ../net_config.json -g 1000 -d 1000  -t 1 -r 10 -o online_mpc_1k_d1000_t1_r10 -p 6
```