/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ulyssezyberaj <ulyssezyberaj@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 22:05:53 by ulyssezyber       #+#    #+#             */
/*   Updated: 2026/10/28 22:33:09 by ulyssezyber      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RPN.hpp"

# include <iostream>
# include <list>
# include <sstream>
# include <stack>

// --- Forme canonique ---
// La classe ne stocke rien : la pile est creee dans calculate, puis detruite.

RPN::RPN()
{
}

RPN::RPN(const RPN& other)
{
	(void)other;
}

RPN& RPN::operator=(const RPN& other)
{
	(void)other;
	return *this;
}

RPN::~RPN()
{
}

// --- Calcul ---

bool RPN::calculate(const std::string& expression) const
{
	// long : 9 * 9 * 9 ... depasse vite un int.
	// Le calcul et le resultat ne sont pas limites a 9.
	std::stack<long, std::list<long> > stack;

	// stringstream est un flux, il lit dans la chaine au lieu du clavier.
	std::stringstream ss(expression);
	std::string token;

	// >> lit un mot et le range dans token.
	// Il s'arrete sur un espace ou une tabulation, et il saute les espaces en trop.
	// "8  9 *" fait donc trois tours : "8", puis "9", puis "*".
	// Chaque mot est un token.
	// Un token valide tient sur un seul caractere : un chiffre, ou + - * /.
	// Quand il n'y a plus rien a lire, >> rend le flux faux et le while s'arrete.
	while (ss >> token)
	{
		// Les nombres passes sont toujours inferieurs a 10.
		if (token.length() == 1 && token[0] >= '0' && token[0] <= '9')
		{
			stack.push(token[0] - '0');
			continue;
		}

		if (token.length() == 1
			&& (token[0] == '+' || token[0] == '-'
				|| token[0] == '*' || token[0] == '/'))
		{
			if (stack.size() < 2)
			{
				std::cerr << "Error" << std::endl;
				return false;
			}
			
			// Le premier destacke est l'operande de DROITE.
			// "5 3 -" : on sort 3, puis 5, et on fait 5 - 3 = 2.
			long right = stack.top();
			stack.pop();
			long left = stack.top();
			stack.pop();

			// "1 0 /" est une division par zero : right vaut 0.
			// "0 1 /" vaut 0, c'est valide : on teste right, pas left.
			if (token[0] == '/' && right == 0)
			{
				std::cerr << "Error" << std::endl;
				return false;
			}

			long result = 0;
			if (token[0] == '+')
				result = left + right;
			else if (token[0] == '-')
				result = left - right;
			else if (token[0] == '*')
				result = left * right;
			else
				result = left / right; // division entiere : 7 / 2 donne 3

			stack.push(result);
			continue;
		}

		std::cerr << "Error" << std::endl;
		return false;
	}

	// Un seul nombre doit rester.
	if (stack.size() != 1)
	{
		std::cerr << "Error" << std::endl;
		return false;
	}

	std::cout << stack.top() << std::endl;
	return true;
}
