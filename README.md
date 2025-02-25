<h3 align="center">
<img src="./assets/smart-key.png" width="100"/></br> 
Smart Key System
</h3>

&nbsp;

<p align="center">
Smart Key system is a project for Embedded Systems course at HCMUT that aims to be a affordable solution to
bring some smart functions back to some old motorcycles. The system consist of 2 subsystems according to a 
key and a lock respectively.
</p>

&nbsp;

## 📝 Specification

### 💻 System requirement

Our project's goal is to mimic the commercial smart key system on modern bycicle thus

1. The smartkey system must have a *Smart Key* and a *Smart Lock* subsystems.

2. The smart key subsystem must operate as a key.

    2.1. The key must function as a wireless key.

        2.1.1. The key must be able to communicate wirelessly with the lock.

            2.1.1.1. The key shall operate at a frequency of 2.4 GHz.

            2.1.1.2. The key shall have a communication range of at least 10 meters.

            2.1.1.3. The key shall support proximity detection.

        2.1.2. The key must be optimize for a long term operation.

            2.1.2.1. The key shall be working at least one years under normal using condition.

            2.1.2.2. The power consumption shall be less than 1 mW in sleep mode and less than 50 mW
               during active transmission.

        2.1.3. The key must have security features.

            2.1.3.1. The key must support rolling code encryption or challange-response authentication
               to prevent replay attacks.

            2.1.3.2. The key should implement frequency hopping to minimize RF interference and improve
               security.

        2.1.4. The key should be durable and environmental resist.

            2.1.4.1. The key shall be rated as IP65 protection standard to ensure resistance again dust
               and water splashes.

            2.1.4.2. The key shall operate with a temperature range of -10°C to 60°C. 

        2.1.5. The key should have lock/unlock and horn burst functions.

    2.2. The key hardware should be afforable and easy to build.

        2.2.1. The key should include 2 buttons for lock/unlock-ing and horn bursting features.

        2.2.2. The key shall be powered by a wide-available, repplaceable 3V battery.

3. The smart lock subsystem must function as a lock.

4. The system should use low-price components and easy to source.

### 🧩 Components

- This project requires below coponents:

| Components | Quantity | Feature |
| ---------- | :------: | ------- |
| `nRF24L01+` RF module | 2 | <ul> <li> Work at 2.4 GHz frequency </li> <li> Communicate with MCU via SPI protocol </li> <li> Auto acknowlegdenment when transmitting </li> <li> Controlable transmitting power </li>  <li> Great range </li> <li> Wide operating voltage </li> <li> Ultra low power down consumption </li> </ul> |
| `ATMEGA328P` MCU | 2 | <ul> <li> Wide availibility </li> <li> Support several communication protocol: UART, SPI, I2C </li> <li> Have a good amount of GPIO pin (2 pins for buttons, 3 pins for indicating LEDs, and 6 pins for comminucation with RF module) </li> <li> Have lots of documentation and a large community </li> <li> Simple to configure and program </li> <li> Ultra low power consumption </li> <li> Operate under a wide voltage range </li> |
| `CR2450` battery | 1 | <ul> <li> Small size </li> <li> Geat voltage consistency (3V) </li> <li> Discharge voltage of 2V </li> <li> Good capacity of 600 mAh </li> </ul> |
| Push button | 2 | <ul> <li> Alway off push button </li> </ul> |
| Relay | 2 | <ul> <li> Able to handle high power </li> </ul> |
| RGB LED | 2 | <ul> <li> Simple to implement and program </li> <li> Wide available </li> </ul> |

Others electric components like resistors, conductors, capacitors, etc would be added later
at the end of this project.

- 🔑 **Smart Key** requirement list:

| Components | Quantity | Function |
| ------------- | :-------------: | ------------- |
| `nRF24L01+` RF module | 1 | RF transmiting and receiving |
| Push button | 2 | One for locating the vehicle (flashing light, horn) and one for locking the vehicle |
| `CR2450` battery | 1 | Power supplying |
| `ATMEGA328P` MCU | 1 | Controlling|

- 🔒 **Smart Lock** requirement list:

| Components | Quantity | Function |
| ------------- | :-------------: | ------------- |
| `nRF24L01+` RF module | 1 | RF transmitting and receiving |
| Multicolor LEDs | 1 | Indicating device working mode |
| Relay | 2 | One for controlling the vehicle's power and one for vehicle's light |
| `ATMEGA328P` MCU | 1 | Controlling |

### 🔲 Block diagram

<p align="center">
<img src="./homework1/smart_key_system_diagram.png"/>
</p>

&nbsp;

## 💁 Our members

|     | Names | IDs |
| ------------- | -------------- | -------------- |
| 1 | Luong Thanh Vy | 2151280 |
| 2 | Nguyen Cuong Quoc | 2251045 |
| 3 | Pham Tan Quang | 2151248 |

