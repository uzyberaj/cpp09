/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ulyssezyberaj <ulyssezyberaj@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 22:05:56 by ulyssezyber       #+#    #+#             */
/*   Updated: 2026/10/28 22:21:00 by ulyssezyber      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RPN_HPP
# define RPN_HPP

# include <string>

// Notation polonaise inversee : les nombres d'abord, l'operation apres.
// "8 9 *" veut dire 8 * 9. L'operation s'applique toujours aux deux derniers nombres.
//
// std::stack sert de pile : LIFO
// on reprend toujours les deux derniers nombres.
//
// Une stack range ses elements dans un autre conteneur.
// Par defaut deque mais ici je mets une list,

class RPN
{
public:
	RPN();
	RPN(const RPN& other);
	RPN& operator=(const RPN& other);
	~RPN();

	// Affiche le resultat sur la sortie standard.
	// En cas de probleme, affiche "Error" sur la sortie d'erreur et renvoie false.
	bool calculate(const std::string& expression) const;
};

#endif
