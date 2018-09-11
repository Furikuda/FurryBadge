# FurryBadge

This place holds code to run on a [Micro:bit](https://microbit.org/) dev board which interacts with the [DCFurs badge](https://github.com/oskirby/dc26-fur-scripts) over bluetooth. 

Currently does only two things:

+ Sends the "OwO" emote beacon on ButtonA press
+ Sends the "BooP" emote beacon on Button B press

## Run things

```
mount /dev/sdb /mnt/usb

yt target bbc-microbit-classic-gcc

yt build && cp build/bbc-microbit-classic-gcc/source/microbit-samples-combined.hex /mnt/usb ; sync
```
