#ifndef CODEXION_H
# define CODEXION_H

#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct s_sim t_sim;

typedef struct t_dongle
{
    int id;
    pthread_mutex_t mutex;
    long long   available_at;

}   t_dongle;

typedef struct s_coder
{
    int id;
    pthread_t   thread;
    t_sim   *sim;

    t_dongle    *left_hand;
    t_dongle    *right_hand;

    long long   last_compile_start;
    int compiles_done;
    int is_dead;
}   t_coder;

typedef struct s_sim
{
    int number_of_coders;
    long long   time_to_burnout;
    long long   time_to_compile;
    long long   time_to_debug;
    long long   time_to_refactor;
    int compiles_required;
    long long   dongle_cooldown;
    char    *scheduler;

    long long start_time;
    int simulation_ended;

    pthread_mutex_t log_mutex;
    pthread_mutex_t sim_mutex;

    t_dongle    *dongles;
    t_coder     *coders;
}   t_sim;

#endif

