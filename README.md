# Debugging On a Pico

This is a sample project (and maybe a template) for a c++ project that runs on a pico (using pico-sdk) and can be debugged
using gdb from within Vscode.

The development machine is a PC Tower running Ubuntu 22.04.

Here are some references.

Be sure to read https://github.com/raspberrypi/pico-examples/ide/vscode.

## Dependencies

This project requires 

-   the `pico-sdk` from `https://github.com/raspberrypi/pico-sdk`, and 
-   openocd from `github.com/raspberrypi/openocd  branch rp2040-v0.12.0`. 

For convenience they are git submodules and hence this repo should be cloned with the following commands.

### Getting the submodules
```bash
git clone --recurse-submodules https://github.com/robertblackwell/pico-debug-example.git
``` 

or

```bash
git clone https://github.com/robertblackwell/pico-debug-example.git
cd pico-debug-example
git submodule init
git submodule update
```

### Alternative to submodules

If you are going to have more than one pico-sdk project it is probably worth your while installing `pico-sdk` and `openocd`
outside of a specific project.

Regardless of where you put pico-sdk and openocd you must have an environment variable set for each:

-   `PICO_SDK_PATH` set to the root directory of the pico sdk, and
-   `PICO_OPENOCD_PATH` set to the root of your openocd installation so that the path to the openocd binary is given by `${PICO_OPENOCD_PATH}/src/openocd` 

You might like to set these in your `~/.bashrc` file.

### Pico-sdk

Please read the `readme.md` for the pico-sdk to make sure you have cmake and a suitable compiler toolchain installed.

Further check that the file `pico_sdk_import.cmake` is copied from the sdk (`./pico-sdk/external/pico_sdk_import.cmake`)
into the top level of this project.

There is already a copy of this file in the project directory leftover from my own work but at least check its contents
are up-to-date.

### Building openocd

```bash
cd openocd
./bootstrap
./configure
make -j4
```

The instructions on [the raspberrypi documentation site](https://www.raspberrypi.com/documentation/microcontrollers/debug-probe.html#installing-openocd)
suggest also running

```bash
make install
```
But I prefer not to run the install step and use the `openocd` binary from its location in `./openocd/src/openocd`.

You can test that the build has worked by :
```bash
cd ..
./openocd/src/openocd --version
```

You should get something like :

```bash
Open On-Chip Debugger 0.12.0-g4d87f6dca (2023-12-06-07:58)
Licensed under GNU GPL v2
For bug reports, read
	http://openocd.org/doc/doxygen/bugs.html
```

### VSCode - more dependencies
Before worrying about debugging you must have Vscode configured to use Cmake and to be able to build your 
pico application through the Vscode build process. That is F7.

This means the following extensions:

-   c/c++ IntelliSense, debugging ... by Microsoft
-   C/C++ extension pack by Microsoft
-   CMake language support for Vscode
-   CMake Tools - extended support in VScode

You should make sure that the CMake Kit selection is visible in the bottom status bar. In order to debug
you must select the `arm-none-eabi` kit. 

See the 'kit' entry in this projects `settings.json`.



The test of this is that you can build a standalone c++ project targetted at the pico by using the vscode build process.

### VSCode - additional dependencies for debugging

An additional extension:

-   Cortex-Debug, without this the lauch.json files provided in the pico examples will not work.

Here are a few references.

https://www.digikey.com.au/en/maker/projects/raspberry-pi-pico-and-rp2040-cc-part-2-debugging-with-vs-code/470abc7efb07432b82c95f6f67f184c0

https://www.youtube.com/watch?v=5rYwKmTVt4M&t=12s

https://github.com/majbthrd/pico-debug/blob/master/howto/openocd.md


#### Connections

```
Pico Probe USB                      --> dev machine
Pico Probe connector labelled debug --> 3 debug pins on the Target Pico
```
#### Control/Data Flow

On the dev machine run the following entities

<pre>
    IDE --> GDB --> OpenOCD 
</pre>

The openocd processes connects to the PicoProbe via USB

The command for the openocd server is:

```
openocd -s ${PICO_OPENOCD_DIR}/tcl/ -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000"

```

The value `${PICO_OPENOCD_PATH}` should point into the location of your openocd installation, `tcl` is a subdirectory of that installation.



## How to start a debugging session

Connect you pico to a `picoprobe` and connect both to your development machine. [Here is a good image of the connections](https://www.raspberrypi.com/documentation/microcontrollers/debug-probe.html#serial-connections).


Start the `openocd server`. From the root of this project run:

```bash
./run_openocd.sh
```

This command starts a server which should continue to run until to kill it. The output should be somethng like:

```bash
Open On-Chip Debugger 0.12.0-g4d87f6dca (2023-12-01-07:54)
Licensed under GNU GPL v2
For bug reports, read
	http://openocd.org/doc/doxygen/bugs.html
Info : Hardware thread awareness created
Info : Hardware thread awareness created
adapter speed: 5000 kHz

Info : Listening on port 6666 for tcl connections
Info : Listening on port 4444 for telnet connections
Info : Using CMSIS-DAPv2 interface with VID:PID=0x2e8a:0x000c, serial=E6614103E796512F
Info : CMSIS-DAP: SWD supported
Info : CMSIS-DAP: Atomic commands supported
Info : CMSIS-DAP: Test domain timer supported
Info : CMSIS-DAP: FW Version = 2.0.0
Info : CMSIS-DAP: Interface Initialised (SWD)
Info : SWCLK/TCK = 0 SWDIO/TMS = 0 TDI = 0 TDO = 0 nTRST = 0 nRESET = 0
Info : CMSIS-DAP: Interface ready
Info : clock speed 5000 kHz
Info : SWD DPIDR 0x0bc12477, DLPIDR 0x00000001
Info : SWD DPIDR 0x0bc12477, DLPIDR 0x10000001
Info : [rp2040.core0] Cortex-M0+ r0p1 processor detected
Info : [rp2040.core0] target has 4 breakpoints, 2 watchpoints
Info : [rp2040.core1] Cortex-M0+ r0p1 processor detected
Info : [rp2040.core1] target has 4 breakpoints, 2 watchpoints
Info : starting gdb server for rp2040.core0 on 3333
Info : Listening on port 3333 for gdb connections
```

If this step does not work there is probably something wrong with your pico/picoprobe connections.

If this works go to vscode, open up your pico main program and set a breakpoint.

Then hit F5 - this should build the project, down load the code to your pico, and eventually
show the program stopped at your selected breakpoint.

If that did not work, I am sorry and I have failed. My #2 purpose for making this project was to give a clear and precise recipe
for debugging c/c++ coded on a pico.

My #1 purpose was so that I had a record of what I did. Some of this stuff is a bit less than obvious and my memory requires assistance.

## Uarts and USB - output from this project

If you examine the `CMakeLists.txt` file you will observer the two lines 

```cmake
pico_enable_stdio_usb(${TARGET} 1)
pico_enable_stdio_uart(${TARGET} 1)

```

These lines cause `printf` (and all stdio input/output) output to got to both the default uart/serial port and the USB port. 

The USB line that receives this io is the one plugged into the target pi pico board not the one plugged into the `pico debug probe`
device.

The serial io goes via GP0 and GP1 to the `pico debug probe` and then along the usb cable connected to that device to the host.

On my Ubuntu machine these connections appear as `/dev/ttyACM0` and `/dev/ttyACM1`.

If you connect `minicom` or someother terminal program to these devices you will see the output appear on both.

If you change the CMakeLists.txt file to either

```cmake
pico_enable_stdio_usb(${TARGET} 1)
pico_enable_stdio_uart(${TARGET} 0)

```
or 
```cmake
pico_enable_stdio_usb(${TARGET} 0)
pico_enable_stdio_uart(${TARGET} 1)

```

output will appear only on one of the `/dev/ttyACMx` ports.

## References

Good website for rp2040 pwm explanation
https://www.i-programmer.info/programming/hardware/14849-the-pico-in-c-basic-pwm.html?start=2

https://www.i-programmer.info/programming/hardware/14849-the-pico-in-c-basic-pwm.html?start=1

A program for loading code into a pico over a serial line
https://github.com/raspberrypi/picotool/blob/master/main.cpp

Loading programs to a pico using SWD using a Raspberry PI
https://www.electronicshub.org/programming-raspberry-pi-pico-with-swd/

https://www.jeffgeerling.com/blog/2023/testing-raspberry-pis-new-debug-probe

https://blog.smittytone.net/2021/02/05/how-to-debug-a-raspberry-pi-pico-with-a-mac-swd/

https://www.digikey.com.au/en/maker/projects/raspberry-pi-pico-and-rp2040-cc-part-2-debugging-with-vs-code/470abc7efb07432b82c95f6f67f184c0


Good ROS2 project to emulate
https://github.com/linorobot/linorobot2

Starting point for understanding ROS2
https://docs.ros.org/en/foxy/Tutorials.html

Pi Pico OS
https://github.com/garyexplains/piccolo_os_v1/tree/main

Pico std Uart and Usb different output
https://www.youtube.com/watch?v=2EKnANSZQKI

Reference manual for rp2040 Arm Cortex M0 processor
https://developer.arm.com/documentation/ddi0484/c/Programmers-Model

Microros on pi pico
https://ubuntu.com/blog/getting-started-with-micro-ros-on-raspberry-pi-pico

https://github.com/micro-ROS/micro-ROS-Agent

https://github.com/micro-ROS/micro_ros_arduino

https://www.youtube.com/watch?v=aD3Lf-9cb0A

https://github.com/ros-drivers/rosserial

https://github.com/yoneken/rosserial_stm32

build ros subscriber and publisher ros2
https://docs.ros.org/en/iron/Tutorials/Beginner-CLI-Tools/Configuring-ROS2-Environment.html

//xrce-dds use manual
https://usermanual.wiki/m/5285fe9aa500a3e8a817f094ccd4f2366c36faacdbc7fe7bc9af937338b95954.pdf#section.5.3

http://wiki.ros.org/rosserial#Protocol
http://wiki.ros.org/rosserial_python
https://github.com/ros-drivers/rosserial/blob/noetic-devel/rosserial_python/src/rosserial_python/SerialClient.pyros2 rclcpp github

MQTT - ROS
https://github.com/ika-rwth-aachen/mqtt_client/tree/main
https://github.com/robofoundry/ros2_mqtt


MQTT serial

https://www.metacodes.pro/funcodes/using_tty2mqtt_to_bridge_between_serial_communication_and_mqtt/
https://courses.ideate.cmu.edu/16-376/s2021/ref/text/code/mqtt-serial-bridge.html

https://github.com/eclipse/paho.mqtt.embedded-c/tree/master
https://eclipse.dev/paho/index.php?page=clients/c/embedded/index.php
This is a multi-part project
https://www.rs-online.com/designspark/mqtt-part-1-developing-iiot-networks

https://pypi.org/project/paho-mqtt/


Demo ROS2 serial - Josh Newans
This is the repo for the python code on the Raspberry PI that listens to the serial port
and publishes into the ROS graph
https://github.com/joshnewans/serial_motor_demo

MicroRos for raspberry pi pico
https://github.com/micro-ROS/micro_ros_raspberrypi_pico_sdk


micro-ros docker image builds
https://hub.docker.com/r/microros/micro-ros-agent

Lets start thinking about doing all the code n a raspberry pi (NOT PICO)
https://github.com/lbrombach
https://docs.donkeycar.com/parts/odometry/
