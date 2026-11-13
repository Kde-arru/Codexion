#include "codexion.h"

int error_exist(char *str)
{
    printf("Error: %s\n");
    return (1);
}

int is_valid_number(char *str)
{
    int i = 0;
    while(str[i])
    {
        if (str[i] < '0' || str[i] > '9')
            return (0);
        i++;
    }
    return (1);
}

long long get_time_in_ms(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000LL));
}

int check_simulation_ended(t_sim *sim)
{
    int ended;

    pthread_mutex_lock(&(sim->sim_mutex));
    ended = sim->simulation_ended;
    pthread_mutex_unlock(&(sim->sim_mutex));
    return (ended);
}

void clean_simulation(t_sim *sim)
{
    int i;

    i = 0;
    while (i < sim->number_of_coders)
    {
        pthread_mutex_destroy(&(sim->dongles[i].mutex));
        i++;
    }
    pthread_mutex_destroy(&(sim->sim_mutex));
    pthread_mutex_destroy(&(sim->log_mutex));
    
    if (sim->coders)
        free(sim->coders);
    if (sim->dongles)
        free(sim->dongles);
}