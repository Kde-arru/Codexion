/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kde-arru <kde-arru@student.42.fr>           +#+  +:+       +#+       */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/19 14:47:00 by kde-arru          #+#    #+#             */
/*   Updated: 2026/11/24 17:51:00 by kde-arru         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_coder_burned_out(t_sim *sim, int i, long long curr)
{
	long long	elapsed;

	elapsed = curr - sim->coders[i].last_compile_start;
	if (elapsed > sim->time_to_burnout)
		return (1);
	return (0);
}

static int	start_threads(t_sim *sim, pthread_t *monitor_thread)
{
	int	i;

	sim->start_time = get_time_in_ms();
	i = 0;
	while (i < sim->number_of_coders)
	{
		sim->coders[i].last_compile_start = sim->start_time;
		if (pthread_create(&(sim->coders[i].thread), NULL,
				coder_routine, &(sim->coders[i])) != 0)
			return (error_exit("Fail to create the thread from coder"));
		i++;
	}
	if (pthread_create(monitor_thread, NULL, monitor_routine, sim) != 0)
		return (error_exit("Fail to create the monitor thread"));
	return (0);
}

int	main(int arc, char **arv)
{
	t_sim		sim;
	pthread_t	monitor_thread;
	int			i;

	if (parse_arguments(&sim, arc, arv) != 0)
		return (1);
	if (init_simulation(&sim) != 0)
		return (1);
	printf("Parsing complete! Coders: %d, Scheduler: %s\n",
		sim.number_of_coders, sim.scheduler);
	if (start_threads(&sim, &monitor_thread) != 0)
		return (1);
	pthread_join(monitor_thread, NULL);
	i = 0;
	while (i < sim->number_of_coders)
	{
		pthread_join(sim.coders[i].thread, NULL);
		i++;
	}
	clean_simulation(&sim);
	printf("simulation cleaned successfuly. Existing.\n");
	return (0);
}
