# Configurable
MAX_CURRENT = 2.0 # A (I_MAX)
MAX_POWER_DISSIPATION = 0.5 # W (PD_MAX)
VOLTAGE = 5.0 # V
GAIN = 20.0 # (2O/50/100/200 depending on device)

R_SENSE = 1.12

# Constants
V_SP = -0.02 # Max swing to Vs power-supply rail (typical, max is -0.03)

r_sense_lt = MAX_POWER_DISSIPATION / (MAX_CURRENT**2)
print("R_sense < {:.2f} Ohm".format(r_sense_lt))

if (R_SENSE >= r_sense_lt):
  print("Invalid R_SENSE")

v_sp_lt = MAX_CURRENT * MAX_POWER_DISSIPATION * GAIN
print(f'{v_sp_lt} < {V_SP}')