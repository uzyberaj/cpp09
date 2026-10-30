/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ulyssezyberaj <ulyssezyberaj@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 22:05:47 by ulyssezyber       #+#    #+#             */
/*   Updated: 2026/10/28 22:05:48 by ulyssezyber      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RPN.hpp"

# include <iostream>

// Notation polonaise inversee.
//
// Usage : ./RPN "expression"
//
// Une seule chaine, entre guillemets. Sans guillemets le shell coupe
// sur les espaces, et * devient un motif de fichiers.
//
// Le resultat (un nombre) va sur la sortie standard.
// "Error" va sur la sortie d'erreur. On n'affiche pas les deux.
//
// Exemples du sujet :
//   ./RPN "8 9 * 9 - 9 - 9 - 4 - 1 +"    -> 42
//   ./RPN "7 7 * 7 -"                    -> 42
//   ./RPN "1 2 * 2 / 2 * 2 4 - +"        -> 0
//   ./RPN "(1 + 1)"                      -> Error
//
// Autres formules, calculees a la main :
//   ./RPN "9 8 * 4 * 4 / 2 + 9 - 8 - 8 - 1 - 6 -"           -> 42
//   ./RPN "1 2 * 2 / 2 + 5 * 6 - 1 3 * - 4 5 * * 8 /"     -> 15
//
// Le resultat peut depasser 9 :  ./RPN "8 9 *"  -> 72
// L'ordre compte :               ./RPN "5 3 -" -> 2
//                                ./RPN "3 5 -" -> -2
// Division entiere :             ./RPN "7 2 /" -> 3
// Division par zero, parenthese, chiffre manquant,
// nombre a plusieurs chiffres, expression vide : Error

int main(int argc, char** argv)
{
	// Le sujet demande une expression en argument. Zero ou deux arguments : Error.
	if (argc != 2)
	{
		std::cerr << "Error" << std::endl;
		return 1;
	}

	RPN rpn;
	if (!rpn.calculate(argv[1]))
		return 1;
	return 0;
}
