/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lumenthi <lumenthi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/15 14:37:38 by lumenthi          #+#    #+#             */
/*   Updated: 2022/08/15 14:45:24 by lumenthi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

#include  <stdio.h>
#include  <signal.h>
#include  <stdlib.h>

/* Control-C */
void  inthandler(int sig)
{
	(void)sig;
	printf("\n--- ping statistics ---");
	exit(0);
}
