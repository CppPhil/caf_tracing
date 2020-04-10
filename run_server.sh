#!/bin/bash

catch_errors() {
    printf "\nrun_server.sh failed!\n" >&2
    exit 1
}

trap catch_errors ERR;

# Directory containing this bash script
readonly DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

readonly PREV_DIR=$(pwd)

usage() {
  cat <<EOF
  run_server.sh [OPTIONS] Runs the server application.

  EXAMPLE:
    run_server.sh --port=65535 

  OPTIONS:
  -h
    --help                this help text
  
  --port=PORT             The port to bind to (0 - 65535)
EOF
}

port=65535

while [ "$1" != "" ]; do
  PARAM=`echo $1 | awk -F= '{print $1}'`
  VALUE=`echo $1 | awk -F= '{print $2}'`
  case $PARAM in
    -h | --help)
      usage
      exit 0
      ;;
    --port)
      build_type=$VALUE
      ;;
    *)
      echo "ERROR: unknown parameter \"$PARAM\""
      usage
      exit 1
      ;;
  esac
  shift
done

cd $DIR

./build/server/server_application $DIR/config.yml --port=$port &

cd $PREV_DIR

exit 0

