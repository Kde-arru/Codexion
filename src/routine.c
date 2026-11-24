/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kde-arru <kde-arru@student.42.fr>           +#+  +:+       +#+       */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/19 14:47:00 by kde-arru          #+#    #+#             */
/*   Updated: 2026/11/24 17:57:00 by kde-arru         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	print_status(t_sim *sim, int coder_id, char *status)
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

int	take_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

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

void	drop_dongles(t_coder *coder)
{
	pthread_mutex_unlock(&(coder->left_hand->mutex));
	pthread_mutex_unlock(&(coder->right_hand->mutex));
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->sim->number_of_coders == 1)
		return (usleep(coder->sim->time_to_burnout * 1000), NULL);
	if (coder->id % 2 == 0)
		usleep(15000);
	while (!check_simulation_ended(coder->sim))
	{
		if (take_dongles(coder) != 0)
			break ;
		pthread_mutex_lock(&(coder->sim->sim_mutex));
		coder->last_compile_start = get_time_in_ms();
		coder->compiles_done++;
		pthread_mutex_unlock(&(coder->sim->sim_mutex));
		print_status(coder->sim, coder->id, "is compiling");
		usleep(coder->sim->time_to_compile * 1000);
		drop_dongles(coder);
		print_status(coder->sim, coder->id, "is resting");
		usleep(10000);
	}
	return (NULL);
}

void	*monitor_routine(void *arg)
{
	t_sim		*sim;
	int			i;
	int			all_done;
	long long	curr;

	sim = (t_sim *)arg;
	while (!check_simulation_ended(sim))
	{
		i = -1;
		all_done = 1;
		while (++i < sim->number_of_coders)
		{
			pthread_mutex_lock(&(sim->sim_mutex));
			curr = get_time_in_ms();
			if (curr - sim->coders[i].last_compile_start > sim->time_to_burnout)
				return (sim->simulation_ended = 1,
					pthread_mutex_unlock(&(sim->sim_mutex)), NULL);
			if (sim->compiles_required != -1
				&& sim->coders[i].compiles_done < sim->compiles_required)
				all_done = 0;
			pthread_mutex_unlock(&(sim->sim_mutex));
		}
		if (sim->compiles_required != -1 && all_done)
			return (sim->simulation_ended = 1, NULL);
		usleep(1000);
	}
	return (NULL);
}
