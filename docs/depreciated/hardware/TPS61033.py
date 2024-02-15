"""
This is a simple script for calculating the equations
for the TPS61033 boost converter.

https://www.ti.com/lit/ds/symlink/tps61033.pdf?ts=1687042630627
"""

# Known values
V_in_min = 2.2 # Minimum input voltage
# V_in_typ = 2.7 # Typical input voltage
# V_in_max = 4.2 # Maximum input voltage
V_out = 5.0 # Using a fixed 5v output
load_current_max = 2.0 # 2A max load current
efficiency = 0.9 # 90% efficiency, estimated

# possibly known values
L = 0.00047 # inductor value, 470uH
f_sw = 2400000 # switching frequency, 2.4MHz
output_voltage_ripple = 0.01 # 5% output voltage ripple

# eq 3 - D, duty cycle
duty_cycle = 1 - ((V_in_min * efficiency) / V_out)

# eq 4 - Delta I(P-P), peak-to-peak inductor ripple current
i_l_pp = (V_in_min * duty_cycle) / (L * f_sw)

# eq 6, I_L(DC), Inductor DC current
i_l_dc = (V_out * load_current_max) / (V_in_min * efficiency)

# eq 8, I_L(P), peak inductor current
i_l_p = i_l_dc + (i_l_pp / 2)

print("Inductor Peak Current: " + str(i_l_p) + "A")

# eq 9, C_out, output capacitor
c_out = (load_current_max * duty_cycle) / (f_sw * output_voltage_ripple)
# convert to uF
c_out = c_out * 1000000
print("Output Capacitor: " + str(c_out) + "uF")


################################################################################
# Selecting the resistor values
V_REF = 600 # 600mV reference voltage according to datasheet

# for a bit of adjustment...
v_out_max = 5100 # 5.1V output voltage
v_out_min = 4900 # 4.9V output voltage
r2_val = 3000

# eq 5, R1
r1_val_max = (v_out_max/V_REF - 1) * r2_val
r1_val_min = (v_out_min/V_REF - 1) * r2_val

print("R1 Max: " + str(r1_val_max) + " Ohms")
print("R1 Min: " + str(r1_val_min) + " Ohms")
print("Difference: " + str(r1_val_max - r1_val_min) + " Ohms")