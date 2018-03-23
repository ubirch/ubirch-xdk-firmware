# ubirch-xdk-firmware
Firmware for Bosch XDK - reads BMA280 accelerometer sensor values and send it to the backend using Ubirch-Protocol

#### Download and install [Bosch XDK Workbench](https://xdk.bosch-connectivity.com/guides)

#### Add [ubirch-protocol](https://github.com/ubirch/ubirch-protocol) library to the Bosch SDK-XDK110

  To include [ubirch-protocol](https://github.com/ubirch/ubirch-protocol#bosch-xdk) library in the Bosch XDK SDK use
  the provided BoschXDK110.mk [Makefile](https://github.com/ubirch/ubirch-protocol/blob/master/BoschXDK110.mk):
  In `ubirch-protocol` directory run:
  ```bash
  make -f BoschXDK110.mk dist
  ```

This creates a distribution directory `BUILD/xdk/ubirch-protocol`, which can be included into the 3rd part libraries
  directory of SDK `SDK/xdk110/Libraries` and edit the config file roughly following [this guide](https://xdk.bosch-connectivity.com/documents/37728/286250/XDK110_Library_Guide.pdf):

  * `SDK/xdk110/Common/application.mk`:
    - add `UBIRCH_LIBRARY_DIR = $(BCDS_LIBRARIES_PATH)/ubirch-protocol`
    - add to `BCDS_XDK_EXT_INCLUDES`
    ```
    -isystem $(UBIRCH_LIBRARY_DIR)/msgpack \
    -isystem $(UBIRCH_LIBRARY_DIR)/nacl \
    -isystem $(UBIRCH_LIBRARY_DIR) \
    ```
  * `SDK/xdk110/Common/Libraries.mk`
    - add to `BCDS_THIRD_PARTY_LIBS`
    ```
    $(UBIRCH_LIBRARY_DIR)/ubirch_protocol.a
    ```

  Now you can include and use the `ubirch_protocol.h`, `msgpack` functionality and of course our `NaCL` port.

  > Just like the TLS library in XDK, this is preliminary as the TRNG of the XDK is not enabled. 
  > [See known issues](http://xdk.bosch-connectivity.com/xdk_docs/html/_known_issues.html).

#### Building and Flashing
  - Put in your WIFI SSID and PASSKEY, Server Host address and port number to send the values to in `ubirch-xdk-firmware.h`
  - Build and flash the firmware using the Bosch XDK Workbench
  - Three LEDs (Yellow, Red and Orange) blink in series
  - XDK connects to the WIFI
  - Gets UTC time from ntp server
  - Every 10 seconds the XDK gets the Accelerometer values from the BMA280 sensors, packs it and signs 
  it using ubrich-protocol and sends it to the backend
