cmake_minimum_required(VERSION 3.15)
project(s175565_first_fit C)

set(CMAKE_C_STANDARD 11)

add_executable(s175565_first_fit
        "Assignment 3 Report Requirements.pdf"
        Makefile
        "Mandatory Assignment 3.pdf"
        memorytests.c
        mymem.c
        mymem.h
        readme.txt
        testrunner.c
        testrunner.h)