# Configurable
MAX_CURRENT = 2.5 # A (I_MAX)
MIN_CURRENT = 0.3 # A (I_MIN)
MIN_VOLTAGE = 6.0 # V
MAX_VOLTAGE = 8.4 # v

R_SENSE = 0.02 # Ohm
GAIN = 50.0 # (2O/50/100/200 depending on device)

max_vdrop = MAX_CURRENT * R_SENSE
min_vdrop = MIN_CURRENT * R_SENSE
min_vdrop_amplified = min_vdrop * GAIN
max_vdrop_amplified = max_vdrop * GAIN

max_power_dissipation = MAX_CURRENT * MAX_CURRENT * R_SENSE

print("Voltage drop [min, max]:", min_vdrop, max_vdrop) # This should be low
print("Voltage drop amplified [min, max]:", min_vdrop_amplified, max_vdrop_amplified) # This should be in the range of the ADC (0-3.3V)
print("Min output voltage:", MIN_VOLTAGE - max_vdrop) # This should be higher than the dropout voltage of the power supply
print(f"Max power dissipation: {max_power_dissipation}W") # This should be low