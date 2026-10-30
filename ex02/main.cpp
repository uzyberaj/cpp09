/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ulyssezyberaj <ulyssezyberaj@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 23:09:54 by ulyssezyber       #+#    #+#             */
/*   Updated: 2026/10/30 15:47:54 by ulyssezyber      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PmergeMe.hpp"

// Usage : ./PmergeMe 3 5 9 7 4
// Beaucoup de nombres :
//   ./PmergeMe `jot -r 3000 1 100000 | tr '\n' ' '`
//	 ./PmergeMe `shuf -i 1-100000 -n 3000 | tr "\n" " "`

int main(int argc, char** argv)
{
	PmergeMe program;

	if (!program.run(argc, argv))
		return 1;
	return 0;
}
