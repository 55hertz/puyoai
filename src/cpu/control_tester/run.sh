#!/bin/bash
cd "$(dirname "$0")"
exec ./control_tester "$@" 2> control_tester.err
