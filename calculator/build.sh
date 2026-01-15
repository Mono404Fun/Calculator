#!/usr/bin/env bash

RED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[33m"
CYAN="\033[36m"
RESET="\033[0m"

OUT_DIR="build"
EXE_NAME="calculator"
EXE_PATH="./$OUT_DIR/bin/$EXE_NAME"

run_file() {
  echo -e "${GREEN}==> Running...${RESET}"
  "./$EXE_PATH"
}

build_cmake() {
  clear
  
  echo -e "${CYAN}=> Buliding project...${RESET}"
  mkdir -p "./$OUT_DIR"
  cd "./$OUT_DIR"

  cmake ..
  cmake --build .

  cd ..

  if [[ $? -eq 0 ]]; then
    # echo -e "${GREEN}==> Build succeeded in ${YELLOW}${DURATION} ms${RESET}! Binary: ${CYAN}${EXE_PATH}${RESET}"
    START_TIME=$(date +%s%3N)
    run_file
    END_TIME=$(date +%s%3N)
    DURATION=$((END_TIME - START_TIME))
    
    echo ""
    echo -e "${GREEN}=> Finished in ${YELLOW}${DURATION} ms${RESET}"
  else
    echo -e "${RED}==> Build failed.${RESET}"
    exit 1
  fi
}


build_cmake

echo ""