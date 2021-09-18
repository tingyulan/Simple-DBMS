# Simple Database Management System

***Introduction to Database Systems at National Chiao Tung University***\
Instructor: [Prof. Wen-Chih Peng](https://sites.google.com/site/wcpeng/)

***
<br>

This project is a simple DBMS implemented using C, as the education material of the Introduction to Database Systems in National Chiao Tung University

## Prerequisite

* make >= 4.1
* gcc >= 5.4.0
* g++ >= 5.4.0
* gtest >= 1.8.1
* python >= 3.6.0

## Unit test

`$ make check`

## System test

`$ python3 test/system/system_test.py ./shell [test_case..]`

> if no test_cases is specified, it will run all system tests by default

## Compile

`$ make`

## Quick Start
`$ ./shell`

## Query

### Supported command
| Command | Note |
|---|---|
| insert | | 
| select | |
| primary key | |
| limit argument | |
| projection (field selection) | |
| where | Where \<conditions\>. The conditions support numeric, string and logical comparison |
| aggregation functions | average, count, sum |
| update query | |
| delete query | |
| equal join | |

### Table schema
#### `user` table
| field_name | data_type |
|---|---|
| id | int |
| name | char[255] |
| email | char[255] |
| age | int |

#### `like` table
| field_name | data_type |
|---|---|
| id1 | int |
| id2 | int |


## Storage mode

### Memory only

The memory only will not store any record after the process terminate

#### Usage:
`$ ./shell`

### File mode

The File mode will store the record into the specify \<DB file\>, the
next execute can automatically reload the record by the same
execution command.

#### Usage:
`$ ./shell <DB file>`

## Project Layout
    ├─ include/           Header files
    ├─ src/               Source files
    │  ├─ shell.c         Entry point, which contains main()
    │  └─ Util.c          Some utility functions
    ├─ test/              Unit tests & system tests
    │  ├─ include/        Test header files
    │  ├─ system/         System tests
    │  └─ *.c             Unit tests source file
    └─ Makefile           Makefile for make
