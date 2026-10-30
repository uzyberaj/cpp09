/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ulyssezyberaj <ulyssezyberaj@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 23:09:34 by ulyssezyber       #+#    #+#             */
/*   Updated: 2026/10/28 23:09:35 by ulyssezyber      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PMERGEME_HPP
# define PMERGEME_HPP

# include <deque>
# include <vector>

// Ford-Johnson : paires, tri recursif des grands, puis insertion des petits
// dans l'ordre de Jacobsthal. Le detail est dans PmergeMe.cpp.
//
// Deux conteneurs, ecrits chacun de leur cote :
//   - std::vector : un seul bloc memoire
//   - std::deque  : une suite de blocs
// Inserer un nombre au milieu ne coute pas le meme temps dans les deux.
// C'est pour ca que le sujet demande de chronometrer les deux.
class PmergeMe
{
public:
	PmergeMe();
	PmergeMe(const PmergeMe& other);
	PmergeMe& operator=(const PmergeMe& other);
	~PmergeMe();

	// Lit les arguments, trie, affiche la suite et les deux temps.
	// Renvoie false si un argument est invalide ("Error" est deja affiche).
	bool run(int argc, char** argv) const;

private:
	static void sortVector(std::vector<int>& data);
	static void sortDeque(std::deque<int>& data);
};

#endif
