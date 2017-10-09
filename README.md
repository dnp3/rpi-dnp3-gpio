# rpi-dnp3-gpio

DNP3 outstation implementation mapped to Raspberry PI's GPIO pins

# prerequisites

This project and opendnp3 require cmake and GCC 4.8:

```
> sudo apt-get install g++-4.8 cmake
```

# dependencies

The library also uses the [inih](https://github.com/benhoyt/inih) library for reading configuration files. This is specified as a git submodule, so be sure to clone this repository recursively:

```
git clone --recursive https://github.com/automatak/rpi-dnp3-gpio.git
```

Build and install the [WiringPi](https://projects.drogon.net/raspberry-pi/wiringpi/download-and-install/) library.

Build and install the 2.2.0 release of [opendnp3](https://github.com/automatak/dnp3). Build instructions are [here](https://automatak.com/opendnp3/docs/guide/current/build/cmake/).

After setting up ASIO_HOME, your cmake invocation will look like:
```
> CXX=g++-4.8 cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
> make
> sudo make install
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


