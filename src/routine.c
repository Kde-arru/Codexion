#include "codexion.h"

void print_status(t_sim *sim, int coder_id, char *status)
{
    pthread_mutex_lock(&(sim->log_mutex));
    if (!sim->simulation_ended)
    {
        printf("%lld %d %s\n",
        get_time_in_ms() - sim->start_time,
        coder_id,
        status);
    }
    pthread_mutex_unlock(&(sim->log_mutex));
}

int take_dongles(t_coder *coder)
{
    t_dongle *first;
    t_dongle *second;

    if (coder->id % 2 == 0)
    {
        first = coder->right_hand;
        second = coder->left_hand;
    }
    else
    {
        first = coder->left_hand;
        second = coder->right_hand;
    }

    pthread_mutex_lock(&(first->mutex));
    print_status(coder->sim, coder->id, "has taken a dongle");

    pthread_mutex_lock(&(second->mutex));
    print_status(coder->sim, coder->id, "has taken a dongle");
    return (0);
}

void drop_dongles(t_coder *coder)
{
    pthread_mutex_unlock(&(coder->left_hand->mutex));
    pthread_mutex_unlock(&(coder->right_hand->mutex));
}

void *coder_routine(void *arg)
{
    t_coder *coder;

    coder = (t_coder *)arg;
    if (coder->sim->number_of_coders == 1)
    {
        usleep(coder->sim->time_to_burnout * 1000);
        return (NULL);
    }
    if (coder->id % 2 == 0)
        usleep(15000);
    while (!check_simulation_ended(coder->sim))
    {
        if (take_dongles(coder) != 0)
            break;
        pthread_mutex_lock(&(coder->sim->sim_mutex));
        coder->last_compile_start = get_time_in_ms();
        pthread_mutex_unlock(&(coder->sim->sim_mutex));
        print_status(coder->sim, coder->id, "is compiling");
        usleep(coder->sim->time_to_compile * 1000);
        drop_dongles(coder);
        print_status(coder->sim, coder->id, "is resting");
        usleep(10000);
    }
    return (NULL);
}

void *monitor_routine(void *arg)
{
    t_sim       *sim;
    int         i;
    long long   current_time;

    sim = (t_sim *)arg;
    while (1)
    {
        i = -1;
        while (++i < sim->number_of_coders)
        {
            pthread_mutex_lock(&(sim->sim_mutex));
            current_time = get_time_in_ms();
            if ((current_time - sim->coders[i].last_compile_start) > sim->time_to_burnout)
            {
                pthread_mutex_lock(&(sim->log_mutex));
                printf("%lld %d died (burnout)\n", current_time - sim->start_time, sim->coders[i].id);
                pthread_mutex_unlock(&(sim->log_mutex));
                sim->simulation_ended = 1;
                pthread_mutex_unlock(&(sim->sim_mutex));
                return (NULL);
            }
            pthread_mutex_unlock(&(sim->sim_mutex));
        }
        usleep(1000);
    }
    return (NULL);
}