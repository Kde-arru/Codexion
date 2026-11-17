#include "codexion.h"

int parse_arguments(t_sim *sim, int arc, char **arv)
{
    if (arc != 9)
        return (error_exit("The total of arguments is not valid!\n \
            Use: ./codexion [coders] [burnout] [compile] [debug] [refactor] \
            [compiles_req] [cooldown] [scheduler]"));

    if (!is_valid_number(arv[1]) || !is_valid_number(arv[2]) ||
        !is_valid_number(arv[3]) || !is_valid_number(arv[4]) ||
        !is_valid_number(arv[5]) || !is_valid_number(arv[6]) ||
        !is_valid_number(arv[7]))
        return (error_exit("All the arguments need to be numbers"));
    
    sim->number_of_coders = atoi(arv[1]);
    sim->time_to_burnout = atoll(arv[2]);
    sim->time_to_compile = atoll(arv[3]);
    sim->time_to_debug = atoll(arv[4]);
    sim->time_to_refactor = atoll(arv[5]);
    sim->compiles_required = atoi(arv[6]);
    sim->dongle_cooldown = atoll(arv[7]);
    sim->scheduler = arv[8];

    if (strcmp(sim->scheduler, "fifo") != 0 &&
        strcmp(sim->scheduler, "adf") != 0)
        return (error_exit("Error: the scheduler needs to be 'fifo' or 'adf'"));
    
    if (sim->number_of_coders <= 0)
        return (error_exit("Error: the numbers of coders must be 1 or greater"));
    
    return (0);
}

int init_simulation(t_sim *sim)
{
    int i;

    sim->simulation_ended = 0;

    if (pthread_mutex_init(&(sim->log_mutex), NULL) != 0)
        return (error_exit("Error: Error at the inicialization of log_mutex"));
    if (pthread_mutex_init(&(sim->sim_mutex), NULL) != 0)
        return (error_exit("Error: Error at the inicialization of sim_mutex"));
    
    sim->dongles = malloc(sizeof(t_dongle) * sim->number_of_coders);
    sim->coders = malloc(sizeof(t_coder) * sim->number_of_coders);
    if (!sim->coders || !sim->dongles)
        return (error_exit("Error on put memory (malloc)."));

    i = 0;
    while (i < sim.number_of_coders)
    {
        sim->dongles[i].id = i + 1;
        sim->dongles[i].available_at = 0;
        if (pthread_mutex_init(&(sim->dongles[i].mutex), NULL) != 0)
            return (error_exit("Error during initialization"));
        i++;
    }

    i = 0;
    while (i < sim->number_of_coders)
    {
        sim->coders[i].id = i + 1;
        sim->coders[i].sim = sim;
        sim->coders[i].compiles_done = 0;
        sim->coders[i].is_dead = 0;
        sim->coders[i].last_compile_start = 0;

        sim->coders[i].left_hand = &(sim->dongles[i]);
        sim->coders[i].right_hand = &(sim->dongles[(i + 1) % sim->number_of_coders]);
        i++;
    }
    return (0); 
}