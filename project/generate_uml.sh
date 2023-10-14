#!/bin/bash

# Run from repo root

set -e

docker-compose run --rm structurizr_cli

java -jar project/plantuml.jar -o "../../assets/uml" -x "./docs/structurizr/uml/*-key.puml" "./docs/structurizr/uml/*.puml" -svg