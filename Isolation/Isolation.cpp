#include <stdio.h> // print
#include <stdlib.h> //abs
#include <ctime>  // do randoma
#include <vector>  // do przechowywania ruchow
#include <chrono> // do czasu funkcji
#include <map>	// do sortowania

using namespace std::chrono;

// ustawienia planszy
const int rows = 6;
const int columns = 8;
char board[rows][columns];
int player1Row = 2, player1Column = 0;
int player2Row = 3, player2Column = 7;
// przypisanie znakow
const char field = '#';
const char emptyField = ' ';
const char player1 = '1';
const char player2 = '2';
// gameplay
bool player1Turn = true;
bool movePhase = true;
bool isGameOver = false;
int possibleRows[8];
int possibleColumns[8];
int numOfPossibleMoves = 0;
const int turnsToLunchPNS = 8;
int maxResources = 999;
int resources = 0;
int numOfSimulations = 10;
// statystyki
int numOfGames = 0;
int maxGames = 0;
int player1Wins = 0;
int numOfAlgorithms = 0;
int numOfAlgorithmsOneGame = 0;
microseconds durationOfAlgorithms{0};
// algorytmy
int maxDepth = 3;
const int MINIMAX = 1;
const int NEGAMAX = 2;
const int ALPHABETA = 3;
const int ALPHABETA_SORT = 4;
const int PNS = 5;
const int MCS = 6;
int algorithm = -99;

struct Move
{
	int row;
	int column;
};

struct GameState
{
	int player1Row;
	int player1Column;
	int player2Row;
	int player2Column;
	Move bestMove;
};

GameState gameState{ player1Row, player1Column, player2Row, player2Column };

enum Type
{
	AND,
	OR
};

enum State
{
	UNKNOWN,
	WIN,
	LOSE
};

struct Node 
{
	Move move;
	Node* parent;
	std::vector<Node* > children;
	int proof = 0;
	int disproof = 0;
	bool isExpanded = false;
	State state;
	Type type; 
};

/********************* GRA NIE ZMIANIAC***********************************/

void initBoard()
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			board[i][j] = field;
		}
	}

	board[player1Row][player1Column] = player1;
	board[player2Row][player2Column] = player2;
}

void printBoard()
{
	{
		printf("\n  0 1 2 3 4 5 6 7\n");
		for (int i = 0; i < rows; i++)
		{
			printf("%i ", i);
			for (int j = 0; j < columns; j++)
			{
				printf("%c ", board[i][j]);
			}
			printf("\n");
		}
		printf("\n");
	}
}

void printMsg()
{
	printf("Gracz %s... %s\n", player1Turn ? "1" : "2", movePhase ? "poruszanie" : "niszczenie");
}

bool isInBoard(int row, int column)
{
	return row >= 0 && row < rows&& column >= 0 && column < columns;
}

bool isValidField(int row, int column)
{
	return board[row][column] == field;
}

void changeState(bool& state)
{
	state = !state;
}

// true gdy znaleziono mozliwe pole
bool findFieldsAround(int playerRow, int playerColumn)
{
	numOfPossibleMoves = 0;

	for (int i = playerRow - 1; i < playerRow + 2; i++)
	{
		for (int j = playerColumn - 1; j < playerColumn + 2; j++)
		{
			if (i == playerRow && j == playerColumn)
			{
				continue;
			}
			else if (isInBoard(i, j) && isValidField(i, j))
			{
				possibleRows[numOfPossibleMoves] = i;
				possibleColumns[numOfPossibleMoves] = j;
				numOfPossibleMoves++;
			}
		}
	}

	return numOfPossibleMoves > 0;
}

void move(int newRow, int newColumn)
{
	if (player1Turn)
	{
		board[player1Row][player1Column] = field;
		player1Row = newRow;
		player1Column = newColumn;
		board[player1Row][player1Column] = player1;
	}
	else
	{
		board[player2Row][player2Column] = field;
		player2Row = newRow;
		player2Column = newColumn;
		board[player2Row][player2Column] = player2;
	}

	printBoard();
}
// playerRow, playerColumn OUT parameters
void GetPlayerPos(int& playerRow, int& playerColumn, bool player)
{
	if (player)
	{
		playerRow = player1Row;
		playerColumn = player1Column;
	}
	else
	{
		playerRow = player2Row;
		playerColumn = player2Column;
	}
}

bool isMovePossible()
{
	int playerRow, playerColumn;
	GetPlayerPos(playerRow, playerColumn, player1Turn);

	for (int i = playerRow - 1; i < playerRow + 2; i++)
	{
		for (int j = playerColumn - 1; j < playerColumn + 2; j++)
		{
			if (i == playerRow && j == playerColumn)
			{
				continue;
			}
			else if (isInBoard(i, j) && isValidField(i, j))
			{
				return true;
			}
		}
	}
	return false;
}

bool canDestroy(int row, int column)
{
	return isInBoard(row, column) && isValidField(row, column);
}

void destroy(int row, int column)
{
	board[row][column] = emptyField;
	printBoard();
}

void setRandomField(int& row, int& column)
{
	srand((int)time(0));
	int randomIndex = rand() % numOfPossibleMoves;
	row = possibleRows[randomIndex];
	column = possibleColumns[randomIndex];
}
/********************* GRA NIE ZMIANIAC***********************************/

/********************* ALGORYTMY ***********************************/

Move getOldMove(bool player)
{
	return player ? Move{ player1Row, player1Column } : Move{ player2Row, player2Column };
}

void movePlayer(Move move, bool player)
{
	if (player)
	{
		board[player1Row][player1Column] = field;
		player1Row = move.row;
		player1Column = move.column;
		board[player1Row][player1Column] = player1;
	}
	else
	{
		board[player2Row][player2Column] = field;
		player2Row = move.row;
		player2Column = move.column;
		board[player2Row][player2Column] = player2;
	}

	//printBoard();
}

std::vector<Move> getAllMoves(bool maximizingPlayer)
{
	std::vector<Move> Moves;
	int playerRow, playerColumn;
	GetPlayerPos(playerRow, playerColumn, maximizingPlayer);

	for (int i = playerRow - 1; i < playerRow + 2; i++)
	{
		for (int j = playerColumn - 1; j < playerColumn + 2; j++)
		{
			// nie bierzemy pod uwage ruchow w tym samym miejscu lub gdy jest jeden ruch od poczatkowego miejsca i wybierzemy miejsce na ktorym stoimy
			if ((i == playerRow && j == playerColumn))
			{
				continue;
			}
			else if (isInBoard(i, j) && isValidField(i, j))
			{
				Move possibleMove{ i, j };
				Moves.push_back(possibleMove);
			}
		}
	}

	return Moves;
}

bool IsInCorner(int row, int column)
{			//lewo gora           // prawo gora				// lewo dol					// prawo dol
	return (row < 2 && column < 2) || (row < 2 && column > 5) || (row > 3 && column < 2) || (row > 3 && column > 5);
}

bool IsInCenter(int row, int column)
{
	return ((row >= 2 && row <= 3) && (column >= 2 && column <= 5));
}

int getValue(bool maximizingPlayer)
{
	int playerRow, playerColumn, value = 0;
	GetPlayerPos(playerRow, playerColumn, maximizingPlayer);

	for (int i = playerRow - 1; i < playerRow + 2; i++)
	{
		for (int j = playerColumn - 1; j < playerColumn + 2; j++)
		{
			if (i == playerRow && j == playerColumn)
			{
				continue;
			}
			else if (isInBoard(i, j) && isValidField(i, j))
			{
				value++;
			}
		}
	}

	if (IsInCenter(playerRow, playerColumn))
	{
		return value + 100;
	}
	else if (IsInCorner(playerRow, playerColumn))
	{
		return value;
	}
	else
	{
		return value + 10;
	}
}

int evaluate(bool maximizingPlayer)
{
	return getValue(maximizingPlayer);
}

int alphabetaWithSorting(GameState& gameState, int depth, bool maximizingPlayer, int alpha, int beta)
{
	int a = alpha;
	int b = beta;

	std::vector<Move> Moves = getAllMoves(maximizingPlayer);
	int numOfMoves = Moves.size();

	if (numOfMoves > 1) // > 1 bo jak zostaje 1 ruch to jest on niszczony i w efekcie jest 0 ruchow
	{
		if (depth > 0)
		{
			std::map<int, Move> moveValues;
			for (auto move : Moves)
			{
				Move oldMove{ getOldMove(maximizingPlayer) };
				movePlayer(move, maximizingPlayer);
				int value = evaluate(maximizingPlayer);
				movePlayer(oldMove, maximizingPlayer);

				// mapa sortuje sie sama w kolejnosci rosnacej
				moveValues.insert(std::pair<int, Move>(value, move));
			}

			Moves.clear();

			if (maximizingPlayer)
			{
				for (std::map<int, Move>::reverse_iterator it = moveValues.rbegin(); it != moveValues.rend(); ++it)
				{
					Moves.push_back(it->second);
				}

				int bestScore = INT_MIN;
				for (auto move : Moves)
				{
					Move oldMove{ getOldMove(maximizingPlayer) };
					movePlayer(move, maximizingPlayer);
					int newScore = alphabetaWithSorting(gameState, depth - 1, false, a, b);
					movePlayer(oldMove, maximizingPlayer);

					if (newScore > bestScore)
					{
						if (maxDepth - depth <= 1)
						{
							gameState.bestMove = move;
						}
						bestScore = newScore;
					}

					if (newScore >= b) 
					{
						break;
					}

					if (newScore > a)
					{
						a = newScore;
					}

				}
				return bestScore;
			}
			else
			{
				for (std::map<int, Move>::iterator it = moveValues.begin(); it != moveValues.end(); ++it)
				{
					Moves.push_back(it->second);
				}

				int bestScore = INT_MAX;
				for (auto move : Moves)
				{
					Move oldMove{ getOldMove(maximizingPlayer) };
					movePlayer(move, maximizingPlayer);
					int newScore = alphabetaWithSorting(gameState, depth - 1, true, a, b);
					movePlayer(oldMove, maximizingPlayer);

					if (newScore < bestScore)
					{
						bestScore = newScore;
					}

					if (newScore <= a)
					{
						break;
					}

					if (newScore < b)
					{
						b = newScore;
					}
				}
				return bestScore;
			}
		}
		else // depth == 0
		{
			if (maxDepth <= 1)
			{
				std::map<int, Move> moveValues;
				for (auto move : Moves)
				{
					Move oldMove{ getOldMove(maximizingPlayer) };
					movePlayer(move, maximizingPlayer);
					int value = evaluate(maximizingPlayer);
					movePlayer(oldMove, maximizingPlayer);

					// mapa sortuje sie sama w kolejnosci rosnacej
					moveValues.insert(std::pair<int, Move>(value, move));
				}

				Moves.clear();

				for (std::map<int, Move>::reverse_iterator it = moveValues.rbegin(); it != moveValues.rend(); ++it)
				{
					Moves.push_back(it->second);
				}

				int bestScore = INT_MIN;
				for (auto move : Moves)
				{
					Move oldMove{ getOldMove(maximizingPlayer) };
					movePlayer(move, maximizingPlayer);
					int newScore = evaluate(maximizingPlayer);
					movePlayer(oldMove, maximizingPlayer);

					if (newScore > bestScore)
					{
						gameState.bestMove = move;
						bestScore = newScore;
					}

					if (newScore >= b)
					{
						break;
					}

					if (newScore > a)
					{
						a = newScore;
					}
				}
				if (maxDepth == 0)
				{
					return bestScore;
				}
			}

			return evaluate(maximizingPlayer);
		}
	}
	else // end game
	{
		return maximizingPlayer ? INT_MIN : INT_MAX;
	}
}

int alphabeta(GameState& gameState, int depth, bool maximizingPlayer, int alpha, int beta)
{
	int a = alpha;
	int b = beta;

	std::vector<Move> Moves = getAllMoves(maximizingPlayer);
	int numOfMoves = Moves.size();

	if (numOfMoves > 1) // > 1 bo jak zostaje 1 ruch to jest on niszczony i w efekcie jest 0 ruchow
	{
		if (depth > 0)
		{
			if (maximizingPlayer)
			{
				int bestScore = INT_MIN;
				for (auto move : Moves)
				{
					Move oldMove{ getOldMove(maximizingPlayer) };
					movePlayer(move, maximizingPlayer);
					int newScore = alphabeta(gameState, depth - 1, false, a, b);
					movePlayer(oldMove, maximizingPlayer);

					if (newScore > bestScore)
					{
						if (maxDepth - depth <= 1)
						{
							gameState.bestMove = move;
						}
						bestScore = newScore;
					}

					if (newScore >= b)
					{
						break;
					}

					if (newScore > a)
					{
						a = newScore;
					}

				}
				return bestScore;
			}
			else
			{
				int bestScore = INT_MAX;
				for (auto move : Moves)
				{
					Move oldMove{ getOldMove(maximizingPlayer) };
					movePlayer(move, maximizingPlayer);
					int newScore = alphabeta(gameState, depth - 1, true, a, b);
					movePlayer(oldMove, maximizingPlayer);

					if (newScore < bestScore)
					{
						bestScore = newScore;
					}

					if (newScore <= a)
					{
						break;
					}

					if (newScore < b)
					{
						b = newScore;
					}
				}
				return bestScore;
			}
		}
		else // depth == 0
		{
			if (maxDepth <= 1)
			{
				int bestScore = INT_MIN;
				for (auto move : Moves)
				{
					Move oldMove{ getOldMove(maximizingPlayer) };
					movePlayer(move, maximizingPlayer);
					int newScore = evaluate(maximizingPlayer);
					movePlayer(oldMove, maximizingPlayer);

					if (newScore > bestScore)
					{
						gameState.bestMove = move;
						bestScore = newScore;
					}

					if (newScore >= b)
					{
						break;
					}

					if (newScore > a)
					{
						a = newScore;
					}
				}
				if (maxDepth == 0)
				{
					return bestScore;
				}
			}

			return evaluate(maximizingPlayer);
		}
	}
	else // end game
	{
		return maximizingPlayer ? INT_MIN : INT_MAX;
	}
}

int negamax(GameState& gameState, int depth, int sign)
{
	std::vector<Move> Moves = getAllMoves(sign == 1);
	int numOfMoves = Moves.size();

	if (numOfMoves > 1) // > 1 bo jak zostaje 1 ruch to jest on niszczony i w efekcie jest 0 ruchow
	{
		if (depth > 0)
		{
			int bestScore = INT_MIN;
			for (auto move : Moves)
			{
				Move oldMove{ getOldMove(sign == 1) };
				movePlayer(move, sign == 1);
				int newScore;
				sign == 1? newScore = -negamax(gameState, depth - 1, -1) : newScore = -negamax(gameState, depth - 1, 1);
				movePlayer(oldMove, sign == 1);
				
				if (newScore > bestScore)
				{
					if (maxDepth - depth <= 1 && sign == 1)
					{
						gameState.bestMove = move;
					}
					bestScore = newScore;
				}
			}
			return bestScore;
		}
		else // depth == 0
		{
			if (maxDepth <= 1)
			{
				int bestScore = INT_MIN;
				for (auto move : Moves)
				{
					Move oldMove{ getOldMove(sign == 1) };
					movePlayer(move, sign == 1);
					int newScore = evaluate(sign == 1);
					movePlayer(oldMove, sign == 1);

					if (newScore > bestScore)
					{
						gameState.bestMove = move;
						bestScore = newScore;
					}
				}
				if (maxDepth == 0)
				{
					return bestScore;
				}
			}

			return sign * evaluate(sign == 1);
		}
	}
	else // end game
	{
		return sign == 1 ? sign * INT_MIN : sign * INT_MAX;
	}
}

int minimax(GameState& gameState, int depth, bool maximizingPlayer)
{
	std::vector<Move> Moves = getAllMoves(maximizingPlayer);
	int numOfMoves = Moves.size();

	if (numOfMoves > 1) // > 1 bo jak zostaje 1 ruch to jest on niszczony i w efekcie jest 0 ruchow
	{
		if (depth > 0)
		{
			if (maximizingPlayer)
			{
				int bestScore = INT_MIN;
				for (auto move : Moves)
				{
					Move oldMove{ getOldMove(maximizingPlayer) };
					movePlayer(move, maximizingPlayer);
					int newScore = minimax(gameState, depth - 1, false);
					movePlayer(oldMove, maximizingPlayer);

					if (newScore > bestScore)
					{
						if (maxDepth - depth <= 1)
						{
							gameState.bestMove = move;
						}
						bestScore = newScore;
					}
				}
				return bestScore;
			}
			else
			{
				int bestScore = INT_MAX;
				for (auto move : Moves)
				{
					Move oldMove{ getOldMove(maximizingPlayer) };
					movePlayer(move, maximizingPlayer);
					int newScore = minimax(gameState, depth - 1, true);
					movePlayer(oldMove, maximizingPlayer);

					if (newScore < bestScore)
					{
						bestScore = newScore;
					}
				}
				return bestScore;
			}
		}
		else // depth == 0
		{
			if (maxDepth <= 1)
			{
				int bestScore = INT_MIN;
				for (auto move : Moves)
				{
					Move oldMove{ getOldMove(maximizingPlayer) };
					movePlayer(move, maximizingPlayer);
					int newScore = evaluate(maximizingPlayer);
					movePlayer(oldMove, maximizingPlayer);

					if (newScore > bestScore)
					{
						gameState.bestMove = move;
						bestScore = newScore;
					}
				}
				if (maxDepth == 0)
				{
					return bestScore;
				}
			}

			return evaluate(maximizingPlayer);
		}
	}
	else // end game
	{
		return maximizingPlayer ? INT_MIN : INT_MAX;
	}
}

bool resourcesAvailable()
{
	resources++;
	return resources < maxResources;
}

void resetResources()
{
	resources = 0;
}

void generateChildren(Node* node, bool maximizingPlayer)
{
	for (auto move : getAllMoves(maximizingPlayer))
	{
		Node* child = new Node();
		child->move = move;
		child->type = node->type == Type::AND ? Type::OR : Type::AND;
		child->parent = node;

		node->children.push_back(child);
	}
}

Node* selectBestMove(Node* root)
{
	if (root->children.size() == 0) 
	{
		return nullptr;
	}

	float value = 0;
	Node* bestNode = nullptr;

	for (Node* child : root->children)
	{
		float childValue = (float)child->disproof / (float)child->proof;
		if (childValue > value)
		{
			value = childValue;
			bestNode = child;
		}
	}

	return bestNode;
}

Node* selectMostProvingNode(Node* node)
{
	while (node->isExpanded)
	{
		int value = INT_MAX;
		Node* bestNode = nullptr;

		if (node->type == Type::AND)
		{
			for (Node* child : node->children)
				if (value > child->disproof)
				{
					bestNode = child;
					value = child->disproof;
				}
		}
		else
		{
			for (Node* child : node->children)
				if (value > child->proof)
				{
					bestNode = child;
					value = child->proof;
				}
		}

		node = bestNode;
	}

	return node;
}

void setProofAndDisproofNumbers(Node* node, bool player)
{
	if (node->isExpanded)
	{
		if (node->type == Type::AND)
		{
			node->proof = 0;
			node->disproof = INT_MAX;

			for (Node* child : node->children)
			{
				if (child->proof == INT_MAX)
				{
					node->proof = INT_MAX;
				}
				else if (node->proof != INT_MAX)
				{
					node->proof += child->proof;
				}
				node->disproof = std::min(node->disproof, child->disproof);
			}
		}
		else
		{
			node->proof = INT_MAX;
			node->disproof = 0;

			for (Node* child : node->children)
			{
				if (child->disproof == INT_MAX)
				{
					node->disproof = INT_MAX;
				}
				else if (node->disproof != INT_MAX)
				{
					node->disproof += child->disproof;
				}
				node->proof = std::min(node->proof, child->proof);
			}
		}
	}
	else
	{
		if (node->state == State::UNKNOWN) 
		{
			node->proof = 1;
			node->disproof = 1;
		}
		else if(node->state == State::WIN)
		{
			node->proof = 0;
			node->disproof = INT_MAX;
		}
		else if (node->state == State::LOSE) 
		{
			node->proof = INT_MAX;
			node->disproof = 0;
		}
	}
}

Node* updateAncestors(Node* node, Node* root, bool player)
{
	while (node != root)
	{
		int oldProof = node->proof;
		int oldDisproof = node->disproof;

		setProofAndDisproofNumbers(node, player);
		if (node->proof == oldProof && node->disproof == oldDisproof)
		{
			return node;
		}

		node = node->parent;
	}

	setProofAndDisproofNumbers(root, player);
	return root;
}

void evaluatePNS(Node* node, bool maximizingPlayer)
{
	std::vector<Move> Moves = getAllMoves(maximizingPlayer);
	int numOfMoves = Moves.size();

	if (numOfMoves > 1)
	{
		node->state = State::UNKNOWN;
	}
	else
	{
		maximizingPlayer ? node->state = State::LOSE : node->state = State::WIN;
	}
}


void expandNode(Node* node, bool maximizingPlayer)
{
	generateChildren(node, maximizingPlayer);

	for (Node* child : node->children)
	{
		evaluatePNS(child, maximizingPlayer);
		setProofAndDisproofNumbers(child, maximizingPlayer);

		if (node->type == Type::AND)
		{
			if (child->disproof == 0)
			{
				break;
			}
		}
		else
		{
			if (child->proof == 0)
			{
				break;
			}
		}
	}

	node->isExpanded = true;
}

Node* PNS_alg(Node* root, bool player)
{
	evaluatePNS(root, player);
	setProofAndDisproofNumbers(root, player);

	Node* current = root;
	while (root->proof != 0 && root->disproof != 0 && resourcesAvailable())
	{
		Node* mostProving = selectMostProvingNode(current);
		expandNode(mostProving, player);
		current = updateAncestors(mostProving, root, player);
	}

	Node* temp = selectBestMove(root);
	return temp;
}

void MonteCarloEvaluation(GameState& gameState, bool maximizingPlayer, int numOfSimulations)
{
	Move bestMove;
	float bestProbability = -1;

	std::vector<Move> Moves = getAllMoves(maximizingPlayer);
				
	for (auto move : Moves)
	{
		int r = 0;
		for (int i = 0; i < numOfSimulations; i++)
		{
			Move oldMove{ getOldMove(maximizingPlayer) };
			movePlayer(move, maximizingPlayer);
			std::vector<Move> Moves = getAllMoves(maximizingPlayer);
			movePlayer(oldMove, maximizingPlayer);
			
			float x = 0;
			while (Moves.size() > 1 && x < 10)
			{
				Move oldMove{ getOldMove(maximizingPlayer) };
				srand((int)time(0));
				int randomIndex = rand() % Moves.size();
				movePlayer(Moves[randomIndex], maximizingPlayer);

				if (getAllMoves(!maximizingPlayer).size() > 2)
				{
					r++;
				}

				Moves = getAllMoves(maximizingPlayer);
				movePlayer(oldMove, maximizingPlayer);

				x++;
			}
			x += 0.01;
			float probability = (float)(r/x) / numOfSimulations;
			//printf("probability %f\n", probability);
			x = 0;

			if (probability > bestProbability)
			{
				bestMove = move;
				bestProbability = probability;
			}
		}
	}

	gameState.bestMove = bestMove;
}

/********************* ALGORYTMY ***********************************/

/********************* GRA NIE ZMIANIAC***********************************/
void destroyRandomField(int& row, int& column)
{
	if (player1Turn)
	{
		if (!findFieldsAround(player2Row, player2Column))
		{
			findFieldsAround(player1Row, player1Column);
		}
	}
	else
	{
		if (!findFieldsAround(player1Row, player1Column))
		{
			findFieldsAround(player2Row, player2Column);
		}
	}
	setRandomField(row, column);
}

void takeField(int& row, int& column)
{
	printMsg();
	if (movePhase)
	{
		if (player1Turn)
		{
			int value = 0;
			gameState = { player1Row, player1Column, player2Row, player2Column, Move{player1Row, player1Column} };
			
			auto start = high_resolution_clock::now();

			if (algorithm == MINIMAX) 
			{
				maxDepth % 2 == 0 ? value = minimax(gameState, maxDepth, true) : value = minimax(gameState, maxDepth, false);
			}
			else if (algorithm == NEGAMAX) 
			{
				maxDepth % 2 == 0 ? value = negamax(gameState, maxDepth, 1) : value = negamax(gameState, maxDepth, -1);
			}
			else if (algorithm == ALPHABETA)
			{
				maxDepth % 2 == 0 ? value = alphabeta(gameState, maxDepth, true, INT_MIN, INT_MAX) : value = alphabeta(gameState, maxDepth, false, INT_MIN, INT_MAX);
			}
			else if (algorithm == ALPHABETA_SORT)
			{
				maxDepth % 2 == 0 ? value = alphabetaWithSorting(gameState, maxDepth, true, INT_MIN, INT_MAX) : value = alphabetaWithSorting(gameState, maxDepth, false, INT_MIN, INT_MAX);
			}
			else if (algorithm == PNS) 
			{
				if (numOfAlgorithmsOneGame >= turnsToLunchPNS) 
				{
					resetResources();
					Node* bestMove = new Node();
					maxDepth % 2 == 0 ? bestMove = PNS_alg(bestMove, true) : bestMove = PNS_alg(bestMove, false);
					if (bestMove != nullptr) 
					{
						gameState.bestMove.row = bestMove->move.row;
						gameState.bestMove.column = bestMove->move.column;
					}
				}
				else 
				{
					maxDepth % 2 == 0 ? value = alphabetaWithSorting(gameState, maxDepth, true, INT_MIN, INT_MAX) : value = alphabetaWithSorting(gameState, maxDepth, false, INT_MIN, INT_MAX);
				}
			}
			else if (algorithm == MCS)
			{
				MonteCarloEvaluation(gameState, true, numOfSimulations);
				//maxDepth % 2 == 0 ? value = alphabetaWithSorting(gameState, maxDepth, true, INT_MIN, INT_MAX) : value = alphabetaWithSorting(gameState, maxDepth, false, INT_MIN, INT_MAX);
			}

			auto stop = high_resolution_clock::now();
			auto duration = duration_cast<microseconds>(stop - start);
			numOfAlgorithms++;
			durationOfAlgorithms += duration;
			
			numOfAlgorithmsOneGame++;

			printf("Best value: %d best move row: %d column: %d: \n", value, gameState.bestMove.row, gameState.bestMove.column);
			row = gameState.bestMove.row;
			column = gameState.bestMove.column;

			if (gameState.player1Row == row && gameState.player1Column == column)
			{
				findFieldsAround(player1Row, player1Column);
				setRandomField(row, column);
			}

			movePlayer(Move{ gameState.player1Row, gameState.player1Column }, true);
			movePlayer(Move{ gameState.player2Row, gameState.player2Column }, false);
			player1Turn = true;
		}
		else
		{
			findFieldsAround(player2Row, player2Column);
			setRandomField(row, column);
		}
	}
	else
	{
		destroyRandomField(row, column);
	}

	printf("Ruch do Wiersz: %d Kolumna: %d\n", row, column);
}

void endGame()
{
	if (player1Turn)
	{
		printf("WYGRANA: %c\n", player2);
	}
	else
	{
		printf("WYGRANA: %c\n", player1);
		player1Wins++;
	}
	isGameOver = true;
}

void tryToMove()
{
	if (isMovePossible())
	{
		int row, column;
		takeField(row, column);
		move(row, column);
		changeState(movePhase);
	}
	else
	{
		endGame();
	}
}
void tryToDestroy()
{
	int row, column;
	takeField(row, column);
	if (canDestroy(row, column))
	{
		destroy(row, column);
		changeState(movePhase);
		changeState(player1Turn);
	}
}

void takeTurn()
{
	movePhase ? tryToMove() : tryToDestroy();
}

void setGameConfig()
{
	printf("Podaj liczbe gier: ");
	scanf_s("%d", &maxGames);

	printf("Wybierz algorytm:\n1 - MiniMax\n2 - NegaMax\n3 - AlphaBeta\n4 - AlphaBeta z sortowaniem\n5 - PNS\n6 - MCS\n");
	scanf_s("%d", &algorithm);

	if (algorithm != MCS) 
	{
		printf("Podaj glebokosc dla algorytmu: ");
		scanf_s("%d", &maxDepth);
	}
	else 
	{
		printf("Podaj ilosc symulacji: ");
		scanf_s("%d", &numOfSimulations);
	}
}

void resetGame()
{
	player1Row = 2, player1Column = 0;
	player2Row = 3, player2Column = 7;

	player1Turn = true;
	movePhase = true;
	isGameOver = false;
	numOfPossibleMoves = 0;
	numOfAlgorithmsOneGame = 0;

	if (numOfGames < maxGames)
	{
		initBoard();
		printBoard();
	}
}
/********************* GRA NIE ZMIANIAC***********************************/

int main()
{
	setGameConfig();
	initBoard();
	printBoard();

	while (numOfGames < maxGames) // petla do grania
	{
		while (!isGameOver)
		{
			takeTurn();
		}

		numOfGames += 1;
		resetGame();
	}

	printf("ALGORYTM WYGRAL %d NA %d GIER\n", player1Wins, numOfGames);
	printf("SREDNI CZAS ALGORYTMU %d mikrosekund\n", (int)durationOfAlgorithms.count() / numOfAlgorithms);

	return 0;
}


