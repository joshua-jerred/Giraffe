#!/bin/bash

set -e

CodeChecker analyze ./build/compile_commands.json --output ./.codechecker --analyzer-config 'clang-tidy:take-config-from-directory=true'
CodeChecker store ./.codechecker --name Giraffe --url http://localhost:8001/giraffe