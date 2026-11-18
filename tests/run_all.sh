#!/bin/zsh
# Ejecuta todos los tests: reglas del port frente al binario original (Unicorn) y tests de geometría.
set -e
cd "${0:A:h}/.."
tests/build_host.sh > /dev/null
tools/.venv/bin/python -I tests/test_rules_diff.py 500
python3 -I tests/test_tree_fall.py extracted_sis
zsh tests/trace_regression.sh
