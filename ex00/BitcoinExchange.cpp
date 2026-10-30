/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ulyssezyberaj <ulyssezyberaj@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 22:04:46 by ulyssezyber       #+#    #+#             */
/*   Updated: 2026/10/28 22:04:47 by ulyssezyber      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BitcoinExchange.hpp"

# include <cstdlib>   // atoi et strtod : passer d'un texte a un nombre
# include <fstream>   // ifstream : ouvrir fichier en lecture
# include <iomanip>   // fixed et setprecision : choisir les decimales d'un double
# include <iostream>
# include <sstream>   // ostringstream : ecrire un nombre dans une string

// --- Fonctions utilitaires ---

// Enleve les espaces et les tabulations au debut et a la fin.
// "  2011-01-03  " -> "2011-01-03". On ne touche pas au milieu.
static std::string trim(const std::string& str)
{
	// size_type -> type que string utilise pour une position.
	// start = premier caractere garde. end = juste apres le dernier garde.
	std::string::size_type start = 0;
	std::string::size_type end = str.length();

	while (start < end && (str[start] == ' ' || str[start] == '\t'))
		start++;
	// end - 1 : le dernier caractere est a l'index longueur - 1.
	while (end > start && (str[end - 1] == ' ' || str[end - 1] == '\t'))
		end--;

	// substr(position, nombre de caracteres).
	// retourne la sous-chaine entre start et end.
	return str.substr(start, end - start);
}

// Date YYYY-MM-DD -> "2011-01-03" = 10 caracteres.
// Je refuse un mois a 42, un 31 fevrier, ou des lettres a la place des chiffres.
static bool isValidDate(const std::string& date)
{
	if (date.length() != 10)
		return false;
	// Les tirets sont fixes : pos 4 et 7. Le reste doit etre des chiffres.
	if (date[4] != '-' || date[7] != '-')
		return false;

	for (int i = 0; i < 10; i++)
	{
		if (i == 4 || i == 7)
			continue;
		if (date[i] < '0' || date[i] > '9')
			return false;
	}

	// substr decoupe l'annee, le mois, le jour.
	// c_str() donne le const char* que atoi attend.
	int year = std::atoi(date.substr(0, 4).c_str());
	int month = std::atoi(date.substr(5, 2).c_str());
	int day = std::atoi(date.substr(8, 2).c_str());

	if (month < 1 || month > 12)
		return false;

	// daysInMonth[1] = janvier, [2] = fevrier, etc.
	int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	// Bissextile : divisible par 4, mais les siecles ne le sont pas,
	// sauf ceux divisibles par 400. 2000 oui, 1900 non. Fevrier passe a 29.
	bool leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
	if (leap)
		daysInMonth[2] = 29;

	if (day < 1 || day > daysInMonth[month])
		return false;

	return true;
}

// Sur Windows une fin de ligne s'ecrit \r\n. getline enleve le \n et garde le \r.
// Ce \r colle a la date, et "2011-01-03\r" ne vaudrait plus la cle de la map.
static void stripCarriageReturn(std::string& line)
{
	if (!line.empty() && line[line.length() - 1] == '\r')
		line.erase(line.length() - 1);
}

// On écrit d'abord 6 decimales, puis on enleve les zeros de la fin.
// 6 decimales effacent aussi le bruit des double :
// 0.1 n'est pas stocke exactement en binaire, et un produit peut sortir
// 33514870.00000001. A 6 chiffres ce residu retombe sur des zeros, que je coupe.
static std::string formatNumber(double value)
{
	// 0 et -0 sont egaux pour ==, mais l'affichage peut ecrire "-0".
	// On forceun vrai 0 pour que la sortie reste "0".
	if (value == 0)
		value = 0;

	// ostringstream est un cout qui ecrit dans une string au lieu de l'ecran.
	// fixed + setprecision(6) : toujours 6 chiffres apres le point, pas 6 au total.
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(6) << value;

	std::string str = oss.str();
	// Tant que ca finit par 0 et qu'il y a un point, j'enleve le dernier caractere.
	// "3.100000" -> "3.1", puis je m'arrete. "3.000000" ira jusqu'a "3."
	while (str.find('.') != std::string::npos && str[str.length() - 1] == '0')
		str.erase(str.length() - 1);
	// Le point tout seul a la fin ne sert plus : "3." -> "3".
	if (!str.empty() && str[str.length() - 1] == '.')
		str.erase(str.length() - 1);
	return str;
}

// --- Forme canonique ---

BitcoinExchange::BitcoinExchange() : _databaseOk(false)
{
	// On charge data.csv tout de suite,
	// pour que processInput trouve la base deja prete.
	_databaseOk = loadDatabase("data.csv");
}

// La map se copie toute seule : chaque date et chaque cours sont dupliques.
BitcoinExchange::BitcoinExchange(const BitcoinExchange& other)
	: _database(other._database), _databaseOk(other._databaseOk)
{
}

BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange& other)
{
	if (this != &other)
	{
		_database = other._database;
		_databaseOk = other._databaseOk;
	}
	return *this;
}

BitcoinExchange::~BitcoinExchange()
{
}

// --- Lecture de data.csv ---

bool BitcoinExchange::loadDatabase(const std::string& filename)
{
	// ifstream : flux de lecture branche sur un fichier.
	// En C++98 le constructeur prend un const char*. string n'est accepte, d'ou .c_str().
	std::ifstream file(filename.c_str());
	if (!file.is_open())
	{
		std::cout << "Error: could not open database." << std::endl;
		return false;
	}

	std::string line;

	// Si getline echoue, le fichier est vide. Il est ouvert, on renvoi true.
	if (!std::getline(file, line))
		return true;

	// Chaque tour lit une ligne, sans le \n. Quand il n'y en a plus, getline
	// rend le flux "faux" et le while s'arrete. Je ne compte pas les lignes d'avance.
	while (std::getline(file, line))
	{
		stripCarriageReturn(line);
		if (line.empty())
			continue;

		// find renvoie l'index de la virgule. npos veut dire "introuvable".
		// Pas -1 : c'est une position qui ne peut pas exister dans la chaine.
		std::string::size_type comma = line.find(',');
		if (comma == std::string::npos)
			continue;

		// "2011-01-01,0.3" -> date = "2011-01-01", rateStr = "0.3"
		std::string date = line.substr(0, comma);
		std::string rateStr = line.substr(comma + 1);

		// les taux du csv fourni sont deja des nombres propres.
		double rate = std::strtod(rateStr.c_str(), NULL);

		// operator[] : cree la case si la date est nouvelle, sinon remplace le cours.
		// Une date du csv n'a qu'un seul prix. La map l'insere deja triee.
		_database[date] = rate;
	}
	return true;
}

// --- Fichier passe a ./btc ---

void BitcoinExchange::processInput(const std::string& filename) const
{
	if (!_databaseOk)
		return;

	std::ifstream file(filename.c_str());
	if (!file.is_open())
	{
		std::cout << "Error: could not open file." << std::endl;
		return;
	}

	std::string line;
	bool firstLine = true;

	while (std::getline(file, line))
	{
		stripCarriageReturn(line);

		if (firstLine)
		{
			firstLine = false;
			if (line == "date | value")
				continue;
		}

		if (line.empty())
			continue;

		// "2001-42-42" n'a pas de '|' : bad input, on affiche la ligne entiere.
		// On traite tout le fichier, meme apres une erreur.
		std::string::size_type pipe = line.find('|');
		if (pipe == std::string::npos)
		{
			std::cout << "Error: bad input => " << line << std::endl;
			continue;
		}

		// trim : le format reel est "date | value", avec des espaces autour du pipe.
		std::string date = trim(line.substr(0, pipe));
		std::string valueStr = trim(line.substr(pipe + 1));

		// On affiche seulement la date, pas la ligne. "2011-02-31 | 1"
		// donne "Error: bad input => 2011-02-31".
		if (!isValidDate(date))
		{
			std::cout << "Error: bad input => " << date << std::endl;
			continue;
		}

		// "2011-01-03 |" : la date est bonne mais il n'y a pas de montant.
		if (valueStr.empty())
		{
			std::cout << "Error: bad input => " << line << std::endl;
			continue;
		}

		// Verification du texte avant strtod.
		// Forme acceptee : un + ou un - au debut, des chiffres, et un seul point.
		bool validValue = true;
		bool hasDot = false;
		std::string::size_type start = 0;

		if (!valueStr.empty() && (valueStr[0] == '+' || valueStr[0] == '-'))
			start = 1;

		// Signe tout seul : "+" ou "-".
		if (start >= valueStr.length())
			validValue = false;

		for (std::string::size_type i = start; i < valueStr.length(); i++)
		{
			if (valueStr[i] == '.')
			{
				if (hasDot)
					validValue = false;
				hasDot = true;
			}
			else if (valueStr[i] < '0' || valueStr[i] > '9')
				validValue = false;
		}

		// endPtr avance jusqu'au premier caractere que strtod n'a pas lu.
		char* endPtr;
		double value = std::strtod(valueStr.c_str(), &endPtr);
		if (!validValue || *endPtr != '\0')
		{
			std::cout << "Error: bad input => " << line << std::endl;
			continue;
		}

		// L'ordre des tests decide du message.
		if (value < 0)
		{
			std::cout << "Error: not a positive number." << std::endl;
			continue;
		}

		// 1000 passe. 1000.1 et 2147483648 tombent ici.
		if (value > 1000)
		{
			std::cout << "Error: too large a number." << std::endl;
			continue;
		}

		// lower_bound : la premiere case dont la date est >= a celle demandee.
		// const_iterator parce que la methode est const.
		// On lit it->first (la date) et it->second (le cours).

		std::map<std::string, double>::const_iterator it = _database.lower_bound(date);
		if (it != _database.end() && it->first == date)
		{
			// La date est dans la base. it est deja dessus, on ne bouge pas.
		}
		else if (it == _database.begin())
		{
			// it pointe sur la date la plus ancienne, et ce n'est pas celle demandee.
			// Toutes les dates de la base sont donc plus recentes.
			// --it sortirait de la map : aucun cours anterieur n'existe.
			std::cout << "Error: bad input => " << line << std::endl;
			continue;
		}
		else
		{
			// Cas normal quand la date n'est pas dans le csv.
			// it est sur la premiere date d'apres, ou sur end() si la mienne
			// est plus recente que toute la base.
			// --it recule d'une case : dernier cours connu avant ce jour.
			--it;
		}

		// J'affiche la date demandee, pas celle du cours utilise.
		double result = value * it->second;
		std::cout << date << " => " << formatNumber(value)
			<< " = " << formatNumber(result) << std::endl;
	}
}
