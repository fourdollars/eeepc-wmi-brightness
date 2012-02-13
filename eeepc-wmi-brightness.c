/* -*- coding: utf-8; indent-tabs-mode: nil; tab-width: 4; c-basic-offset: 4; -*- */
/**
 * Copyright (C) 2012 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BL_MAX   "/sys/class/backlight/eeepc-wmi/max_brightness"
#define BL_BRN   "/sys/class/backlight/eeepc-wmi/brightness"
#define BL_POWER "/sys/class/backlight/eeepc-wmi/bl_power"

void print_usage(void);
int read_int(const char*);
void write_int(const char*, int);
void brightness_toggle(void);
void brightness_up(void);
void brightness_down(void);

int main(int argc, char* argv[])
{
    if (argc == 1) {
        print_usage();
    }
    else if (strcmp("toggle", argv[1]) == 0) {
        brightness_toggle();
    }
    else if (strcmp("up", argv[1]) == 0) {
        brightness_up();
    }
    else if (strcmp("down", argv[1]) == 0) {
        brightness_down();
    }
    else {
        print_usage();
    }
    exit(EXIT_SUCCESS);
}

void print_usage(void)
{
    printf("Usage: eeepc-wmi-brightness [toggle|up|down]\n");
    return;
}

int read_int(const char* file)
{
    char buf[64] = {0};
    FILE *fp = NULL;

    if (eaccess(file, F_OK|R_OK) != 0) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    fp = fopen(file, "r");

    if (fp == NULL) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    if (fscanf(fp, "%s", buf) <= 0) {
        perror(NULL);
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    fclose(fp);
    return atoi(buf);
}

void write_int(const char* file, int value)
{
    FILE *fp = NULL;

    if (eaccess(file, F_OK|W_OK) != 0) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    fp = fopen(file, "w");

    if (fp == NULL) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    if (fprintf(fp, "%d\n", value) <= 0) {
        perror(NULL);
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    fclose(fp);
    return;
}

void brightness_toggle(void)
{
    int status = read_int(BL_POWER);

    switch (status) {
        case 0:
            write_int(BL_POWER, 4);
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            write_int(BL_POWER, 0);
            break;
        default:
            perror("Should not be here.");
            break;
    }
}

void brightness_up(void)
{
    int max = read_int(BL_MAX);

    if (max == 1) {
        /* Scalar board brightness up */
        write_int(BL_BRN, 1);
    }
    else {
        int curr = read_int(BL_BRN);
        if (curr < max) {
            write_int(BL_BRN, curr + 1);
        }
    }
}

void brightness_down(void)
{
    if (read_int(BL_MAX) == 1) {
        /* Scalar board brightness down */
        write_int(BL_BRN, 0);
    }
    else {
        int curr = read_int(BL_BRN);
        if (curr > 0) {
            write_int(BL_BRN, curr - 1);
        }
    }
}
/* vim:set fileencodings=utf-8 tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
