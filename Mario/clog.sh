#!/usr/bin/env bash

LOG_PATH="build/Debug/swo_$(date +%F_%H-%M-%S).log"

# Receive ITM port 0 over SWO without resetting the running target
STM32_Programmer_CLI -c port=SWD mode=HOTPLUG -swv freq=144 portnumber=0 "${LOG_PATH}" -RA
