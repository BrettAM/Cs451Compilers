#!/bin/bash

./c- -P ./assignmentData/$1.c- | diff assignmentData/$1.Pout -
