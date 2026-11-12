#!/usr/bin/env bash
# Compiles both sketches for the Arduino Uno. Run from anywhere:
#   ./scripts/check.sh
# Needs arduino-cli with the AVR core and the ZumoShield library. First time:
#   arduino-cli core update-index
#   arduino-cli core install arduino:avr
#   arduino-cli lib install ZumoShield
set -euo pipefail

if ! command -v arduino-cli >/dev/null 2>&1; then
  echo "arduino-cli not found. Install it: https://arduino.github.io/arduino-cli/latest/installation/"
  echo "Then run the three 'first time' commands at the top of this script."
  exit 1
fi

repo="$(cd "$(dirname "$0")/.." && pwd)"
status=0
for sketch in "$repo"/firmware/*/; do
  name="$(basename "$sketch")"
  echo "=== $name ==="
  if output="$(arduino-cli compile --fqbn arduino:avr:uno --warnings all "$sketch" 2>&1)"; then
    # Show our own warnings (not the Arduino core's) and the size summary.
    echo "$output" | grep -E "$name\.ino:.*(warning|error)" || true
    echo "$output" | grep -E "Sketch uses|Global variables"
  else
    echo "$output" | grep -vE "/hardware/avr/|^ +\^" | tail -30
    echo "FAILED: $name"
    status=1
  fi
done
exit $status
