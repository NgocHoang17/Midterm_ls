# Midterm Project – Implement `ls(1)`

## 1. Overview
A simplified version of the UNIX `ls` command written in C.  
Supports listing files, hidden files, detailed info, and recursive directories.

## 2. Features
- `-a`: show hidden files  
- `-l`: long format (permissions, owner, size, date)  
- `-R`: recursive listing  
- Error handling & stable (no segfaults)

## 3. Build
```bash
make

## 4. Run
```bash
./my_ls
./my_ls -a
./my_ls -l
./my_ls -R /etc
./my_ls -al

## 5. Project Structure
midterm_ls/
│
├── include/
│   ├── list_dir.h
│   ├── utils.h
│   └── file_info.h
│
├── src/
│   ├── main.c
│   ├── list_dir.c
│   ├── utils.c
│   └── file_info.c
│
├── Makefile
└── README.md

## 6. Notes
Author:Chu Ngoc Hoang
