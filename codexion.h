#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

typedef struct s_sim t_sim;

typedef struct s_dongle
{
    int             id;
    pthread_mutex_t mutex;
    long long       available_at;
}   t_dongle;

typedef struct s_coder
{
    int             id;
    pthread_t       thread;
    t_sim           *sim;

    t_dongle        *left_hand;
    t_dongle        *right_hand;

    long long       last_compile_start;
    int             compiles_done;
    int             is_dead;
}   t_coder;

typedef struct s_sim
{
    int             number_of_coders;
    long long       time_to_burnout;
    long long       time_to_compile;
    long long       time_to_debug;
    long long       time_to_refactor;
    int             compiles_required;
    long long       dongle_cooldown;
    char            *scheduler;

    long long       start_time;
    int             simulation_ended;

    pthread_mutex_t log_mutex;
    pthread_mutex_t sim_mutex;

    t_dongle        *dongles;
    t_coder         *coders;
}   t_sim;

int         parse_arguments(t_sim *sim, int arc, char **arv);
int         init_simulation(t_sim *sim);
int         error_exit(char *str);
int         is_valid_number(char *str);
long long   get_time_in_ms(void);
int         check_simulation_ended(t_sim *sim);
void        clean_simulation(t_sim *sim);
void        print_status(t_sim *sim, int coder_id, char *status);
int         take_dongles(t_coder *coder);
void        drop_dongles(t_coder *coder);
void        *coder_routine(void *arg);
void        *monitor_routine(void *arg);

#endif