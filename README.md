# ATTiny555
A single-header 555 timer simulator running on the ATTiny85 :)

For a more detailed description (and build instructions), [check me out on Hackaday!](https://hackaday.io/project/183013-attiny555)

## Overview

By default, the ATTiny555 will do these three functions:

1. When the voltage on THRES rises above two-thirds of the input voltage, the OUT pin is pulled high and the DIS pin is set to sink current.
2. When the voltage on TRIG drops below one-third of the input voltage, the OUT pin is pulled low and the DIS pin is set to high impedance (essentially making it float)
3. When RESET is pulled low, OUT is pulled low and the DIS pin is set to high impedance.

## API Usage

It’s a bit weird to call this an API, because it’s just a thin wrapper over low-level bit hacks. But anyways, here are the main functions of the header:

### Initialization

Just call the `AT555_begin()` function once upon startup.

### Reading the internal flip-flop

The macro `AT555_flipFlop` contains the value of the internal flip-flop. It’s nonzero when the discharge pin is active, and zero when the discharge is inactive.

### Using the “Original Layout”

If you’re using the original layout, define `AT555_ORIGINAL` **before** including `ATTiny555.h`, like so:

```c++
#define AT555_ORIGINAL
#include "ATTiny555.h"
```

Otherwise, the module will compile for the flip-chip layout, messing up your pins.

### Adding custom output pin functionality

Sometimes, your application may require you to add custom functionality for the output pin. If this is needed, simply define `AT555_DISABLE_OUT` **before** including `ATTiny555.h`, like so:

```c++
#define AT555_DISABLE_OUT
#include "ATTiny555.h"
```

Now the output pin is completely in your control!

If you need a macro that expands to the Arduino pin number of the output pin, just use `AT555_OUTPIN`!

### Changing the trigger and threshold values

By default, the trigger value is set to one-third of the VCC and the threshold value is set to two-thirds of VCC. However, these values can be changed. Keep in mind that modifying both is only supported using the flip-chip layout.

| Layout    | Is TRIG value mutable? | Is THRESH value mutable?                    |
| --------- | ---------------------- | ------------------------------------------- |
| Original  | :heavy_check_mark:     | :x: (only through external voltage divider) |
| Flip-Chip | :heavy_check_mark:     | :heavy_check_mark:                          |

They are stored as integers, where `255` is VCC and `0` is zero volts. For example, half of VCC would be expressed as `127` (rounding down).

To change the values, simply define them **before** including `ATTiny555.h`, like so:

```c++
#define AT555_TRIGVAL   (??)
#define AT555_THRESHVAL (??)

#include "ATTiny555.h"
```

> Tip: You can `#define` them as variable names if you need to change them programmatically! Just be sure to define them before including `ATTiny555.h`. Just be aware this requires evil global variables.
>
> ```c++
> #define AT555_TRIGVAL   (myTrigVal)
> #define AT555_THRESHVAL (myThreshVal)
> 
> uint8_t myTrigVal;
> uint8_t myThreshVal;
> 
> #include "ATTiny555.h"
> 
> // ta da!
> ```
