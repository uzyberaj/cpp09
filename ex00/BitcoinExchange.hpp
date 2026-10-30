/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ulyssezyberaj <ulyssezyberaj@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 22:04:53 by ulyssezyber       #+#    #+#             */
/*   Updated: 2026/10/28 22:04:54 by ulyssezyber      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BITCOINEXCHANGE_HPP
# define BITCOINEXCHANGE_HPP

# include <map>     // std::map, conteneur date -> cours
# include <string>  // std::string, dates et nom du fichier

// Le constructeur lit data.csv. processInput lit le fichier que je passe a ./btc.
//
// std::map parce qu'on associe une date a un cours,
// et il faut que les dates restent triees.
// La map range chaque nouvelle cle a sa place toute seule.
// 
// lower_bound s'appuie la-dessus pour trouver une date en coupant
// la collection en deux a chaque essai, au lieu de tout lire depuis le debut.
//
// Un vector garderait l'ordre du fichier. Je devrais trier, puis ecrire
// la recherche de la date d'avant moi-meme.
// Une list se parcourt maillon par maillon, du debut jusqu'a la date.
// Une pile ne laisse voir qu'un seul element.

class BitcoinExchange
{
public:
	// Forme canonique
	BitcoinExchange();
	BitcoinExchange(const BitcoinExchange& other);
	BitcoinExchange& operator=(const BitcoinExchange& other);
	~BitcoinExchange();

	// Parcourt le fichier d'entree et affiche un resultat ou une erreur par ligne.
	// const : lecture seule, on ne modifie pas la base.
	void processInput(const std::string& filename) const;

private:
	// Cle = "YYYY-MM-DD", valeur = cours du jour.
	// Comparer deux string compare leurs caracteres.
	// "2011-01-03" est bien avant "2011-01-09", sans trier a la main.
	std::map<std::string, double> _database;

	// true seulement si data.csv s'est ouvert.
	// S'il reste false, processInput s'arrete tout de suite.
	bool _databaseOk;

	// Remplit _database. Prive : main cree l'objet, elle n'ouvre pas le csv elle-meme.
	bool loadDatabase(const std::string& filename);
};

#endif
