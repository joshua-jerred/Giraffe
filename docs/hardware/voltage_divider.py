##############################
# Voltage Divider Calculator #
##############################

R1 = 10000
R2 = 10000
MEASURED_VOLTAGE = 3300

VOLTAGE = (MEASURED_VOLTAGE * (R1 + R2)) / R2
print(VOLTAGE)