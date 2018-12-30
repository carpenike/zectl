/*
 * Copyright (c) 2018, John Ramsden.
 * MIT License, see:
 * https://github.com/johnramsden/zectl/blob/master/LICENSE.md
 */

/*
 * Code for commandline application 'zectl'
 */

#include <stdio.h>
#include <string.h>

#include "util.h"
#include "common.h"
#include "zfs.h"
#include "system_linux.h"

/* Function pointer to command */
typedef ze_error_t (*command_func)(int argc, char **argv);

/* Command name -> function map */
typedef struct {
    char *name;
    command_func command;
} command_map_t;

/* Print zectl command usage */
static void ze_usage(void){
    puts("\nUsage:");
    puts("zectl activate <boot environment>");
    puts("zectl create <boot environment>");
    puts("zectl destroy <boot environment>");
    puts("zectl get <property>");
    puts("zectl list");
    puts("zectl mount <boot environment>");
    puts("zectl rename <boot environment> <boot environment>");
    puts("zectl set <property=value> <boot environment>");
    puts("zectl snapshot <boot environment>@<snap>");
    puts("zectl unmount <boot environment>");
}

/* TODO Implement */
ze_error_t
ze_list(int argc, char **argv){
    ze_error_t ret = ZE_ERROR_SUCCESS;

    const char *pool = "zroot"; // TODO: Get pool
    const char *f = "list.lua";

    libze_handle_t *lzeh = libze_init();
    if (lzeh) {
        DEBUG_PRINT("ROOT: %s\n", lzeh->rootfs);
        DEBUG_PRINT("BE ROOT: %s\n", lzeh->be_root);
        DEBUG_PRINT("BOOTFS: %s\n", lzeh->bootfs);
        DEBUG_PRINT("ZPOOL: %s\n", lzeh->zpool);
    }

    zfs_run_channel_program(f, pool);

    return ret;
}

/*
 * Check the command matches with one of the available options.
 * Return a function pointer to the requested command or NULL if no match
 */
static command_func
get_command(command_map_t *ze_command_map,
            int num_command_options, char *input_name){
    command_func command = NULL;

    for (int i = 0; i < num_command_options; i++) {
        if(strcmp(input_name,ze_command_map[i].name) == 0) {
            command = ze_command_map[i].command;
        }
    }
    return command;
}

#define NUM_COMMANDS 1 // Will be 9

int main(int argc, char **argv) {

    int ze_argc = argc-1;
    char *ze_argv[ze_argc];

    /* Set up all commands */
    command_map_t ze_command_map[NUM_COMMANDS] = {
            /* If commands are added or removed, must modify 'NUM_COMMANDS' */
//            {"activate", ze_run_activate},
//            {"create", ze_run_create},
//            {"destroy", ze_run_destroy},
//            {"get", ze_run_get},
            {"list", ze_list},
//            {"mount", ze_run_mount},
//            {"rename", ze_run_rename},
//            {"set", ze_run_set},
//            {"snapshot", ze_run_snapshot},
//            {"unmount", ze_run_unmount}
    };

    fputs("ZE: Boot Environment Manager for ZFS\n\n", stdout);

//    if((boot_environment = ze_init()) == NULL) {
//        fputs("bez: System may not be configured correctly for boot environments\n", stderr);
//        exit(EXIT_FAILURE);
//    }

    /* Check correct number of parameters were input */
    if(argc < 2){
        fprintf(stderr, "\nbez: Invalid input, please enter a command.\n");
        ze_usage();
        exit(EXIT_FAILURE);
    } else {
        /* Shift commandline arguments removing the program name 'bez'. */
        for(int i = 0; i<ze_argc; i++) {
            ze_argv[i] = argv[i+1];
        }
    }

    // Get command requested
    command_func ze_command = get_command(ze_command_map,
                                           NUM_COMMANDS, ze_argv[0]);
    // Run command if valid
    if(ze_command != NULL) {
        if(ze_command(ze_argc, ze_argv) != ZE_ERROR_SUCCESS){
            fprintf(stderr, "bez: Failed to run 'bez %s'.\n", ze_argv[0]);
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "\nbez: Invalid input, no match found.");
        ze_usage();
        exit(EXIT_FAILURE);
    }

    return 0;
}