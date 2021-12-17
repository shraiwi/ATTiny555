# ATTiny555
 A 555 timer simulator running on the ATTiny85 :)

For a more detailed description (and build instructions), [check me out on Hackaday!](https://hackaday.io/project/183013-attiny555)

## Overview

Essentially, the ATTiny555 will do these three functions:

1. When the voltage on THRES rises above two-thirds of the input voltage, the OUT pin is pulled low and the DIS pin is set to sink current.
2. When the voltage on TRIG drops below one-third of the input voltage, the OUT pin is pulled high and the DIS pin is set to high impedance (essentially making it float)
3. When RESET is pulled low, OUT is pulled low and the DIS pin is set to high impedance.
