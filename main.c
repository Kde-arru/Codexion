#include "codexion.h"

int main(int arc, char **arv)
{
    t_sim sim;
    pthread_t monitor_thread;
    int i;

    if (parse_arguments(&sim, arc, arv) != 0)
        return (1);
    if (init_simulation(&sim) != 0)
        return (1);
    printf("Parsing complete! Coders: %d, Scheduler: %s\n",sim.number_of_coders, sim.scheduler);
    sim.start_time = get_time_in_ms();
    i = 0;
    while (i < sim.number_of_coders)
    {
        sim.coders[i].last_compile_start = sim.start_time;

        if (pthread_create(&(sim.coders[i].thread), NULL, coder_routine, &(sim.coders[i])) != 0)
            return (error_exit("Fail to create the thread from coder"));
        i++;
    }
    if (pthread_create(&monitor_thread, NULL, monitor_routine, &sim) != 0)
        return (error_exit("Fail to create the monitor thread"));

    pthread_join(monitor_thread, NULL);
    i = 0;
    while(i < sim.number_of_coders)
    {
        pthread_join(sim.coders[i].thread,  NULL);
        i++;
    }
    clean_simulation(&sim);
    printf("simulation cleaned successfuly. Existing.\n");
    return (0);
}
