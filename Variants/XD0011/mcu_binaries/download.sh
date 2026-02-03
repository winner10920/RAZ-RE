#! /bin/bash

JLinkExe -device N32G031K8 -if swd -speed 4000 -CommandFile download.cmd.jlink 