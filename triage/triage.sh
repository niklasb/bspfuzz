#!/bin/bash
cd "$(dirname "$0")"
./triage.py ../fuzz/out*/fuzzer*/crashes
