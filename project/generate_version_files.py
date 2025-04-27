#!/usr/bin/env python3

"""Generate version files for the project.
Usage: python generate_version_files.py js_output_file hpp_output_file
"""

import configparser
import subprocess
import os
import sys
import datetime
import json

print("Generating version files...")

THIRD_PARTY_LIBRARIES = [
    '/lib/third_party/sqlite_cpp/CMakeLists.txt',
    '/lib/third_party/nlohmann_json/CMakeLists.txt'
    ]

# ./project/generate_version_files.py
REPO_ROOT = os.path.abspath(os.path.dirname(os.path.abspath(__file__)) + '/../')
INPUT_VERSION_FILE = REPO_ROOT + '/version'
OUTPUT_VERSION_FILE = REPO_ROOT + '/build/version.ini'
OUTPUT_BUILD_FILE = REPO_ROOT + '/build/software_manifest.json'
OUTPUT_JS_FILE = REPO_ROOT + '/src/common/version.js'

output_versions = configparser.ConfigParser()
output_versions.read(INPUT_VERSION_FILE)

# Get the latest git tag
# output_versions['project']['version'] = str(subprocess.check_output(['git', 'describe', '--tags', '--abbrev=0']).decode('ascii').strip())
# print("Latest git tag: " + output_versions['project']['version'])
output_versions.add_section('project')
output_versions['project']['hash'] = str(subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD']).decode('ascii').strip())
output_versions['project']['branch'] = str(subprocess.check_output(['git', 'rev-parse', '--abbrev-ref', 'HEAD']).decode('ascii').strip())
output_versions['project']['commit_count'] = str(subprocess.check_output(['git', 'rev-list', 'HEAD', '--count']).decode('ascii').strip())
output_versions['project']['repo'] = "https://github.com/joshua-jerred/Giraffe"

repo_clean:bool = subprocess.check_output(['git', 'status', '--porcelain']).decode('ascii').strip() == ""
output_versions['version']['clean'] = str(repo_clean)
# generate the version string per `docs/versioning.md`
VALID_INPUT_STAGES = ['alpha', 'beta', 'rc', 'release']
if output_versions['version'].get('stage') not in VALID_INPUT_STAGES:
    sys.exit("Invalid stage: " + output_versions['version'].get('stage'))

# if not bool(repo_clean):
    # print("append '-dev' to the semantic version due to uncommitted changes")
    # output_versions['version']['stage'] = output_versions['version']['stage'] + '-dev'

major = output_versions['version'].getint('major')
minor = output_versions['version'].getint('minor')
patch = output_versions['version'].getint('patch')
stage = output_versions['version'].get('stage')


version_string = f"{major}.{minor}.{patch}"
if stage != "release":
    version_string += f"-{stage}"
if not repo_clean:
    version_string += "-dev"

# Add it to the output
output_versions['version']['semantic'] = version_string


# date and time of build
date = datetime.datetime.now()
output_versions['project']['date'] = date.strftime("%Y-%m-%d")
output_versions['project']['time'] = date.strftime("%H:%M:%S")



output_versions['submodules'] = {}
GIT_MODULES_FILE = REPO_ROOT + '/.gitmodules'
f = open(GIT_MODULES_FILE, 'r')
for line in f:
    if "path = " in line:
        dir_name = line.strip().split('=')[1].strip()
        submodule_name = dir_name.split('/')[-1]
        submodule_path = REPO_ROOT + '/' + dir_name
        submodule_git_hash = subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD'], cwd=submodule_path).decode('ascii').strip()
        output_versions['submodules'][submodule_name] = submodule_git_hash
f.close()

output_versions['third_party'] = {}
def getCmakeProjectVersion(project_path:str):
    """Attempts to find the project version in the CMakeLists.txt file

    Args:
        project_path (str): path to the project
    """
    with open(project_path) as f:
        while True:
            line = f.readline()
            if not line:
                sys.exit("Could not find project version in " + project_path)
            if "project" in line and "VERSION" in line:
                line = line.split('(')[-1].split(')')[0].split('VERSION')
                if len(line) != 2:
                    sys.exit("Parsing error (cmake project version): " + project_path)
                project_name = line[0].strip()
                project_version = line[1].strip().split(' ')[0]
                output_versions['third_party'][project_name] = project_version
                return

for lib in THIRD_PARTY_LIBRARIES:
    getCmakeProjectVersion(REPO_ROOT + lib)
    
output_versions['languages'] = {}
output_versions['languages']['nodejs'] = subprocess.check_output(['node', '--version']).decode('ascii').strip()

GTEST_CMAKE_FILE = REPO_ROOT + '/lib/third_party/googletest/CMakeLists.txt'
with open(GTEST_CMAKE_FILE) as f:
    for line in f:
        if "GOOGLETEST_VERSION" in line:
            line = line.split('GOOGLETEST_VERSION')[-1].split(')')[0].strip()
            output_versions['third_party']['google_test'] = line

# generate version.js
output_buffer = f"""/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   version.js
 * @brief  Software version information
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @copyright  2023 (license to be defined)
 */

/// @warning This file is autogenerated and should not be edited directly.

module.exports = {{
  version_string: "{output_versions['version'].get('semantic_version')}",
  version_major: {output_versions['version'].get('major')},
  version_minor: {output_versions['version'].get('major')},
  version_patch: {output_versions['version'].get('major')},
  version_stage: "{output_versions['version'].get('stage')}",
  build_date: "{output_versions['project'].get('date')}",
  build_time: "{output_versions['project'].get('time')}",
  git_hash: "{output_versions['project'].get('hash')}",
  git_branch: "{output_versions['project'].get('branch')}",
  git_commit_count: "{output_versions['project'].get('commit_count')}",
  git_repo: "{output_versions['project'].get('repo')}",
  nodejs_version: "{output_versions['languages'].get('nodejs')}"
}};

/// @warning This file is autogenerated and should not be edited directly.
"""

with open(OUTPUT_JS_FILE, 'w') as f:
    print("Writing: " + OUTPUT_JS_FILE)
    f.write(output_buffer)

with open(OUTPUT_VERSION_FILE, 'w') as configfile:
    print("Writing: " + OUTPUT_VERSION_FILE)
    output_versions.write(configfile)

with open(OUTPUT_BUILD_FILE, 'w') as f:
    out_data = {}
    for section in output_versions.sections():
        out_data[section] = dict(output_versions[section])

    # Fix boolean values (str -> bool)
    # This is necessary because configparser does not support boolean values
    # it seems. Big regret going with configparser, should have just used json
    # from the start.
    out_data['version']['clean'] = out_data['version']['clean'] == "True"
    
    FORCE_INT = ['major', 'minor', 'patch']
    for key in FORCE_INT:
        out_data['version'][key] = int(out_data['version'][key])

    print("Writing: " + OUTPUT_BUILD_FILE)
    json.dump(out_data, f, indent=4)

print("Done.")