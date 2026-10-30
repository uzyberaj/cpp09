<p align="left">
  <a href="#--------cpp09--">🇬🇧 English</a> |
  <a href="#--------cpp09---1">🇫🇷 Français</a>
</p>

<h1 align="center">
  <br>
  <img src="https://i.ibb.co/MkL14CvZ/logo-cpp-bonus.png" alt="C++ logo" width="150">
  <br>
  cpp09
  <br>
</h1>

<h4 align="center"><em>C++ Module 09 — STL containers: a map, a stack, and two sorts.</em></h4>

<p align="center">
  <img src="https://img.shields.io/badge/exercises-3-orange">
  <img src="https://img.shields.io/badge/C%2B%2B-100%25-blue">
  <img src="https://img.shields.io/badge/Grade-100%2F100-brightgreen">
  <img src="https://img.shields.io/badge/standard-C%2B%2B98-informational">
</p>

<p align="center">
  <a href="./subject.md">Subject</a> |
  <a href="#-project-overview">Overview</a> •
  <a href="#-code-architecture-and-organization">Architecture</a> •
  <a href="#-operating">Operating</a> •
  <a href="#%EF%B8%8F-compiling-and-testing">Compiling</a>
</p>

---

## 📌 **Project overview**

**cpp09** is the last C++ module of the 42 Common Core. It uses the **STL containers** on three small programs: a bitcoin exchange, a reverse Polish calculator, and the **Ford-Johnson** merge-insertion sort.

Each exercise must use at least one container. A container used in one exercise cannot be used again in the next ones. Exercise 02 uses two. All code complies with **C++98**.

The three programs are independent. They do not share classes.

### 1. Exercise 00 — Bitcoin Exchange ✅

`btc` reads `data.csv` (date, exchange rate) into a **`std::map`**, then values each line of a second file:

* Input lines have the form `date | value`
* A valid date is `Year-Month-Day`. A valid value is a float or a positive integer, from **0 to 1000** included
* If the date is missing from the database, the program uses the **closest earlier** date, never a later one
* One bad line prints an error and the rest of the file is still processed
* `data.csv` must sit next to the binary. The database is loaded in the constructor

### 2. Exercise 01 — Reverse Polish Notation ✅

`RPN` evaluates one expression passed as a single argument, with a **`std::stack`** stored in a **`std::list`**:

* Tokens are separated by spaces. Operators are `+`, `-`, `/` and `*`
* Input numbers are single digits (`0`–`9`). The result of the calculation may be larger than 9
* Parentheses and decimal numbers are rejected
* The result is printed on standard output. `Error` goes to standard error
* Division is an integer division. Division by zero is an error

### 3. Exercise 02 — PmergeMe ✅

`PmergeMe` sorts a sequence of positive integers with the **Ford-Johnson** algorithm (merge-insertion sort), once in a **`std::vector`** and once in a **`std::deque`**:

* Pairs are kept: the larger value is sorted recursively, the smaller one stays attached to it
* The smaller values are then inserted by binary search, in **Jacobsthal** order, only in front of their partner
* The program prints the sequence before and after, then the time taken by each container
* The timer covers the copy into the container **and** the sort
* Duplicates are kept. `0`, a negative number, or a non-numeric argument prints `Error` on standard error

### Error handling ⚠️

These programs do not throw exceptions. They print a message and keep going, or stop when the input itself cannot be opened.

| Exercise | Output | When |
|----------|--------|------|
| ex00 | `Error: could not open file.` | Missing argument, or input file cannot be opened |
| ex00 | `Error: could not open database.` | `data.csv` cannot be opened |
| ex00 | `Error: bad input => …` | Bad date, missing `\|`, or a value that is not a number |
| ex00 | `Error: not a positive number.` | Value `< 0` |
| ex00 | `Error: too large a number.` | Value `> 1000` |
| ex01 | `Error` | Bad token, not enough operands, division by zero, or more than one value left on the stack |
| ex02 | `Error` | No argument, a non-numeric token, `0`, a negative number, or an integer above `INT_MAX` |

Exercise 00 writes every message on **standard output**. Exercises 01 and 02 write `Error` on **standard error**.

---

## 📂 **Code architecture and organization**

```
cpp09/
├── ex00/   BitcoinExchange.hpp + BitcoinExchange.cpp + main.cpp — btc
│           data.csv (exchange rates) + input.txt (subject example)
├── ex01/   RPN.hpp + RPN.cpp + main.cpp — RPN
├── ex02/   PmergeMe.hpp + PmergeMe.cpp + main.cpp — PmergeMe
├── subject.md
└── evaluation_sheet.md
```

#### 🔹 `BitcoinExchange` (ex00)

| Method | Description |
|--------|-------------|
| `BitcoinExchange()` | Loads `data.csv` into `std::map<std::string, double>` |
| `processInput(filename) const` | Reads the input file line by line and prints a value or an error |
| `loadDatabase(filename)` | Private. Fills the map. `processInput` does nothing if this failed |

#### 🔹 `RPN` (ex01)

| Method | Description |
|--------|-------------|
| `calculate(expression) const` | Evaluates the expression. Prints the result and returns `true`, or prints `Error` and returns `false` |

The stack is local to `calculate`. The class stores no data. The canonical form is empty for that reason.

#### 🔹 `PmergeMe` (ex02)

| Method | Description |
|--------|-------------|
| `run(argc, argv) const` | Parses the arguments, sorts them in both containers, prints the sequence and both times |
| `sortVector(data)` | Private. Ford-Johnson on a `std::vector` |
| `sortDeque(data)` | Private. The same algorithm, written again on a `std::deque` |

`jacobsthalOrder` is a free function. It only builds the insertion order. It does not sort the numbers.

---

## 🏛 **Operating**

### 🔸 **One container per exercise**

The module forbids reusing a container:

| Exercise | Container | Why this one |
|----------|-----------|----------------|
| ex00 | `std::map` | A date is a key, a rate is a value, and the keys stay sorted |
| ex01 | `std::stack` stored in a `std::list` | Each operator needs the last two numbers. `vector` and `deque` stay free for ex02 |
| ex02 | `std::vector` and `std::deque` | Both give direct access and can insert in the middle. The time differs because their memory is not laid out the same way |

A `map` keeps keys in order by itself. With dates written `YYYY-MM-DD`, alphabetical order is chronological order. `lower_bound` finds the first date greater than or equal to the requested one. If it is not an exact match, stepping back one element is the closest earlier date.

A `stack` is an adapter. `push`, `top` and `pop` only see the top. `pop` returns nothing: the value is read with `top`, then removed. The underlying container here is a `list`, so ex02 can still use `vector` and `deque`. In C++98 the two closing brackets of `std::stack<long, std::list<long> >` must be separated by a space.

### 🔸 **Reverse Polish notation (ex01)**

The operator comes after its operands. `8 9 *` means `8 * 9`. The first value popped is the **right** operand: `5 3 -` is `5 - 3`, which is `2`.

```text
8  9  *   →  stack: 72
72 9  -   →  stack: 63
```

At the end exactly one number must remain. `"7"` is valid and prints `7`. `"1 2 3 +"` leaves two numbers and is an error.

### 🔸 **Ford-Johnson (ex02)**

Ford-Johnson, also called merge-insertion sort, has three moves:

1. Pair the numbers and **keep the pairs**.
2. Sort the larger value of each pair with the same algorithm.
3. Insert the smaller values back by binary search, in Jacobsthal order, only in front of their partner.

Take the unsorted list:

```text
[9, 2, 8, 1, 7, 4, 6, 5, 3]
```

#### Pairs

Group them, and remember which small number belongs to which large one. The last number has no partner. It will be inserted later, over the whole chain.

```text
[9, 2]   [8, 1]   [7, 4]   [6, 5]   [3]
```

Compare inside each pair. The larger value stays on top, the smaller one stays underneath:

```text
9    8    7    6    (3)
2    1    4    5
```

#### Sorting the larger values

The larger values are compared with each other. The pairs are not broken: when a large value moves, its small partner moves with it.

Compare neighbours:

```text
9 > 8          7 > 6
```

Reorder those pairs:

```text
8    9    6    7    (3)
1    2    5    4
```

Pair them again. `8 > 6`, and `9 > 8` is already known:

```text
6    8    9    7    (3)
5    1    2    4
```

`7 > 6` is already known, so `7` can only be compared with `8` or with `9`. After those comparisons the larger values are sorted, and the pairs are still intact:

```text
6    7    8    9
5    4    1    2
```

In the code this whole step is the recursive call on `winners`. `loserOf[large]` still holds the small partner, because it is indexed by the large value's identity, not by its position.

Name them:

```text
a1  a2  a3  a4          b1  b2  b3  b4     b5
6   7   8   9           5   4   1   2      3
```

`b5` is the unpaired `3`.

#### Inserting the smaller values

`b1` is inserted first. `6` is the smallest large value, and `5` is smaller than `6`, so `5` is smaller than every large value already in the chain:

```text
[5, 6, 7, 8, 9]
```

Four candidates remain: `4`, `1`, `2` and `3`.

Jacobsthal does not look at which candidate is the smallest number. It looks at **where its partner sits**, and at how many elements the previous insertions could add in front of that partner. Each insertion can grow the search zone of the candidates that come after it. The goal is to use a zone while it is still small.

The search zone of a small value is every element already placed **before its large partner**.

Binary search opens that zone in the middle and throws away the half that cannot contain the number, the way you open a dictionary.

If `4` (partner `7`) is inserted first, it must land before `7`, so the zone is `[5, 6]`:

```text
[5, 6, 7, 8, 9]
    ↑
4 < 6    → drop the right side
4 < 5    → one comparison left

[4, 5, 6, 7, 8, 9]
```

That is **2** comparisons.

Then `1` (partner `8`) searches `[4, 5, 6, 7]`:

```text
[4, 5, 6, 7]
       ↑
1 < 6
1 < 5
1 < 4
```

That is **3** comparisons. Total: **5**.

If `1` is inserted first, its zone stops before `8`, so it is `[5, 6, 7]`:

```text
[5, 6, 7]
    ↑
1 < 6
1 < 5

[1, 5, 6, 7, 8, 9]
```

That is **2** comparisons.

Then `4` (partner `7`) searches only up to `7`:

```text
[1, 5, 6, 7, 8, 9]
    ↑
4 < 5
4 > 1

[1, 4, 5, 6, 7, 8, 9]
```

That is **2** comparisons again. Total: **4** instead of **5**.

So `b3` is inserted before `b2`. Inserting `b2` first can add one element in front of `b3`'s partner and push that zone from 3 elements to 4. `b2`'s partner is earlier in the chain, so its zone cannot grow past 3 elements if `b3` goes first:

```text
b2 then b3  →  2 + 3 = 5 comparisons
b3 then b2  →  2 + 2 = 4 comparisons
```

#### How many comparisons a zone costs

```text
max comparisons = ceil(log2(zone size + 1))
```

`ceil` rounds up to the next integer. `log2(x)` is the power to which 2 must be raised to obtain `x`:

```text
log2(2) = 1     because 2^1 = 2
log2(4) = 2     because 2^2 = 4
log2(8) = 3     because 2^3 = 8
```

The zone size is the number of elements already sitting before the partner. For `1`, partner of `8`, in `[5, 6, 7, 8, 9]`:

```text
zone size = 3          (5, 6, 7)
ceil(log2(3 + 1)) = ceil(log2(4)) = 2
```

The same limits, without computing the logarithm each time:

| Zone size | Max comparisons |
|-----------|-----------------|
| 1 | 1 |
| 2 to 3 | 2 |
| 4 to 7 | 3 |
| 8 to 15 | 4 |
| 16 to 31 | 5 |

#### Choosing the next group

Three quantities decide the next candidate:

* **Candidate index** — the small value under consideration (`b2`, `b3`, `b4`…)
* **Last handled index** — every small value up to this index is already inserted
* **Max zone size** — the largest search that candidate could face

```text
max zone size = candidate index + last handled index - 1
```

This size does not depend on the final position of the number. It is the worst case: the partner, plus every small value already inserted that might have landed in front of it.

`b1` is already placed, so the last handled index is `1`.

For `b3 = 1`, partner `a3 = 8`. Three elements sit before `8`:

```text
max zone size = 3 + (1 - 1) = 3     →  2 comparisons
```

For `b4 = 2`, partner `a4 = 9`. Four elements sit before `9`:

```text
max zone size = 4 + (1 - 1) = 4     →  3 comparisons
```

With 2 comparisons the largest allowed zone has 3 elements (`2^2 - 1`). The largest candidate index that still fits is:

```text
candidate index + (1 - 1) <= 3
candidate index <= 3
```

So the group stops at `b3`. Inside the group the order is backwards: `b3`, then `b2`.

Two formulas describe that limit. With `k` binary comparisons the largest allowed zone is:

```text
max zone size = 2^k - 1
```

Combined with the formula above, the next group boundary is:

```text
next index = 2^k - last handled index
```

```text
3  = 2^2 - 1
5  = 2^3 - 3
11 = 2^4 - 5
```

That formula does not pick a value out of the list. It picks **how far along the `b` indices** the group may go so that every insertion in the group stays inside the comparison budget.

`jacobsthalOrder` stores these boundaries in `jac`, starting from `1` and `3`. The next boundary is the same number, written with the two boundaries already known:

```text
jac = [1, 3]
3 + 2 * 1 = 5          and  2^3 - 3 = 5
jac = [1, 3, 5]
5 + 2 * 3 = 11         and  2^4 - 5 = 11
jac = [1, 3, 5, 11]
```

The boundaries count from 1 (`b1`, `b3`, `b5`). `pendingIds` counts from 0, so the code stores `index - 1`. For the five small values of the example, the returned positions are `0, 2, 1, 4, 3`:

```text
case 0   case 1   case 2   case 3   case 4
b1 = 5   b2 = 4   b3 = 1   b4 = 2   3 alone
```

Insertion order: `5`, then `1`, then `4`, then `3`, then `2`.

### 🔸 **Why the two containers do not take the same time (ex02)**

A `vector` is one contiguous block. Reaching element `i` is one offset from the start, and sliding the tail on an insertion stays in that block.

A `deque` is a sequence of fixed blocks. Reaching element `i` first finds the block. An insertion in the middle still moves elements, across those blocks.

The algorithm is the same. `fordJohnsonDeque` is a handwritten copy of `fordJohnsonVector`, because the subject advises against one generic function: the measured `insert` must be the `deque` insert. The clock starts before the copy (`vec = input`, or `deq.assign(...)`), not only around the sort. `assign` is used because a `deque` cannot be assigned from a `vector`.

---

## ⚙️ **Compiling and testing**

Each exercise is **independent** and has its own `Makefile`. Compile and run from the exercise directory.

### 🔧 Compiling

| Exercise | Binary | Command |
|----------|--------|---------|
| ex00 | `btc` | `cd ex00 && make` |
| ex01 | `RPN` | `cd ex01 && make` |
| ex02 | `PmergeMe` | `cd ex02 && make` |

```bash
make        # compile
make re     # full rebuild
make fclean # remove binary and object files
```

Compiler flags: `c++ -Wall -Wextra -Werror -std=c++98`

---

### ▶️ Use and execution

- #### ✅ Exercise 00 — `./btc`

  `data.csv` must be in the current directory.

  ```bash
  cd ex00 && make && ./btc input.txt
  ```

  The subject file prints:

  ```text
  2011-01-03 => 3 = 0.9
  2011-01-03 => 2 = 0.6
  2011-01-03 => 1 = 0.3
  2011-01-03 => 1.2 = 0.36
  2011-01-09 => 1 = 0.32
  Error: not a positive number.
  Error: bad input => 2001-42-42
  2012-01-11 => 1 = 7.1
  Error: too large a number.
  ```

  `2011-01-03` is not in the database. The earlier rate is `2011-01-01` at `0.3`. `2011-01-09` uses `2011-01-07` at `0.32`. `2012-01-11` is an exact match at `7.1`.

  `./btc` with no file, or with a missing file, prints `Error: could not open file.`

- #### ✅ Exercise 01 — `./RPN`

  The expression must be one quoted argument. Without quotes the shell splits on spaces, and `*` expands to filenames.

  ```bash
  cd ex01 && make
  ./RPN "8 9 * 9 - 9 - 9 - 4 - 1 +"
  ./RPN "7 7 * 7 -"
  ./RPN "1 2 * 2 / 2 * 2 4 - +"
  ./RPN "(1 + 1)"
  ```

  Expected results: `42`, `42`, `0`, then `Error`.

  Also:

  ```bash
  ./RPN "9 8 * 4 * 4 / 2 + 9 - 8 - 8 - 1 - 6 -"    # 42
  ./RPN "1 2 * 2 / 2 + 5 * 6 - 1 3 * - 4 5 * * 8 /"  # 15
  ./RPN "8 9 *"    # 72, the result may exceed 9
  ./RPN "5 3 -"    # 2
  ./RPN "7 2 /"    # 3, integer division
  ```

- #### ✅ Exercise 02 — `./PmergeMe`

  ```bash
  cd ex02 && make && ./PmergeMe 3 5 9 7 4
  ```

  ```text
  Before: 3 5 9 7 4
  After: 3 4 5 7 9
  Time to process a range of 5 elements with std::vector : … us
  Time to process a range of 5 elements with std::deque : … us
  ```

  The two displayed times are not required to have a fixed value. They must be different enough to see that the containers do not cost the same.

  ```bash
  ./PmergeMe 9 2 8 1 7 4 6 5 3
  ./PmergeMe "-1" "2"          # Error
  ./PmergeMe `jot -r 3000 1 1000 | tr '\n' ' '`
  ```

  `jot -r 3000 1 1000` repeats numbers. A duplicate is still a positive integer, so it is sorted and kept.

---

<h1 align="center">
  <br>
  <img src="https://i.ibb.co/MkL14CvZ/logo-cpp-bonus.png" alt="logo C++" width="150">
  <br>
  cpp09
  <br>
</h1>

<h4 align="center"><em>Module C++ 09 — Conteneurs STL : une map, une pile, et deux tris.</em></h4>

<p align="center">
  <img src="https://img.shields.io/badge/exercices-3-orange">
  <img src="https://img.shields.io/badge/C%2B%2B-100%25-blue">
  <img src="https://img.shields.io/badge/Note-100%2F100-brightgreen">
  <img src="https://img.shields.io/badge/standard-C%2B%2B98-informational">
</p>

<p align="center">
  <a href="./subject.md">Sujet</a> |
  <a href="#-aperçu-du-projet">Aperçu</a> •
  <a href="#-architecture-et-organisation-du-code">Architecture</a> •
  <a href="#-fonctionnement">Fonctionnement</a> •
  <a href="#%EF%B8%8F-compilation-et-tests">Compilation</a>
</p>

---

## 📌 **Aperçu du projet**

**cpp09** est le dernier module C++ du tronc commun de 42. Il utilise les **conteneurs de la STL** sur trois petits programmes : un échange de bitcoins, une calculatrice en notation polonaise inversée, et le tri fusion-insertion de **Ford-Johnson**.

Chaque exercice doit utiliser au moins un conteneur. Un conteneur déjà utilisé ne peut plus servir dans les exercices suivants. L'exercice 02 en utilise deux. Tout le code respecte **C++98**.

Les trois programmes sont indépendants. Ils ne partagent pas de classes.

### 1. Exercice 00 — Bitcoin Exchange ✅

`btc` charge `data.csv` (date, taux) dans une **`std::map`**, puis évalue chaque ligne d'un second fichier :

* Une ligne d'entrée a la forme `date | value`
* Une date valide s'écrit `Année-Mois-Jour`. Une valeur valide est un flottant ou un entier positif, de **0 à 1000** inclus
* Si la date n'est pas dans la base, le programme prend la date **antérieure la plus proche**, jamais une date ultérieure
* Une mauvaise ligne affiche une erreur, et le reste du fichier est quand même lu
* `data.csv` doit être à côté du binaire. La base est chargée dans le constructeur

### 2. Exercice 01 — Reverse Polish Notation ✅

`RPN` évalue une expression passée en un seul argument, avec une **`std::stack`** stockée dans une **`std::list`** :

* Les jetons sont séparés par des espaces. Les opérateurs sont `+`, `-`, `/` et `*`
* Les nombres en entrée sont des chiffres seuls (`0`–`9`). Le résultat du calcul peut dépasser 9
* Les parenthèses et les nombres à virgule sont refusés
* Le résultat va sur la sortie standard. `Error` va sur la sortie d'erreur
* La division est entière. Diviser par zéro est une erreur

### 3. Exercice 02 — PmergeMe ✅

`PmergeMe` trie une suite d'entiers positifs avec l'algorithme de **Ford-Johnson** (tri fusion-insertion), une fois dans un **`std::vector`** et une fois dans une **`std::deque`** :

* Les paires sont conservées : le plus grand est trié récursivement, le plus petit reste attaché à lui
* Les petits sont ensuite insérés par recherche binaire, dans l'ordre de **Jacobsthal**, uniquement devant leur partenaire
* Le programme affiche la suite avant et après, puis le temps pris par chaque conteneur
* Le chrono couvre la copie dans le conteneur **et** le tri
* Les doublons sont gardés. `0`, un négatif ou un argument non numérique affiche `Error` sur la sortie d'erreur

### Gestion des erreurs ⚠️

Ces programmes ne lèvent pas d'exceptions. Ils affichent un message et continuent, ou s'arrêtent quand l'entrée elle-même ne peut pas être ouverte.

| Exercice | Sortie | Quand |
|----------|--------|-------|
| ex00 | `Error: could not open file.` | Argument manquant, ou fichier d'entrée impossible à ouvrir |
| ex00 | `Error: could not open database.` | `data.csv` impossible à ouvrir |
| ex00 | `Error: bad input => …` | Date invalide, `\|` manquant, ou valeur qui n'est pas un nombre |
| ex00 | `Error: not a positive number.` | Valeur `< 0` |
| ex00 | `Error: too large a number.` | Valeur `> 1000` |
| ex01 | `Error` | Jeton invalide, pas assez d'opérandes, division par zéro, ou plus d'une valeur restante sur la pile |
| ex02 | `Error` | Aucun argument, jeton non numérique, `0`, nombre négatif, ou entier au-dessus de `INT_MAX` |

L'exercice 00 écrit tous ses messages sur la **sortie standard**. Les exercices 01 et 02 écrivent `Error` sur la **sortie d'erreur**.

---

## 📂 **Architecture et organisation du code**

```
cpp09/
├── ex00/   BitcoinExchange.hpp + BitcoinExchange.cpp + main.cpp — btc
│           data.csv (taux) + input.txt (exemple du sujet)
├── ex01/   RPN.hpp + RPN.cpp + main.cpp — RPN
├── ex02/   PmergeMe.hpp + PmergeMe.cpp + main.cpp — PmergeMe
├── subject.md
└── evaluation_sheet.md
```

#### 🔹 `BitcoinExchange` (ex00)

| Méthode | Description |
|---------|-------------|
| `BitcoinExchange()` | Charge `data.csv` dans `std::map<std::string, double>` |
| `processInput(filename) const` | Lit le fichier d'entrée ligne par ligne et affiche une valeur ou une erreur |
| `loadDatabase(filename)` | Privée. Remplit la map. `processInput` ne fait rien si ce chargement a échoué |

#### 🔹 `RPN` (ex01)

| Méthode | Description |
|---------|-------------|
| `calculate(expression) const` | Évalue l'expression. Affiche le résultat et renvoie `true`, ou affiche `Error` et renvoie `false` |

La pile est locale à `calculate`. La classe ne stocke rien. La forme canonique est vide pour cette raison.

#### 🔹 `PmergeMe` (ex02)

| Méthode | Description |
|---------|-------------|
| `run(argc, argv) const` | Lit les arguments, les trie dans les deux conteneurs, affiche la suite et les deux temps |
| `sortVector(data)` | Privée. Ford-Johnson sur un `std::vector` |
| `sortDeque(data)` | Privée. Le même algorithme, réécrit sur une `std::deque` |

`jacobsthalOrder` est une fonction libre. Elle construit seulement l'ordre d'insertion. Elle ne trie pas les nombres.

---

## 🏛 **Fonctionnement**

### 🔸 **Un conteneur par exercice**

Le module interdit de réutiliser un conteneur :

| Exercice | Conteneur | Pourquoi celui-là |
|----------|-----------|-------------------|
| ex00 | `std::map` | Une date est une clé, un taux est une valeur, et les clés restent triées |
| ex01 | `std::stack` stockée dans une `std::list` | Chaque opérateur a besoin des deux derniers nombres. `vector` et `deque` restent libres pour l'ex02 |
| ex02 | `std::vector` et `std::deque` | Les deux donnent un accès direct et peuvent insérer au milieu. Le temps diffère parce que leur mémoire n'est pas organisée de la même façon |

Une `map` garde ses clés triées toute seule. Avec des dates écrites `YYYY-MM-DD`, l'ordre alphabétique est l'ordre chronologique. `lower_bound` trouve la première date supérieure ou égale à celle demandée. Si ce n'est pas la date exacte, reculer d'un élément donne la date antérieure la plus proche.

Une `stack` est un adaptateur. `push`, `top` et `pop` ne voient que le dessus. `pop` ne renvoie rien : la valeur se lit avec `top`, puis on la retire. Le conteneur de stockage est ici une `list`, pour laisser `vector` et `deque` à l'exercice 02. En C++98, les deux chevrons fermants de `std::stack<long, std::list<long> >` doivent être séparés par un espace.

### 🔸 **Notation polonaise inversée (ex01)**

L'opérateur arrive après ses opérandes. `8 9 *` veut dire `8 * 9`. La première valeur dépilée est l'opérande de **droite** : `5 3 -` vaut `5 - 3`, donc `2`.

```text
8  9  *   →  pile : 72
72 9  -   →  pile : 63
```

À la fin, il doit rester exactement un nombre. `"7"` est valide et affiche `7`. `"1 2 3 +"` laisse deux nombres : c'est une erreur.

### 🔸 **Ford-Johnson (ex02)**

Ford-Johnson, aussi appelé tri fusion-insertion, tient en trois gestes :

1. Faire des paires et **les conserver**.
2. Trier le plus grand de chaque paire avec le même algorithme.
3. Réinsérer les plus petits par recherche binaire, dans l'ordre de Jacobsthal, uniquement devant leur partenaire.

On part de la liste non triée :

```text
[9, 2, 8, 1, 7, 4, 6, 5, 3]
```

#### Les paires

On groupe les chiffres, et on retient quel petit appartient à quel grand. Le dernier nombre n'a pas de partenaire. Il sera inséré plus tard, sur toute la chaîne.

```text
[9, 2]   [8, 1]   [7, 4]   [6, 5]   [3]
```

On compare les deux chiffres de chaque paire. Le plus grand reste au-dessus, le plus petit reste en dessous :

```text
9    8    7    6    (3)
2    1    4    5
```

#### Trier les plus grands

On compare ensuite les plus grands entre eux. Les paires ne sont pas défaites : quand un grand se déplace, son petit partenaire se déplace avec lui.

On compare les voisins :

```text
9 > 8          7 > 6
```

On réordonne ces paires :

```text
8    9    6    7    (3)
1    2    5    4
```

On refait des paires. `8 > 6`, et on sait déjà que `9 > 8` :

```text
6    8    9    7    (3)
5    1    2    4
```

`7 > 6` est déjà connu, donc `7` ne peut plus être comparé qu'à `8` ou à `9`. Après ces comparaisons, les grands sont triés et les paires sont toujours là :

```text
6    7    8    9
5    4    1    2
```

Dans le code, toute cette étape est l'appel récursif sur `winners`. `loserOf[grand]` contient toujours le petit, parce qu'il est indexé par l'identité du grand, pas par sa position.

On les nomme :

```text
a1  a2  a3  a4          b1  b2  b3  b4     b5
6   7   8   9           5   4   1   2      3
```

`b5` est le `3` sans paire.

#### Insérer les plus petits

`b1` est inséré en premier. `6` est le plus petit des grands, et `5` est plus petit que `6`, donc `5` est plus petit que tous les grands déjà dans la chaîne :

```text
[5, 6, 7, 8, 9]
```

Il reste quatre candidats : `4`, `1`, `2` et `3`.

Jacobsthal ne regarde pas lequel de ces nombres est le plus petit. Il regarde **où se trouve son partenaire**, et combien d'éléments les insertions précédentes pourraient ajouter devant ce partenaire. Chaque insertion peut agrandir la zone de recherche de ceux qui viennent après. Le but est d'utiliser une zone tant qu'elle est encore petite.

La zone de recherche d'un petit, ce sont tous les éléments déjà placés **devant son grand partenaire**.

La recherche binaire ouvre cette zone au milieu et élimine la moitié où le nombre ne peut pas être, comme quand on ouvre un dictionnaire.

Si on insère `4` (paire de `7`) en premier, il doit se placer avant `7`. La zone est `[5, 6]` :

```text
[5, 6, 7, 8, 9]
    ↑
4 < 6    → on supprime la partie de droite
4 < 5    → il reste une comparaison

[4, 5, 6, 7, 8, 9]
```

Ça fait **2** comparaisons.

Ensuite `1` (paire de `8`) cherche dans `[4, 5, 6, 7]` :

```text
[4, 5, 6, 7]
       ↑
1 < 6
1 < 5
1 < 4
```

Ça fait **3** comparaisons. Total : **5**.

Si on commence par `1`, sa zone s'arrête avant `8`, donc `[5, 6, 7]` :

```text
[5, 6, 7]
    ↑
1 < 6
1 < 5

[1, 5, 6, 7, 8, 9]
```

Ça fait **2** comparaisons.

Ensuite `4` (paire de `7`) ne cherche que jusqu'à `7` :

```text
[1, 5, 6, 7, 8, 9]
    ↑
4 < 5
4 > 1

[1, 4, 5, 6, 7, 8, 9]
```

Encore **2** comparaisons. Total : **4** au lieu de **5**.

On insère donc `b3` avant `b2`. Insérer `b2` d'abord peut ajouter un élément devant le partenaire de `b3`, et sa zone passe de 3 éléments à 4. Le partenaire de `b2` est plus tôt dans la chaîne : si `b3` passe en premier, cette zone ne peut pas dépasser 3 éléments.

```text
b2 puis b3  →  2 + 3 = 5 comparaisons
b3 puis b2  →  2 + 2 = 4 comparaisons
```

#### Combien de comparaisons coûte une zone

```text
comparaisons max = ceil(log2(taille de la zone + 1))
```

`ceil` arrondit à l'entier supérieur. `log2(x)` est la puissance à laquelle il faut élever 2 pour obtenir `x` :

```text
log2(2) = 1     car 2^1 = 2
log2(4) = 2     car 2^2 = 4
log2(8) = 3     car 2^3 = 8
```

La taille de la zone est le nombre d'éléments déjà présents avant le partenaire. Pour `1`, qui fait paire avec `8`, dans `[5, 6, 7, 8, 9]` :

```text
taille de la zone = 3     (5, 6, 7)
ceil(log2(3 + 1)) = ceil(log2(4)) = 2
```

Les mêmes limites, sans recalculer le logarithme à chaque fois :

| Taille de la zone | Comparaisons max |
|-------------------|------------------|
| 1 | 1 |
| 2 à 3 | 2 |
| 4 à 7 | 3 |
| 8 à 15 | 4 |
| 16 à 31 | 5 |

#### Choisir le prochain groupe

Trois quantités décident du prochain candidat :

* **Indice candidat** — le petit qu'on envisage d'insérer (`b2`, `b3`, `b4`…)
* **Dernier indice traité** — tous les petits jusqu'à cet indice sont déjà insérés
* **Taille de zone max** — la plus grande recherche que ce candidat pourrait avoir à faire

```text
taille de zone max = indice candidat + dernier indice traité - 1
```

Cette taille ne dépend pas de l'emplacement final du nombre. C'est le pire cas : le partenaire, plus chaque petit déjà inséré qui a pu se placer devant lui.

`b1` est déjà placé, donc le dernier indice traité vaut `1`.

Pour `b3 = 1`, partenaire `a3 = 8`. Il y a 3 éléments avant `8` :

```text
taille de zone max = 3 + (1 - 1) = 3     →  2 comparaisons
```

Pour `b4 = 2`, partenaire `a4 = 9`. Il y a 4 éléments avant `9` :

```text
taille de zone max = 4 + (1 - 1) = 4     →  3 comparaisons
```

Avec 2 comparaisons, la plus grande zone autorisée contient 3 éléments (`2^2 - 1`). Le plus grand indice candidat qui respecte encore cette limite est :

```text
indice candidat + (1 - 1) <= 3
indice candidat <= 3
```

Le groupe s'arrête donc à `b3`. À l'intérieur du groupe, on repart de la fin : `b3`, puis `b2`.

Deux formules décrivent cette limite. Avec `k` comparaisons binaires, la plus grande zone autorisée est :

```text
taille de zone max = 2^k - 1
```

En la combinant avec la formule précédente, la borne du prochain groupe est :

```text
prochain indice = 2^k - dernier indice traité
```

```text
3  = 2^2 - 1
5  = 2^3 - 3
11 = 2^4 - 5
```

Cette formule ne choisit pas une valeur dans la liste. Elle choisit **jusqu'à quel indice `b`** le groupe peut aller pour que chaque insertion du groupe reste dans le nombre de comparaisons visé.

`jacobsthalOrder` range ces bornes dans `jac`, en partant de `1` et `3`. La borne suivante est le même nombre, écrit avec les deux bornes déjà connues :

```text
jac = [1, 3]
3 + 2 * 1 = 5          et  2^3 - 3 = 5
jac = [1, 3, 5]
5 + 2 * 3 = 11         et  2^4 - 5 = 11
jac = [1, 3, 5, 11]
```

Les bornes comptent à partir de 1 (`b1`, `b3`, `b5`). `pendingIds` compte à partir de 0, donc le code stocke `indice - 1`. Pour les cinq petits de l'exemple, les positions renvoyées sont `0, 2, 1, 4, 3` :

```text
case 0   case 1   case 2   case 3   case 4
b1 = 5   b2 = 4   b3 = 1   b4 = 2   3 seul
```

Ordre d'insertion : `5`, puis `1`, puis `4`, puis `3`, puis `2`.

### 🔸 **Pourquoi les deux conteneurs ne prennent pas le même temps (ex02)**

Un `vector` est un seul bloc. Atteindre l'élément `i` est un décalage depuis le début, et décaler la fin lors d'une insertion reste dans ce bloc.

Une `deque` est une suite de blocs fixes. Atteindre l'élément `i` commence par trouver le bon bloc. Une insertion au milieu déplace aussi des éléments, à travers ces blocs.

L'algorithme est le même. `fordJohnsonDeque` est une copie écrite à la main de `fordJohnsonVector`, parce que le sujet conseille d'éviter une seule fonction générique : l'`insert` mesuré doit être celui de la `deque`. Le chrono démarre avant la copie (`vec = input`, ou `deq.assign(...)`), pas seulement autour du tri. `assign` est utilisé parce qu'une `deque` ne peut pas recevoir un `vector` avec `=`.

---

## ⚙️ **Compilation et tests**

Chaque exercice est **indépendant** et a son propre `Makefile`. On compile et on lance depuis le dossier de l'exercice.

### 🔧 Compilation

| Exercice | Binaire | Commande |
|----------|---------|----------|
| ex00 | `btc` | `cd ex00 && make` |
| ex01 | `RPN` | `cd ex01 && make` |
| ex02 | `PmergeMe` | `cd ex02 && make` |

```bash
make        # compile
make re     # recompilation complète
make fclean # supprime le binaire et les fichiers objet
```

Options du compilateur : `c++ -Wall -Wextra -Werror -std=c++98`

---

### ▶️ Utilisation

- #### ✅ Exercice 00 — `./btc`

  `data.csv` doit être dans le dossier courant.

  ```bash
  cd ex00 && make && ./btc input.txt
  ```

  Le fichier du sujet affiche :

  ```text
  2011-01-03 => 3 = 0.9
  2011-01-03 => 2 = 0.6
  2011-01-03 => 1 = 0.3
  2011-01-03 => 1.2 = 0.36
  2011-01-09 => 1 = 0.32
  Error: not a positive number.
  Error: bad input => 2001-42-42
  2012-01-11 => 1 = 7.1
  Error: too large a number.
  ```

  `2011-01-03` n'est pas dans la base. Le taux antérieur est celui du `2011-01-01`, à `0.3`. `2011-01-09` utilise le `2011-01-07`, à `0.32`. `2012-01-11` est une date exacte, à `7.1`.

  `./btc` sans fichier, ou avec un fichier absent, affiche `Error: could not open file.`

- #### ✅ Exercice 01 — `./RPN`

  L'expression doit être un seul argument, entre guillemets. Sans guillemets, le shell coupe sur les espaces, et `*` devient la liste des fichiers du dossier.

  ```bash
  cd ex01 && make
  ./RPN "8 9 * 9 - 9 - 9 - 4 - 1 +"
  ./RPN "7 7 * 7 -"
  ./RPN "1 2 * 2 / 2 * 2 4 - +"
  ./RPN "(1 + 1)"
  ```

  Résultats attendus : `42`, `42`, `0`, puis `Error`.

  Aussi :

  ```bash
  ./RPN "9 8 * 4 * 4 / 2 + 9 - 8 - 8 - 1 - 6 -"    # 42
  ./RPN "1 2 * 2 / 2 + 5 * 6 - 1 3 * - 4 5 * * 8 /"  # 15
  ./RPN "8 9 *"    # 72, le résultat peut dépasser 9
  ./RPN "5 3 -"    # 2
  ./RPN "7 2 /"    # 3, division entière
  ```

- #### ✅ Exercice 02 — `./PmergeMe`

  ```bash
  cd ex02 && make && ./PmergeMe 3 5 9 7 4
  ```

  ```text
  Before: 3 5 9 7 4
  After: 3 4 5 7 9
  Time to process a range of 5 elements with std::vector : … us
  Time to process a range of 5 elements with std::deque : … us
  ```

  Les deux temps affichés n'ont pas une valeur fixe. Ils doivent être assez différents pour voir que les conteneurs ne coûtent pas la même chose.

  ```bash
  ./PmergeMe 9 2 8 1 7 4 6 5 3
  ./PmergeMe "-1" "2"          # Error
  ./PmergeMe `jot -r 3000 1 1000 | tr '\n' ' '`
  ```

  `jot -r 3000 1 1000` répète des nombres. Un doublon reste un entier positif : il est trié et conservé.
