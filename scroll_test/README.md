scroll_test
===========

Utility to test 2 scrolling images on the screen.

There are currently some memory issues with regard to scrolling. The Dispmanx windowing system
seems to crash and burn if you stress it too much and this program was written to stress it.

It creates 1 image that is 3 times the height of the screen and exactly with width.
It also creates 1 image that is 3 times with width of the screen and exactly the height.

It scrolls the really tall picture vertically and the really wide picture horizontally.

Running this program multiple times may cause the Raspberry Pi to lock up hard.

Start off with smallest resolution possible (640x480) and work up from there.

Press the "Any" key to exit.
