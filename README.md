# thingy91_adp5360_simple

## Overview
Thingy91 has an ADP5360 PMIC (shame it's not a Nordic nPM1300), but the atv2's sensor module does not init the device, it happens as a board init via SYS_INIT.

This sample shows init and using it via start-up thread, or via SYS_INIT like the atv2/thingy91 board init does.

You change the `DEBUG_USE_SYSINIT` define to true/false depending on how you want it to swing. [(LINK)](https://github.com/droidecahedron/thingy91_adp5360_simple/blob/52a1d335fbc818c2e4deb9e9d2e18d38b7672f8d/src/main.c#L16)

`thingy91_board_init` is broken out from the board init in the SDK which has a `SYS_INIT` that gets called. Here is a handy diagram for `SYS_INIT` that I like to refer to.

![image](https://github.com/droidecahedron/thingy91_adp5360_simple/assets/63935881/9b8076cf-b1c9-422e-8dfe-1ba4d923207c)

`SYS_INIT` a handy thing to read about and potentially use in case you are working on controlling something in both bootloader and application, and run into strange cases of application driver inits wiping any work done in the boot. (For example, having a pin stay high from boot to a specific execution in application). However, in the case of atv2 and peeling functionality out, it can make it harder to reason about what the code is doing. It also complicates error handling a bit as well.

Further reading here: [Zephyr project SYS_INIT doc](https://docs.zephyrproject.org/latest/doxygen/html/group__sys__init.html), [NCS Intermediate](https://academy.nordicsemi.com/courses/nrf-connect-sdk-intermediate/lessons/lesson-1-zephyr-rtos-advanced/topic/boot-up-sequence-execution-context/)

## hardware / documentation
- Thingy91
  
  ![image](https://github.com/droidecahedron/thingy91_adp5360_simple/assets/63935881/22f5c0fe-d8a3-462c-ace9-84259d406d86)

- [Web manpage](https://docs.nordicsemi.com/category/thingy91-category)
- [Downloads (incl. schematic)](https://www.nordicsemi.com/Products/Development-hardware/Nordic-Thingy-91/Download)


## Terminal Shots
```c
#define DEBUG_USE_SYSINIT false
```

![image](https://github.com/droidecahedron/thingy91_adp5360_simple/assets/63935881/fdea31a6-c61c-49b4-9a74-197b88b24a8e)

```c
#define DEBUG_USE_SYSINIT true
```

![image](https://github.com/droidecahedron/thingy91_adp5360_simple/assets/63935881/45a3dd6e-557a-4e21-ac62-e8f70db3eadf)

