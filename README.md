# rpi-dnp3-gpio

DNP3 outstation implementation mapped to Raspberry PI's GPIO pins

# dependencies

Build and install the [WiringPi](https://projects.drogon.net/raspberry-pi/wiringpi/download-and-install/) library.

Build and install [opendnp3](https://github.com/jadamcrain/dnp3). Build instructions are [here](https://automatak.com/opendnp3/docs/guide/current/build/cmake/).

The library also uses the [inih](https://github.com/benhoyt/inih) library for reading configuration files. This is specified as a git submodule, so be sure to clone this repository recursively:

```
git clone --recursive https://github.com/automatak/rpi-dnp3-gpio.git
```

# building

The build uses cmake:

```
> mkdir build
> cd build
> cmake ..
> make
```

#usage 

The program takes a single argument, the path to the INI configuration file:

```
> ./rpi-dnp3-gpio ../default.ini
```


