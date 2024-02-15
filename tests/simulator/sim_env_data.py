import os
import sys

# -----------------------------------------------------------------------------
#    PRES   HGHT   TEMP   DWPT   RELH   MIXR   DRCT   SKNT   THTA   THTE   THTV
    # hPa     m      C      C      %    g/kg    deg   knot     K      K      K 
# -----------------------------------------------------------------------------

PATH = os.path.dirname(os.path.realpath(__file__))
INPUT_FILE = PATH + '/sim_env_data.txt'

f = open(INPUT_FILE, 'r')

data = []
for line in f:
    line = line.split()
    if len(line) == 11:
        data.append([float(x) for x in line])

outlines = []
for line in data:
    line[7] = line[7] * 0.514444 # knots to m/s
    # {alt:meters, temp:C, pres:mbar, wind_dir:deg, wind_speed:(knots->m/s), rel_hum:% }
    out = f'{{{line[1]}, {line[2]}, {line[0]}, {line[6]}, {line[7]}, {line[4]}}},'
    print(out)