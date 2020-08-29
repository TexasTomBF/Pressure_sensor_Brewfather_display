# Pressure_sensor_Brewfather_display
A 0-3bar (0-45psi) pressure sensor with display and WIFI, sends pressure data to Brewfather

- This project uses a 0.96" OLED I2C display, a Wemos D1 mini pro uC and a 0-5MPa analog pressure transducer(0.5-45.V).
- It sends the pressure data to BreFather (or any other service that has support for it) every 15 mins
- It displays the pressure values from the transducer on a 0.96" OLED display
- If no WIFI connection is established it displays a 'x' in the display, it will retry every 5 mins.
- Each sensors calibration can be tweaked for accuracy, since every sensor is slightly different (see code for coefficients)

Make sure to input your SSDI and password plus your API key to Brewfather before compiling and uploading to Wemos.

The sensor will still display pressure with no WIFI or BF connectivity.

You could also skip the display and just use it to publish pressure data to BrewFather
