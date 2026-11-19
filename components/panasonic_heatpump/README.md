# ESPHome Panasonic Heatpump Component

## Usage

### What you need

* ESPHome compatible microcontroller (e.g. ESP32, ESP32-S2, ESP32-C3, ...)
* ADUM1201 Dual Channel Digital Magnetic Isolator  
  (to convert 5V UART signal from the heatpump to 3.3V UART signal of the ESP controller)
* CN-CNT cable/connectors to Heatpump/CZ-TAW1 (see [Heishamon](https://github.com/heishamon/HeishaMon) github site for more information)
  * For example: [S05B-XASK-1 JST Connector](https://a.aliexpress.com/_EvkmGVo)
  * For example: [XAP-05V-1 5Pin Cable with Female to Female Connector](https://a.aliexpress.com/_ExPT82E)

> [!IMPORTANT]
> The support for ESP8266 and the arduino framework is deprecated for this component.  
> Since v0.0.8 this component uses threads to process the UART communication.  
> But if you still want to use an ESP8266 controller or the arduino framework,  
> please use the branch `heatpump/arduino`.  
>
> ```yaml
> external_components:
>  - source:
>      type: git
>      url: https://github.com/ElVit/esphome_components
>      ref: 'heatpump/arduino'
>    components: [ panasonic_heatpump ]
> ```

### Wiring

![wiring_adum1201.png](../../prototypes/panasonic_heatpump/wiring_adum1201.png)

### CN-CNT Pinout (from top to bottom)

pin | function
----|---------
1   | +5V (250mA)
2   | 0-5V TX (from heatpump)
3   | 0-5V RX (to heatpump)
4   | NC (not connected)
5   | GND

## ESPHome example yaml code

```yaml
substitutions:
  pin_rx_hp: GPIO37  # heatpump reads data (RX) on this pin    (yellow)
  pin_tx_hp: GPIO39  # heatpump sends data (TX) on this pin    (green)
  pin_tx_wm: GPIO18  # WiFi module sends data (TX) on this pin (white)
  pin_rx_wm: GPIO16  # WiFi module reads data (RX) on this pin (blue)

external_components:
  - source:
      type: git
      url: https://github.com/ElVit/esphome_components/
    components: [ panasonic_heatpump ]

uart:
  - id: uart_heatpump
    tx_pin: $pin_rx_hp
    rx_pin: $pin_tx_hp
    baud_rate: 9600
    data_bits: 8
    parity: EVEN
    stop_bits: 1
  - id: uart_cz_taw1
    tx_pin: $pin_rx_wm
    rx_pin: $pin_tx_wm
    baud_rate: 9600
    data_bits: 8
    parity: EVEN
    stop_bits: 1

panasonic_heatpump:
  id: my_heatpump
  uart_id: uart_heatpump
  uart_client_id: uart_cz_taw1
  log_uart_msg: true
  update_interval: 5s
  uart_client_timeout: 10s
  request_send_interval: 1500ms

sensor:
  - platform: panasonic_heatpump
    top1:
      name: "Pump Flow"

binary_sensor:
  - platform: panasonic_heatpump
    top0:
      name: "Heatpump State"

text_sensor:
  - platform: panasonic_heatpump
    top4:
      name: "Operating Mode State"

number:
  - platform: panasonic_heatpump
    set5:
      name: "Set Z1 Heat Request Temperature"
      min_value: -5.0
      max_value: 5.0
      step: 1.0

select:
  - platform: panasonic_heatpump
    cool_mode: true
    set2:
      name: "Set Holiday Mode"

switch:
  - platform: panasonic_heatpump
    set1:
      name: "Set Heatpump"

climate:
  - platform: panasonic_heatpump
    cool_mode: true
    zone1:
      name: "Zone 1"
      min_temperature: -5.0
      max_temperature: 5.0
      temperature_step: 1.0

water_heater:
  - platform: panasonic_heatpump
    tank:
      name: "DHW"
      min_temperature: 40.0
      max_temperature: 65.0
      target_temperature_step: 1.0
```

Thanks to [oxyde42](https://github.com/oxyde42) there is also an exmaple yaml code for the HeishaMon Large Board:

* [example_heishamon_large.yaml](../../prototypes/panasonic_heatpump/example_heishamon_large.yaml)

## Configuration variables

* **id** (*Optional*, ID): Manually specify the ID used for actions.
* **update_interval** (*Optional*, [Time](https://esphome.io/guides/configuration-types/#time)): The interval of the polling request message to get the heatpump values. This time applies only if no `uart_client_id` is set. Defaults to `3s`.
* **uart_id** (Optional, [ID](https://esphome.io/guides/configuration-types/#id)): Manually specify the UART ID of the Heatpump. Required if multiple UART buses are defined.
* **uart_client_id** (*Optional*, [ID](https://esphome.io/guides/configuration-types/#id)): Manually specify the UART ID of an additonal UART client like the Panasonic CZ-TAW1. If this ID is not set then your ESP controller will send the polling request messages.
* **log_uart_msg** (*Optional*, boolean): Shows the raw UART messages in the logs, if set to `true`. The messages will be written to the log level `INFO`. Defaults to false.
* **uart_client_timeout** (*Optional*, [Time](https://esphome.io/guides/configuration-types/#time)): Maximum idle time for the `uart_client_id` connection. If no request is sent from the UART client for this duration, the component will send a POLLING request to the heatpump. This will (hopefully) prevent the heatpump from becoming unresponsive. This feature can also be disabled by setting this option to `100ms` or lower. Defaults to `10s`.
* **request_send_interval** (*Optional*, [Time](https://esphome.io/guides/configuration-types/#time)): Minimum delay between UART requests. This can be increased for devices that need a larger gap between queued commands. Defaults to `250ms`.

### Sensors

All sensors are optional and all options from [sensor component](https://esphome.io/components/sensor/) can be applied.  
Here a list of all supported sensors:

```yaml
sensor:
  - platform: panasonic_heatpump
    top1:
      name: "Pump Flow"
    top5:
      name: "Main Inlet Temp"
    top6:
      name: "Main Outlet Temp"
    top7:
      name: "Main Target Temp"
    top8:
      name: "Compressor Freq"
    top9:
      name: "DHW Target Temp"
    top10:
      name: "DHW Temp"
    top11:
      name: "Operations Hours"
    top12:
      name: "Operations Counter"
    top14:
      name: "Outside Temp"
    top15:
      name: "Heat Power Production"
    top16:
      name: "Heat Power Consumption"
    top21:
      name: "Outside Pipe Temp"
    top22:
      name: "DHW Heat Delta"
    top23:
      name: "Heat Delta"
    top24:
      name: "Cool Delta"
    top25:
      name: "DHW Holiday Shift Temp"
    top27:
      name: "Z1 Heat Request Temp"
    top28:
      name: "Z1 Cool Request Temp"
    top29:
      name: "Z1 Heat Curve Target High Temp"
    top30:
      name: "Z1 Heat Curve Target Low Temp"
    top31:
      name: "Z1 Heat Curve Outside High Temp"
    top32:
      name: "Z1 Heat Curve Outside Low Temp"
    top33:
      name: "Room Thermostat Temp"
    top34:
      name: "Z2 Heat Request Temp"
    top35:
      name: "Z2 Cool Request Temp"
    top36:
      name: "Z1 Water Temp"
    top37:
      name: "Z2 Water Temp"
    top38:
      name: "Cool Power Production"
    top39:
      name: "Cool Power Consumption"
    top40:
      name: "DHW Power Production"
    top41:
      name: "DHW Power Consumption"
    top42:
      name: "Z1 Water Target Temp"
    top43:
      name: "Z2 Water Target Temp"
    top45:
      name: "Room Holiday Shift Temp"
    top46:
      name: "Buffer Temp"
    top47:
      name: "Solar Temp"
    top48:
      name: "Pool Temp"
    top49:
      name: "Main Hex Outlet Temp"
    top50:
      name: "Discharge Temp"
    top51:
      name: "Inside Pipe Temp"
    top52:
      name: "Defrost Temp"
    top53:
      name: "Eva Outlet Temp"
    top54:
      name: "Bypass Outlet Temp"
    top55:
      name: "Ipm Temp"
    top56:
      name: "Z1 Temp"
    top57:
      name: "Z2 Temp"
    top62:
      name: "Fan1 Motor Speed"
    top63:
      name: "Fan2 Motor Speed"
    top64:
      name: "High Pressure"
    top65:
      name: "Pump Speed"
    top66:
      name: "Low Pressure"
    top67:
      name: "Compressor Current"
    top70:
      name: "Sterilization Temp"
    top71:
      name: "Sterilization Max Time"
    top72:
      name: "Z1 Cool Curve Target High Temp"
    top73:
      name: "Z1 Cool Curve Target Low Temp"
    top74:
      name: "Z1 Cool Curve Outside High Temp"
    top75:
      name: "Z1 Cool Curve Outside Low Temp"
    top77:
      name: "Heating Off Outdoor Temp"
    top78:
      name: "Heater On Outdoor Temp"
    top79:
      name: "Heat To Cool Temp"
    top80:
      name: "Cool To Heat Temp"
    top82:
      name: "Z2 Heat Curve Target High Temp"
    top83:
      name: "Z2 Heat Curve Target Low Temp"
    top84:
      name: "Z2 Heat Curve Outside High Temp"
    top85:
      name: "Z2 Heat Curve Outside Low Temp"
    top86:
      name: "Z2 Cool Curve Target High Temp"
    top87:
      name: "Z2 Cool Curve Target Low Temp"
    top88:
      name: "Z2 Cool Curve Outside High Temp"
    top89:
      name: "Z2 Cool Curve Outside Low Temp"
    top90:
      name: "Room Heater Operations Hours"
    top91:
      name: "DHW Heater Operations Hours"
    top93:
      name: "Pump Duty"
    top95:
      name: "Max Pump Duty"
    top96:
      name: "Heater Delay Time"
    top97:
      name: "Heater Start Delta"
    top98:
      name: "Heater Stop Delta"
    top102:
      name: "Solar On Delta"
    top103:
      name: "Solar Off Delta"
    top104:
      name: "Solar Frost Protection"
    top105:
      name: "Solar High Limit"
    top113:
      name: "Buffer Tank Delta"
    top115:
      name: "Water Pressure"
    top116:
      name: "Second Inlet Temp"
    top117:
      name: "Economizer Outlet Temp"
    top118:
      name: "Second Room Thermostat Temp"
    top127:
      name: "Z1 Valve PID"
    top128:
      name: "Z2 Valve PID"
    top131:
      name: "Bivalent Start Temperature"
    top134:
      name: "Bivalent Advanced Start Temp"
    top135:
      name: "Bivalent Advanced Stop Temp"
    top136:
      name: "Bivalent Advanced Start Delay"
    top137:
      name: "Bivalent Advanced Stop Delay"
    top138:
      name: "Bivalent Advanced DHW Delay"
    top142:
      name: "Expansion Valve"
    top146:
      name: "Zone Configuration"
    top147:
      name: "Internal Heater Capacity"
    top152:
      name: "Dry Concrete Target Temp"
    top153:
      name: "Pool Delta"
    top154:
      name: "SG Ready 1 Heat Capacity"
    top155:
      name: "SG Ready 1 DHW Capacity"
    top156:
      name: "SG Ready 2 Heat Capacity"
    top157:
      name: "SG Ready 2 DHW Capacity"
    top158:
      name: "Room Operation Max Time"
    top159:
      name: "DHW Heatup Max Time"
    top169:
      name: "Heat Request Temp With Buffer"
    top171:
      name: "Heating On Outdoor Temp"
    top172:
      name: "Heating On Delay"
    top173:
      name: "DHW Booster Delay"
    top174:
      name: "Efficiency Stage 1 Time"
    top175:
      name: "Efficiency Stage 2 Time"
    top176:
      name: "Efficiency Stage 3 Time"
    top177:
      name: "Sensor Offset Remote Th1"
    top178:
      name: "Sensor Offset Outdoor"
    top184:
      name: "SG Ready 1 DHW Power"
    top185:
      name: "SG Ready 1 Heat Power"
    top186:
      name: "SG Ready 1 Cool Power"
    top187:
      name: "SG Ready 2 DHW Power"
    top188:
      name: "SG Ready 2 Heat Power"
    top189:
      name: "SG Ready 2 Cool Power"
    top190:
      name: "SG Ready Stop Power"
    top191:
      name: "SG Ready 1 Cool Capacity"
    top192:
      name: "SG Ready 2 Cool Capacity"
    top193:
      name: "Sensor Offset Return"
    top195:
      name: "Sensor Offset DHW Tank"
    top196:
      name: "Sensor Offset Flow"
    top197:
      name: "Heat Pump Capacity"

    xtop0:
      name: "Heat Power Consumption Extra"
    xtop1:
      name: "Cool Power Consumption Extra"
    xtop2:
      name: "DHW Power Consumption Extra"
    xtop3:
      name: "Heat Power Production Extra"
    xtop4:
      name: "Cool Power Production Extra"
    xtop5:
      name: "DHW Power Production Extra"
```

### Binary Sensors

All binary sensors are optional and all options from [binary sensor component](https://esphome.io/components/binary_sensor/) can be applied.  
Here a list of all supported binary sensors:

```yaml
binary_sensor:
  - platform: panasonic_heatpump
    uart_client_timed_out:
      name: "UART Client Timed Out"
    top0:
      name: "Heatpump State"
    top2:
      name: "Force DHW State"
    top3:
      name: "Quiet Mode Schedule"
    top13:
      name: "Main Schedule State"
    top26:
      name: "Defrosting State"
    top60:
      name: "Internal Heater State"
    top61:
      name: "External Heater State"
    top68:
      name: "Force Heater State"
    top69:
      name: "Sterilization State"
    top99:
      name: "Buffer Installed"
    top100:
      name: "DHW Installed"
    top108:
      name: "Alt External Sensor"
    top109:
      name: "Anti Freeze Mode"
    top110:
      name: "Optional PCB"
    top119:
      name: "External Control"
    top120:
      name: "External Heat Cool Control"
    top121:
      name: "External Error Signal"
    top122:
      name: "External Compressor Control"
    top123:
      name: "Z2 Pump State"
    top124:
      name: "Z1 Pump State"
    top129:
      name: "Bivalent Control"
    top132:
      name: "Bivalent Advanced Heat"
    top133:
      name: "Bivalent Advanced DHW"
    top144:
      name: "Dry Concrete State"
    top149:
      name: "SG Ready Control"
    top150:
      name: "Demand Control"
    top151:
      name: "DHW Defrost"
    top160:
      name: "Quiet Mode Actual State"
    top161:
      name: "Powerful Mode Actual State"
    top163:
      name: "External Switch State"
    top164:
      name: "Bivalent Boiler Contact State"
    top165:
      name: "Z2 Active"
    top166:
      name: "Z1 Active"
    top167:
      name: "Backup Heater Enable"
    top168:
      name: "Cooling Enable"
    top170:
      name: "Static Pressure"
    top179:
      name: "Anti Stick Mode"
    top181:
      name: "Bivalent External Pump"
    top194:
      name: "Defrost Signal"
```

### Text Sensors

All text sensors are optional and all options from [text sensor component](https://esphome.io/components/text_sensor/) can be applied.  
Here a list of all supported text sensors:

```yaml
text_sensor:
  - platform: panasonic_heatpump
    top4:
      name: "Operating Mode State"
    top17:
      name: "Powerful Mode Time"
    top18:
      name: "Quiet Mode Level"
    top19:
      name: "Holiday Mode State"
    top20:
      name: "ThreeWay Valve State"
    top44:
      name: "Error"
    top58:
      name: "DHW Heater State"
    top59:
      name: "Room Heater State"
    top76:
      name: "Heating Mode"
    top81:
      name: "Cooling Mode"
    top92:
      name: "Heat Pump Model"
    top94:
      name: "Zones State"
    top101:
      name: "Solar Mode"
    top106:
      name: "Pump Flowrate Mode"
    top107:
      name: "Liquid Type"
    top111:
      name: "Z1 Sensor Settings"
    top112:
      name: "Z2 Sensor Settings"
    top114:
      name: "External Pad Heater"
    top125:
      name: "TwoWay Valve State"
    top126:
      name: "ThreeWay Valve State2"
    top130:
      name: "Bivalent Mode"
    top139:
      name: "HeatingControl"
    top140:
      name: "SmartDHW"
    top141:
      name: "Quiet Mode Priority"
    top143:
      name: "DHW Sensor Selection"
    top145:
      name: "DHW Capacity Mode"
    top148:
      name: "DHW Heater Type"
    top162:
      name: "Heat Cool Switch State"
    top180:
      name: "Bivalent Operating Mode"
    top182:
      name: "Extra Pump Use"
    top183:
      name: "Extra Pump Mode"
    top198:
      name: "Heat Pump Type"
```

### Numbers

All numbers are optional and all options from [number component](https://esphome.io/components/number/) can be applied.  
Additionally the options `min_value`, `max_value` and `step` can override the default limits of each set entitiy.  
This is usefull for example for `set5` to `set8` if `direct temperature` is configured instead of `compensation curve` (see `top76` and `top81`).  
In Direct mode the numbers are absolute temperatures, not shifts: use `min_value: 20`, `max_value: 60` for `set5` / `set7` (heat) and `min_value: 5`, `max_value: 20` for `set6` / `set8` (cool). Do the same on the `zone1` / `zone2` climates with `min_temperature` / `max_temperature`.
Here a list of all supported numbers:

```yaml
number:
  - platform: panasonic_heatpump
    set5:
      name: "Set Z1 Heat Request Temperature"
    set6:
      name: "Set Z1 Cool Request Temperature"
    set7:
      name: "Set Z2 Heat Request Temperature"
    set8:
      name: "Set Z2 Cool Request Temperature"
    set11:
      name: "Set DHW Temp"
    set15:
      name: "Set Max Pump Duty"
    set16_01:
      name: "Set Zone1 Heat Target High"
    set16_02:
      name: "Set Zone1 Heat Target Low"
    set16_03:
      name: "Set Zone1 Heat Outside Low"
    set16_04:
      name: "Set Zone1 Heat Outside High"
    set16_05:
      name: "Set Zone2 Heat Target High"
    set16_06:
      name: "Set Zone2 Heat Target Low"
    set16_07:
      name: "Set Zone2 Heat Outside Low"
    set16_08:
      name: "Set Zone2 Heat Outside High"
    set16_09:
      name: "Set Zone1 Cool Target High"
    set16_10:
      name: "Set Zone1 Cool Target Low"
    set16_11:
      name: "Set Zone1 Cool Outside Low"
    set16_12:
      name: "Set Zone1 Cool Outside High"
    set16_13:
      name: "Set Zone2 Cool Target High"
    set16_14:
      name: "Set Zone2 Cool Target Low"
    set16_15:
      name: "Set Zone2 Cool Outside Low"
    set16_16:
      name: "Set Zone2 Cool Outside High"
    set18:
      name: "Set Floor Heat Delta"
    set19:
      name: "Set Floor Cool Delta"
    set20:
      name: "Set DHW Heat Delta"
    set21:
      name: "Set Heater Delay Time"
    set22:
      name: "Set Heater Start Delta"
    set23:
      name: "Set Heater Stop Delta"
    set27:
      name: "Set Buffer Delta"
    set29:
      name: "Set Heating Off Outdoor Temp"
    set36:
      name: "Set Bivalent Start Temp"
    set37:
      name: "Set Bivalent AP Start Temp"
    set38:
      name: "Set Bivalent AP Stop Temp"
    set46:
      name: "Set Heater On Outdoor Temp"
    set47:
      name: "Set Heat To Cool Temp"
    set48:
      name: "Set Cool To Heat Temp"
    set_sterilization_temp:
      name: "Set Sterilization Temp"
    set_sterilization_max_time:
      name: "Set Sterilization Max Time"
    set_room_operation_max_time:
      name: "Set Room Operation Max Time"
    set_dhw_heatup_max_time:
      name: "Set DHW Heatup Max Time"
```

### Switches

All switches are optional and all options from [switch component](https://esphome.io/components/switch/) can be applied.  
Here a list of all supported switches:

```yaml
switch:
  - platform: panasonic_heatpump
    set1:
      name: "Set Heatpump"
    set10:
      name: "Set Force DHW"
    set12:
      name: "Set Force Defrost"
    set13:
      name: "Set Force Sterilization"
    set14:
      name: "Set Pump"
    set24:
      name: "Set Main Schedule"
    set25:
      name: "Set Alt External Sensor"
    set28:
      name: "Set Buffer"
    set30:
      name: "Set External Control"
    set31:
      name: "Set External Error"
    set32:
      name: "Set External Compressor Control"
    set33:
      name: "Set External Heat Cool Control"
    set34:
      name: "Set Bivalent Control"
    error_reset:
      name: "Error Reset (Restart Heatpump)"
    force_heater:
      name: "Force Heater"
    quiet_mode_schedule:
      name: "Quiet Mode Schedule"
```

### Selects

All selects are optional and all options from [select component](https://esphome.io/components/select/) can be applied.  
Additionally the option `cool_mode` can be configured.  
If `cool_mode` is set to `true` the entity `set9` will have the additional select options `COOL`, `COOL+TANK`, `AUTO` and `AUTO+TANK`.  
Here a list of all supported selects:

```yaml
select:
  - platform: panasonic_heatpump
    cool_mode: false
    set2:
      name: "Set Holiday Mode"
    set3:
      name: "Set Quiet Mode"
    set4:
      name: "Set Powerful Mode"
    set9:
      name: "Set Operation Mode"
    set17:
      name: "Set Zones"
    set26:
      name: "Set External PadHeater"
    set35:
      name: "Set Bivalent Mode"
    set39:
      name: "Set Heating Control"
    set40:
      name: "Set Smart DHW"
    set41:
      name: "Set Quiet Mode Priority"
    set42:
      name: "Set Pump Flow Rate Mode"
    set43:
      name: "Set DHW Sensor Selection"
    set44:
      name: "Set DHW Heater State"
    set45:
      name: "Set Room Heater State"
```

### Climates

All climates are optional and all options from [climate component](https://esphome.io/components/climate/) can be applied.  
Additionally the option `cool_mode` can be configured.  
If `cool_mode` is set to `true` the entity `zone1` and `zone2` will have the additional climate modes `COOL` and `AUTO`.  
Additionally the options `min_temperature`, `max_temperature` and `temperature_step` can override the default limits on each climate entitiy.  
This is usefull for example for `zone1` and `zone2` if `direct temperature` is configured instead of `compensation curve` (see `top76` and `top81`).  
Here a list of all supported climates:

```yaml
climate:
  - platform: panasonic_heatpump
    cool_mode: false
    zone1:
      name: "Zone 1"
    zone2:
      name: "Zone 2"
```

### Water Heater

All water_heater are optional and all options from [water_heater component](https://esphome.io/components/water_heater/) can be applied.  
Here a list of all supported water_heater:

```yaml
water_heater:
  - platform: panasonic_heatpump
    tank:
      name: "DHW"
```

## Custom Entities (For Advanced Users)

If you review the [ProtocolByteDecrypt.md](https://github.com/heishamon/HeishaMon/blob/main/ProtocolByteDecrypt.md) file you will find also some TOPs and SETs which are not implemented yet in heishamon.  
They are usually marked as TOP (without a number).  
The nice part of ESPHome is that it is so highly customizeable.  
So if you want some additional TOP or SET entities you can easily create your own.  
Under the hood the received uart message from the heatpump is stored in a vector,  
so you can decode each byte by yourself.  
Here an example how to create a sensor and a text_sensor:

```yaml
panasonic_heatpump:
  id: my_heatpump

sensor:
  - platform: template
    name: "Dry concrete target temperature for actual stage"
    update_interval: 3s
    device_class: temperature
    unit_of_measurement: °C
    lambda: |-
      // get the requried byte
      int byte = my_heatpump->get_response_byte(46);
      // a valid byte range is 0x00-0xFF
      // do not update if the byte is invalid
      if (byte < 0) return {};
      // convert the byte (see HeishaMon/ProtocolByteDecrypt.md)
      // in this case -128
      return byte - 128;

text_sensor:
  - platform: template
    name: "DHW capacity (J-series only)"
    update_interval: 3s
    lambda: |-
      // get the requried byte
      int byte = my_heatpump->get_response_byte(9);
      // a valid byte range is 0x00-0xFF
      // do not update if the byte is invalid
      if (byte < 0) return {};
      // convert the byte (see HeishaMon/ProtocolByteDecrypt.md)
      // in this case 3rd and 4th bit (--> b0011 0000)
      int state = ((byte >> 4) & 0b11) - 1;
      // set text
      if (state == 0) return { "Standard" };
      if (state == 1) return { "DHW" };
      // if state is unkown do not update
      return {};
```

## Model-specific notes (K / L series)

- **Heater deltas are signed and clamped.** On K series the installer menu items *Water temperature for heater ON* and *Water temperature for heater OFF* are negative offsets from the water setpoint (defaults around -4 °C and -2 °C). The heat pump accepts **-10 … 0 K**; values below -10 (e.g. -11 or -14) are applied transiently and then revert to -10. `set22` / `set23` therefore default to -10 … 0 K.
- **Heater delay is clamped at 60 min.** `set21` defaults to 0 … 60 minutes; values above 60 (e.g. 250) revert to 60 on this generation.
- **`set46` accepts -20 °C on K series.** The unit reports -20 °C even though older HeishaMon documentation says the lower bound is -15 °C; the component range is -20 … 20.
- **Direct mode needs different ranges.** In Direct mode, `set5`-`set8` are absolute water temperatures, not shifts: heat 20 … 60 °C, cool 5 … 20 °C. Override `min_value` / `max_value` on those numbers. The `zone1` / `zone2` climates must likewise override `min_temperature` / `max_temperature` (20 … 60 °C for heating). If cooling is disabled, set `cool_mode: false` on the climate and select; with `cool_mode: true` the climate uses a two-point heat/cool target that shares one min/max range.
- **Heater enable has two layers.** `set44` / `set45` (`top58` / `top59`) only allow or block the heater per mode. `top167 Backup Heater Enable` is the custom-menu master switch and is **read-only** over CN-CNT. To turn the backup heater off completely, use the controller's *Installer / custom menu → Back-up heater*.
- **Disabled does not mean never.** The K/L indoor unit still energises the backup heater for defrost, anti-freeze and indoor heat-exchanger protection. Panasonic documents this explicitly ("Backup heater is automatically turned ON when it is disabled … protection control of the indoor unit heat exchanger"). Do not cut the heater supply to prevent it: low water temperature during deice can then raise a malfunction.
- **Model-specific TOPs.** `top115` (water pressure), `top117` (economizer outlet), `top141` (quiet mode priority), `top143` (DHW sensor selection, All-In-One only), `top151` (DHW defrost), `top162` (heat/cool switch), `top167`/`top168` (custom menu) and `top197`/`top198` (rated capacity / Standard vs T-CAP) are K/L series values. `top145` (DHW capacity mode) is J-series only, and `top126` (second inlet) is L-series only; they may read as undefined on other models.
- **Force Heater** (`force_heater`, same as `SET47` / the Heater button on the remote) bypasses the normal outdoor-temperature and mode gating and is the first thing to check when the internal heater runs while everything appears disabled. Its state is reported by `top68 Force Heater State`.

### Write safety

- Read-only TOP entities cannot change the heat pump.
- Every writable entity only sends a command when you change it (or an automation calls it); nothing is written during boot or polling.
- The new controls use the same offsets, scaling, and value ranges as HeishaMon's documented commands (`SET47`, `SET49`, `SET50`, `SET53`, `SET54`, `SET55`).
- The new `force_heater` and `quiet_mode_schedule` switches read the current byte from the last response and preserve the neighbouring fields before writing. `force_heater` is equivalent to pressing the remote's Heater button and intentionally runs the backup heater, so leave it off unless you are testing.
- Heater start/stop delta (`set22`/`set23`) and bivalent advanced start/stop (`set37`/`set38`) only change configuration values; they do not command the heater directly.

## Known Issues

When the ESP controller is connected initially to the heatpump (and the heatpump is powered),  
the heatpump may not respond to any request messages.  
If the CZ-TAW1 is also connected to the ESP controller you will probably see some requests like 0x31 05 10 01 ...  
These are initial request messages.  
To solve this problem you can try one of the follwing:

* enable the switch `error_reset`
* turn off and on the power of the heatpump (switching the heatpump off is not enough)

The heatpump will restart and should respond to the requests.

## Sources

:heart: A big THANKS to [Egyras](https://github.com/Egyras), [IgorYbema](https://github.com/IgorYbema) and the work done on the repository [HeishaMon](https://github.com/heishamon/HeishaMon) for decoding the panasonic uart protocol and providing information to build hardware based on an ESP Chip.  
:heart: Thanks to the whole home assistant community for sharing their knowlege and helping me to create this ESPHome component!  
