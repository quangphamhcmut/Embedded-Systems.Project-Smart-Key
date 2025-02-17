# Embedded-Systems.Project-Smart-Key

This is the repository for our project in Embedded System Course at HCMUT and it also include all our homeworks.

## Members {#members}

|     | Names | IDs |
| ------------- | -------------- | -------------- |
| 1 | Luong Thanh Vy | 2151280 |
| 2 | Nguyen Cuong Quoc | 2251045 |
| 3 | Pham Tan Quang | 2151248 |


## Specification {#spec}

### Requirment table

[Requirement table](https://docs.google.com/spreadsheets/d/1nUn6pa3cHQUdWSz3ERi9wC6BmqBU7rnba5peyX5WJQM/edit?usp=sharing) 

### Components

- **Smart Key**

| Components | Quantity |    |
| ------------- | ------------- | ------------- |
| *nRF24L01+* RF module | 1 | <ul> <li> Communicate with MCU via SPI protocol </li> <li> Auto acknowlegdenment when transmitting </li> <li> Controlable transmitting power </li>  <li> Great range </li> <li> Wide operating voltage </li> <li> Ultra low power down consumption </li> </ul> |
| Push button | 2 | <ul> <li> A button for locating the vehicle (flashing light, horn) </li> <li> A button for locking the vehicle </li> </ul> |
| CR2450 battery | 1 | A small battery with <ul> <li> Geat voltage consistency (3V) </li> <li> Discharge voltage of 2V </li> <li> Good capacity of 600 mAh </li> </ul> |
| ATMEGA328P MCU | 1 | A wide available microcontroller that: <ul> <li> Support several communication protocol: UART, SPI, I2C </li> <li> Have a good amount of GPIO pin (2 pins for buttons, 3 pins for indicating LEDs, and 6 pins for comminucation with RF module) </li> <li> Have lots of documentation and a large community </li> <li> Simple to configure and program </li> <li> Ultra low power consumption </li> <li> Operate under a wide voltage range </li> |

## Block diagram

![Block diagram](./homework1/smart_key_system_diagram.png) 
