#!/bin/bash

catch_errors() {
  printf "\nrun_client.sh failed!\n" >&2
  exit 1
}

trap catch_errors ERR;

# Directory containing this bash script
readonly DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

readonly PREV_DIR=$(pwd)

usage() {
  cat <<EOF
  run_client.sh [OPTIONS] Runs the client application.

  EXAMPLE:
    run_client.sh --host=192.168.178.21 --port=65535 

  OPTIONS:
  -h
    --help                this help text
  
  --host=IPADDR           The IPv4 address to connect to.

  --port=PORT             The port to use (0 - 65535)
EOF
}

host="192.168.178.21"
port=65535

while [ "$1" != "" ]; do
  PARAM=`echo $1 | awk -F= '{print $1}'`
  VALUE=`echo $1 | awk -F= '{print $2}'`
  case $PARAM in
    -h | --help)
      usage
      exit 0
      ;;
    --host)
      host=$VALUE
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

./build/client/client_application $DIR/config.yml --host=$host --port=$port

cd $PREV_DIR

exit 0

