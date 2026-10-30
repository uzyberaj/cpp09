/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ulyssezyberaj <ulyssezyberaj@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 23:09:38 by ulyssezyber       #+#    #+#             */
/*   Updated: 2026/10/30 15:14:05 by ulyssezyber      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PmergeMe.hpp"

# include <climits>    // INT_MAX
# include <iostream>
# include <sys/time.h> // gettimeofday

// --- Temps ---

// Heure actuelle en microsecondes.
// gettimeofday est assez fin pour voir l'ecart entre vector et deque.
// clock() est parfois trop grossier et affiche 0.
static double timeUs(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	// Secondes * 1 000 000 + microsecondes = un seul nombre.
	return (tv.tv_sec * 1000000.0) + tv.tv_usec;
}

// --- Lecture des arguments ---

// Accepte un entier de 1 a INT_MAX, ecrit seulement avec des chiffres.
// Refuse "-1", "0", "abc", "+3", une chaine vide, un nombre trop grand.
// out n'est rempli qu'a la fin : en cas d'echec, on ne laisse pas un demi-nombre.
static bool parsePositiveInt(const char* str, int& out)
{
	unsigned long value;
	size_t i;

	if (str == NULL || str[0] == '\0')
		return false;

	value = 0;
	i = 0;
	while (str[i] != '\0')
	{
		unsigned int digit;

		if (str[i] < '0' || str[i] > '9')
			return false;
		// '8' est un caractere. '8' - '0' donne le nombre 8.
		digit = str[i] - '0';
		// On verifie avant de multiplier, sinon value deborde en silence.
		if (value > (unsigned long)INT_MAX / 10)
			return false;
		value = value * 10 + digit;
		if (value > (unsigned long)INT_MAX)
			return false;
		i++;
	}
	if (value == 0)
		return false;
	out = static_cast<int>(value);
	return true;
}

// --- Ordre d'insertion (Jacobsthal) ---

// Apres le tri des grands, les petits b1, b2, b3... attendent.
//
//   a1 a2 a3 a4      b1 b2 b3 b4     nombre seul
//   6  7  8  9       5  4  1  2      3
// b1 = 5 est plus petit que a1 = 6, le plus petit grand.
// On le pose en premier, sans vraiment : [5, 6, 7, 8, 9].
//
// La zone d'un petit, c'est tout ce qui est deja place devant son grand.
// Comparaisons max = ceil(log2(taille de zone + 1))
//   1 element -> 1,  2 a 3 -> 2,  4 a 7 -> 3,  8 a 15 -> 4
// Inserer un petit peut ajouter une case dans la zone des suivants.
// On choisit donc le plus grand indice dont la zone tient encore
// dans le budget, puis on redescend.
//
// Taille zone max = indice candidat + dernier indice traite - 1
// b1 deja pose : dernier = 1. Avec 2 comparaisons, zone max = 3.
// indice + 1 - 1 <= 3, donc on peut aller jusqu'a b3, pas plus.
// b3 puis b2 coute 2 + 2 comparaisons.
// b2 puis b3 coute 2 + 3, parce que b2 peut agrandir la zone de b3.
//
// Borne du groupe : prochain indice = 2^comparaisons - dernier indice.
//   3 = 2^2 - 1,  5 = 2^3 - 3,  11 = 2^4 - 5
// Dans jac, la ligne d'en dessous calcule le meme nombre :
//   dernier deja pose + 2 * l'avant-dernier
//   jac = [1, 3]
//   dernier = 3, avant-dernier = 1
//   3 + 2 * 1 = 5
//   2^3 - 3 = 8 - 3 = 5
//   jac = [1, 3, 5]
//   dernier = 5, avant-dernier = 3
//   5 + 2 * 3 = 11
//   2^4 - 5 = 16 - 5 = 11
//   jac = [1, 3, 5, 11]
static std::vector<size_t> jacobsthalOrder(size_t n)
{
	std::vector<size_t> order;
	std::vector<size_t> jac;
	size_t prev;
	size_t g;

	if (n == 0)
		return order;

	// Borne 1 : b1 tout seul. Borne 3 : le premier vrai groupe (b3 puis b2).
	jac.push_back(1);
	jac.push_back(3);
	while (jac.back() < n)
	{
		size_t next;

		// Dernier nombre de jac + 2 * celui juste avant.
		// [1, 3] -> 3 + 2*1 = 5. Puis [1, 3, 5] -> 5 + 2*3 = 11.
		next = jac[jac.size() - 1] + 2 * jac[jac.size() - 2];
		jac.push_back(next);
	}

	// Positions dans pendingIds, qui commence a la case 0.
	// n°1 -> case 0, n°2 -> case 1, n°3 -> case 2...
	// 0, 2, 1, 4, 3 veut dire : b1, puis b3, puis b2,
	// puis le nombre seul (b5), puis b4.
	// Valeurs de l'exemple : 5, 1, 4, 3, 2.
	prev = 0;
	g = 0;
	while (g < jac.size())
	{
		size_t hi;
		size_t idx;

		hi = jac[g];
		// La derniere borne peut depasser n. On s'arrete au dernier petit.
		if (hi > n)
			hi = n;
		if (hi > prev)
		{
			idx = hi;
			while (idx > prev)
			{
				// La borne compte a partir de 1, pendingIds a partir de 0.
				order.push_back(idx - 1);
				idx--;
			}
		}
		prev = jac[g];
		if (prev >= n)
			break;
		g++;
	}
	return order;
}

// --- Ford-Johnson sur un vector ---

// On trie des indices. Les valeurs restent dans values : une paire survit
// meme quand la recursion deplace les grands.
// Au retour, ids est l'ordre de lecture. values[ids[0]] est le plus petit.
//
// Mon exemple [9, 2, 8, 1, 7, 4, 6, 5, 3] :
// 1. Paires gardees : [9,2] [8,1] [7,4] [6,5], et 3 tout seul.
//    On compare dans la paire. Grands : 9 8 7 6. Petits : 2 1 4 5.
// 2. L'appel recursif refait des paires avec les grands seulement,
//    jusqu'a les avoir tries : 6 7 8 9. Les petits suivent leur grand.
// 3. On reinsere 5, puis 1, puis 4, puis 3, puis 2.
//    5 (b1) devant 6. 1 (b3) devant 8. 4 (b2) devant 7.
//    3 n'a pas de grand : toute la chaine. 2 (b4) devant 9.
static void fordJohnsonVector(std::vector<size_t>& ids, const std::vector<int>& values)
{
	// winners : les grands, dans l'ordre des paires, puis tries par la recursion.
	// loserOf[grand] : le petit de ce grand. C'est la paire conservee.
	// chain : les grands tries. On y insere les petits.
	// pos[id] : case actuelle de ce grand dans chain. Une insertion decale.
	// pendingIds : b1, b2, b3... dans l'ordre des grands tries, puis le nombre seul.
	// pendingWinner : le grand du petit, ou -1 s'il n'a jamais eu de paire.
	std::vector<size_t> winners;
	std::vector<int> loserOf(values.size(), -1);
	std::vector<size_t> chain;
	std::vector<int> pos(values.size(), -1);
	std::vector<size_t> pendingIds;
	std::vector<int> pendingWinner;
	std::vector<size_t> order;
	bool hasLeft;
	size_t leftId;
	size_t limit;
	size_t i;

	// 0 ou 1 element : il n'y a rien a comparer.
	if (ids.size() <= 1)
		return;

	// Paire de deux en deux. Le grand est stocke, le petit reste accroche a lui.
	// Nombre impair : le dernier, comme le 3, n'a pas d'adversaire.
	hasLeft = (ids.size() % 2) != 0;
	leftId = 0;
	limit = ids.size();
	if (hasLeft)
	{
		leftId = ids.back();
		limit = ids.size() - 1;
	}
	i = 0;
	while (i < limit)
	{
		size_t x;
		size_t y;

		x = ids[i];
		y = ids[i + 1];
		if (values[x] < values[y])
		{
			winners.push_back(y);
			loserOf[y] = static_cast<int>(x);
		}
		else
		{
			winners.push_back(x);
			loserOf[x] = static_cast<int>(y);
		}
		i += 2;
	}

	// Meme algorithme, seulement sur les grands.
	// C'est le tri recursif des a : on re-paire 9 avec 8, 7 avec 6, et ainsi de suite,
	// jusqu'a 6 7 8 9. loserOf est range par l'id du grand, pas par sa place :
	// quand 9 change de position, son petit 2 est toujours loserOf[9].
	fordJohnsonVector(winners, values);

	// Chaine de depart : les grands tries. Dans l'exemple, 6 7 8 9.
	chain = winners;
	i = 0;
	while (i < chain.size())
	{
		pos[chain[i]] = static_cast<int>(i);
		i++;
	}

	// Petits alignes sur les grands tries : b1 sous a1, b2 sous a2...
	// Exemple : 5 sous 6, 4 sous 7, 1 sous 8, 2 sous 9.
	i = 0;
	while (i < winners.size())
	{
		pendingIds.push_back(static_cast<size_t>(loserOf[winners[i]]));
		pendingWinner.push_back(static_cast<int>(winners[i]));
		i++;
	}
	// Le 3 n'a pas de partenaire. -1 : sa zone sera toute la chaine.
	if (hasLeft)
	{
		pendingIds.push_back(leftId);
		pendingWinner.push_back(-1);
	}

	// Jacobsthal donne les cases de pendingIds : b1, b3, b2, puis la suite.
	// Pour l'exemple, 5 puis 1 puis 4 puis 3 puis 2.
	order = jacobsthalOrder(pendingIds.size());
	i = 0;
	while (i < order.size())
	{
		size_t id;
		size_t end;
		size_t lo;
		size_t hi;
		int val;
		size_t k;

		k = order[i];
		id = pendingIds[k];
		val = values[id];
		// Zone = les cases devant le grand. 1 (paire de 8) ne regarde pas apres 8.
		// Le nombre seul n'a pas de grand : on cherche dans toute la chaine.
		// end est exclu. Pour b3=1 devant 8, la zone de depart est 5 6 7.
		if (pendingWinner[k] < 0)
			end = chain.size();
		else
			end = static_cast<size_t>(pos[pendingWinner[k]]);

		// Recherche binaire, comme ouvrir un dictionnaire au milieu.
		// Si le milieu est plus petit que le nombre, on jette la gauche.
		// Sinon on jette la droite, milieu compris.
		// 1 dans [5, 6, 7] : milieu 6, 1 est plus petit, puis 5. Deux comparaisons.
		// ceil(log2(3 + 1)) = 2.
		lo = 0;
		hi = end;
		while (lo < hi)
		{
			size_t mid;

			mid = lo + (hi - lo) / 2;
			if (values[chain[mid]] < val)
				lo = mid + 1;
			else
				hi = mid;
		}
		chain.insert(chain.begin() + lo, id);
		// Inserer agrandit la chaine. Les pos des grands a droite sont recalculees,
		// sinon la zone du prochain petit partirait de la mauvaise case.
		while (lo < chain.size())
		{
			pos[chain[lo]] = static_cast<int>(lo);
			lo++;
		}
		i++;
	}

	// ids ressort dans l'ordre trie. swap echange les tableaux sans les recopier.
	ids.swap(chain);
}

// Les valeurs ne bougent pas pendant le tri. ids[i] = i au debut :
// l'indice 0 represente le premier chiffre. Ford-Johnson deplace ces indices.
// A la fin on relit values dans l'ordre des indices tries.
static void sortDataVector(std::vector<int>& data)
{
	std::vector<size_t> ids(data.size());
	std::vector<int> sorted(data.size());
	size_t i;

	if (data.size() < 2)
		return;
	// Au debut, l'indice i represente data[i].
	i = 0;
	while (i < data.size())
	{
		ids[i] = i;
		i++;
	}
	fordJohnsonVector(ids, data);
	i = 0;
	while (i < data.size())
	{
		sorted[i] = data[ids[i]];
		i++;
	}
	data.swap(sorted);
}

// --- Ford-Johnson sur une deque ---
// Les trois memes etapes que fordJohnsonVector :
// paires conservees, recursion sur les grands, insertion Jacobsthal
// avec recherche binaire bornee au partenaire.
// Recopie a la main : l'insert au milieu doit etre celui de la deque,
// sinon le chrono ne mesure pas ce conteneur.
// jacobsthalOrder reste un vector. Il ne trie pas les chiffres,
// il donne seulement l'ordre des b.

static void fordJohnsonDeque(std::deque<size_t>& ids, const std::deque<int>& values)
{
	std::deque<size_t> winners;
	std::deque<int> loserOf(values.size(), -1);
	std::deque<size_t> chain;
	std::deque<int> pos(values.size(), -1);
	std::deque<size_t> pendingIds;
	std::deque<int> pendingWinner;
	std::vector<size_t> order;
	bool hasLeft;
	size_t leftId;
	size_t limit;
	size_t i;

	if (ids.size() <= 1)
		return;

	// Paire : le plus grand va dans winners, le plus petit dans loserOf.
	// Nombre impair : le dernier est mis de cote, il n'a pas d'adversaire.
	hasLeft = (ids.size() % 2) != 0;
	leftId = 0;
	limit = ids.size();
	if (hasLeft)
	{
		leftId = ids.back();
		limit = ids.size() - 1;
	}
	i = 0;
	while (i < limit)
	{
		size_t x;
		size_t y;

		x = ids[i];
		y = ids[i + 1];
		if (values[x] < values[y])
		{
			winners.push_back(y);
			loserOf[y] = static_cast<int>(x);
		}
		else
		{
			winners.push_back(x);
			loserOf[x] = static_cast<int>(y);
		}
		i += 2;
	}

	// Meme recursion que le vector : on retrie seulement les grands.
	fordJohnsonDeque(winners, values);

	chain = winners;
	i = 0;
	while (i < chain.size())
	{
		pos[chain[i]] = static_cast<int>(i);
		i++;
	}

	// Petits dans l'ordre des grands. Le nombre seul a un gagnant fictif -1.
	i = 0;
	while (i < winners.size())
	{
		pendingIds.push_back(static_cast<size_t>(loserOf[winners[i]]));
		pendingWinner.push_back(static_cast<int>(winners[i]));
		i++;
	}
	if (hasLeft)
	{
		pendingIds.push_back(leftId);
		pendingWinner.push_back(-1);
	}

	// Meme ordre que le vector : b1, puis b3, b2, etc.
	order = jacobsthalOrder(pendingIds.size());
	i = 0;
	while (i < order.size())
	{
		size_t id;
		size_t end;
		size_t lo;
		size_t hi;
		int val;
		size_t k;

		k = order[i];
		id = pendingIds[k];
		val = values[id];
		// Zone de recherche : devant le grand, ou toute la chaine si -1.
		if (pendingWinner[k] < 0)
			end = chain.size();
		else
			end = static_cast<size_t>(pos[pendingWinner[k]]);

		lo = 0;
		hi = end;
		// Dictionnaire : milieu plus petit, on jette la gauche. Sinon la droite.
		while (lo < hi)
		{
			size_t mid;

			mid = lo + (hi - lo) / 2;
			if (values[chain[mid]] < val)
				lo = mid + 1;
			else
				hi = mid;
		}
		// insert de la deque : c'est ce decalage qui coute plus cher
		// que dans un vector, parce que les cases sont en plusieurs blocs.
		chain.insert(chain.begin() + lo, id);
		while (lo < chain.size())
		{
			pos[chain[lo]] = static_cast<int>(lo);
			lo++;
		}
		i++;
	}

	ids.swap(chain);
}

// Comme sortDataVector, avec des deque. ids trie des numeros de case,
// sorted reconstruit les valeurs a la fin.
static void sortDataDeque(std::deque<int>& data)
{
	std::deque<size_t> ids(data.size());
	std::deque<int> sorted(data.size());
	size_t i;

	if (data.size() < 2)
		return;
	i = 0;
	while (i < data.size())
	{
		ids[i] = i;
		i++;
	}
	fordJohnsonDeque(ids, data);
	i = 0;
	while (i < data.size())
	{
		sorted[i] = data[ids[i]];
		i++;
	}
	data.swap(sorted);
}

// Les methodes de la classe appellent le tri ecrit plus haut.

void PmergeMe::sortVector(std::vector<int>& data)
{
	sortDataVector(data);
}

void PmergeMe::sortDeque(std::deque<int>& data)
{
	sortDataDeque(data);
}

// --- Affichage ---

// "Before: " ou "After: ", puis les nombres separes par un espace.
static void printSequence(const char* label, const std::vector<int>& data)
{
	size_t i;

	std::cout << label;
	i = 0;
	while (i < data.size())
	{
		if (i != 0)
			std::cout << " ";
		std::cout << data[i];
		i++;
	}
	std::cout << std::endl;
}

// --- Forme canonique ---
// L'objet ne garde aucune suite. Tout vit dans run(), puis disparait.
// (void)other evite l'avertissement "parametre inutilise" avec -Werror.

PmergeMe::PmergeMe()
{
}

PmergeMe::PmergeMe(const PmergeMe& other)
{
	(void)other;
}

PmergeMe& PmergeMe::operator=(const PmergeMe& other)
{
	(void)other;
	return *this;
}

PmergeMe::~PmergeMe()
{
}

// --- Programme ---

bool PmergeMe::run(int argc, char** argv) const
{
	std::vector<int> input;
	std::vector<int> vec;
	std::deque<int> deq;
	double start;
	double vectorUs;
	double dequeUs;
	int i;

	if (argc < 2)
	{
		std::cerr << "Error" << std::endl;
		return false;
	}
	i = 1;
	while (i < argc)
	{
		int number;

		if (!parsePositiveInt(argv[i], number))
		{
			std::cerr << "Error" << std::endl;
			return false;
		}
		input.push_back(number);
		i++;
	}

	// Le chrono englobe la copie et le tri.
	// Temps de toute l'operation, pas seulement la boucle.
	start = timeUs();
	vec = input;
	sortVector(vec);
	vectorUs = timeUs() - start;

	// input et vec sont deux vector : vec = input copie les nombres.
	// deq est une deque. deq = input ne compile pas, les types different.
	// assign lit input du debut a la fin et remplit la deque avec les memes nombres.
	start = timeUs();
	deq.assign(input.begin(), input.end());
	sortDeque(deq);
	dequeUs = timeUs() - start;

	// Les deux tris doivent donner la meme suite. Sinon on n'affiche rien.
	if (vec.size() != deq.size())
	{
		std::cerr << "Error" << std::endl;
		return false;
	}
	i = 0;
	while (i < (int)vec.size())
	{
		if (vec[i] != deq[i])
		{
			std::cerr << "Error" << std::endl;
			return false;
		}
		i++;
	}

	printSequence("Before: ", input);
	printSequence("After: ", vec);
	// fixed + precision(2) : "18.00 us", pas "18" ni une longue suite de decimales.
	std::cout << std::fixed;
	std::cout.precision(2);
	std::cout << "Time to process a range of " << input.size()
		<< " elements with std::vector : " << vectorUs << " us" << std::endl;
	std::cout << "Time to process a range of " << input.size()
		<< " elements with std::deque : " << dequeUs << " us" << std::endl;
	return true;
}
