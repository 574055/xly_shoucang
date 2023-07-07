# ESP32_Xtray_J

## Overview
* Compatible with v4 and v5 ESP32 X-Trays.
* Capabilities:
    * Wi-Fi connection
    * SD card persistence (for settings, options, data backup)
    * RFID-weight matching

## File Structure
* Most .hpp files can be found in `include/`.
* Most .cpp files can be found in `src/`.

## Testing
* This project includes a test suite. Testing can be generally divided into **host device testing** and **embedded device testing**.
* Host device testing refers to testing performed on your local machine (e.g. PC, mac).
    * These tests can be run by using
        ```
        pio test -e native
        ```
    * We use a special environment for these tests, hence the `-e native` flag.

* Embedded device testing refers to dynamic testing on the embedded controller.
    * Tests of this category haven't been implemented yet.

