/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kde-arru <kde-arru@student.42.fr>           +#+  +:+       +#+       */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/19 14:47:00 by kde-arru          #+#    #+#             */
/*   Updated: 2026/11/24 17:34:00 by kde-arru         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	parse_arguments(t_sim *sim, int arc, char **arv)
{
	if (arc != 9)
		return (error_exit("Invalid arguments count"));
	if (!is_valid_number(arv[1])
		|| !is_valid_number(arv[2])
		|| !is_valid_number(arv[3])
		|| !is_valid_number(arv[4])
		|| !is_valid_number(arv[5])
		|| !is_valid_number(arv[6])
		|| !is_valid_number(arv[7]))
		return (error_exit("All arguments must be numbers"));
	sim->number_of_coders = atoi(arv[1]);
	sim->time_to_burnout = atoll(arv[2]);
	sim->time_to_compile = atoll(arv[3]);
	sim->time_to_debug = atoll(arv[4]);
	sim->time_to_refactor = atoll(arv[5]);
	sim->compiles_required = atoi(arv[6]);
	sim->dongle_cooldown = atoll(arv[7]);
	sim->scheduler = arv[8];
	if (strcmp(sim->scheduler, "fifo") != 0
		&& strcmp(sim->scheduler, "adf") != 0)
		return (error_exit("Scheduler must be 'fifo' or 'adf'"));
	if (sim->number_of_coders <= 0)
		return (error_exit("Coders must be 1 or greater"));
	return (0);
}

static int	init_mutexes(t_sim *sim)
{
	if (pthread_mutex_init(&(sim->log_mutex), NULL) != 0)
		return (error_exit("Error initializing log_mutex"));
	if (pthread_mutex_init(&(sim->sim_mutex), NULL) != 0)
		return (error_exit("Error initializing sim_mutex"));
	return (0);
}

static int	init_dongles(t_sim *sim)
{
	int	i;

	sim->dongles = malloc(sizeof(t_dongle) * sim->number_of_coders);
	if (!sim->dongles)
		return (error_exit("Error allocating dongles"));
	i = 0;
	while (i < sim->number_of_coders)
	{
		sim->dongles[i].id = i + 1;
		sim->dongles[i].available_at = 0;
		if (pthread_mutex_init(&(sim->dongles[i].mutex), NULL) != 0)
			return (error_exit("Error initializing dongle mutex"));
		i++;
	}
	return (0);
}

static int	init_coders(t_sim *sim)
{
	int	i;

	sim->coders = malloc(sizeof(t_coder) * sim->number_of_coders);
	if (!sim->coders)
		return (error_exit("Error allocating coders"));
	i = 0;
	while (i < sim->number_of_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].sim = sim;
		sim->coders[i].compiles_done = 0;
		sim->coders[i].is_dead = 0;
		sim->coders[i].last_compile_start = 0;
		sim->coders[i].left_hand = &(sim->dongles[i]);
		sim->coders[i].right_hand = &(sim->dongles[(i + 1)
				% sim->number_of_coders]);
		i++;
	}
	return (0);
}

int	init_simulation(t_sim *sim)
{
	sim->simulation_ended = 0;
	if (init_mutexes(sim) != 0)
		return (1);
	if (init_dongles(sim) != 0)
		return (1);
	if (init_coders(sim) != 0)
		return (1);
	return (0);
}
