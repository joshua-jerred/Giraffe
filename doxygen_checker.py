import subprocess

COMMAND = 'doxygen ./docs/Doxygen/Doxyfile_no_output'

data = subprocess.run(COMMAND, capture_output=True, shell=True)
output = data.stdout.splitlines()
errors = data.stderr.splitlines()

# Do whatever you want with each line of stdout
for line in output:
    line = line.decode('utf-8')
    # print('' + line)

# Do whatever you want with each line of stderr
for line in errors:
    line = line.decode('utf-8')
    print(line)