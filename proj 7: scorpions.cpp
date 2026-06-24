// scorpions.cpp

// Portions that you are to complete are marked with a TODO: comment.
// We've provided some incorrect return statements (so indicated) just
// to allow this skeleton program to compile and run, albeit incorrectly.
// The first thing you probably want to do is implement the utterly trivial
// functions (marked TRIVIAL).  Then get Pit::display going.  That gives you
// more flexibility in the order you tackle the rest of the functionality.
// As you finish implementing each TODO: item, remove its TODO: comment; that
// makes it easier to find what you have left to do.

#include <iostream>
#include <string>
#include <random>
#include <utility>
#include <cstdlib>
using namespace std;

///////////////////////////////////////////////////////////////////////////
// Manifest constants
///////////////////////////////////////////////////////////////////////////

const int MAXROWS = 20;             // max number of rows in the pit
const int MAXCOLS = 40;             // max number of columns in the pit
const int MAXSCORPIONS = 140;       // max number of scorpions allowed

const int UP = 0;
const int DOWN = 1;
const int LEFT = 2;
const int RIGHT = 3;
const int NUMDIRS = 4;

///////////////////////////////////////////////////////////////////////////
// Type definitions
///////////////////////////////////////////////////////////////////////////

class Pit;  // This is needed to let the compiler know that Pit is a
// type name, since it's mentioned in the Scorpion declaration.

class Scorpion
{
public:
    // Constructor
    Scorpion(Pit* pp, int r, int c);

    // Accessors
    int  row() const;
    int  col() const;

    // Mutators
    void move();

private:
    Pit* m_pit;
    int  m_row;
    int  m_col;
};

class Player
{
public:
    // Constructor
    Player(Pit* pp, int r, int c);

    // Accessors
    int  row() const;
    int  col() const;
    int  age() const;
    bool isDead() const;

    // Mutators
    void   stand();
    void   move(int dir);
    void   setDead();

private:
    Pit* m_pit;
    int   m_row;
    int   m_col;
    int   m_age;
    bool  m_dead;
};

class Pit
{
public:
    // Constructor/destructor
    Pit(int nRows, int nCols);
    ~Pit();

    // Accessors
    int     rows() const;
    int     cols() const;
    Player* player() const;
    int     scorpionCount() const;
    int     numberOfScorpionsAt(int r, int c) const;
    void    display() const;

    // Mutators
    bool   addScorpion(int r, int c);
    bool   addPlayer(int r, int c);
    bool   destroyOneScorpion(int r, int c);
    bool   moveScorpions();

private:
    int        m_rows;
    int        m_cols;
    Player* m_player;
    Scorpion* m_scorpions[MAXSCORPIONS];
    int        m_nScorpions;

    // Helper functions
    bool isPosInBounds(int r, int c) const;
};

class Game
{
public:
    // Constructor/destructor
    Game(int rows, int cols, int nScorpions);
    ~Game();

    // Mutators
    void play();

private:
    Pit* m_pit;
};

///////////////////////////////////////////////////////////////////////////
//  Auxiliary function declarations
///////////////////////////////////////////////////////////////////////////

int randInt(int min, int max);
int decodeDirection(char dir);
bool directionToDeltas(int dir, int& rowDelta, int& colDelta);
void clearScreen();

///////////////////////////////////////////////////////////////////////////
//  Scorpion implementation
///////////////////////////////////////////////////////////////////////////

Scorpion::Scorpion(Pit* pp, int r, int c)
{
    if (pp == nullptr)
    {
        cout << "***** A scorpion must be created in some Pit!" << endl;
        exit(1);
    }
    if (r < 1 || r > pp->rows() || c < 1 || c > pp->cols())
    {
        cout << "***** Scorpion created with invalid coordinates (" << r << ","
            << c << ")!" << endl;
        exit(1);
    }
    m_pit = pp;
    m_row = r;
    m_col = c;
}

int Scorpion::row() const
{
    return m_row;
}

int Scorpion::col() const
{
    return m_col;
}

void Scorpion::move()
{
    // Attempt to move in a random direction; if can't move, don't move
    switch (randInt(0, NUMDIRS - 1))
    {
    case UP:
        if (m_row > 1)
            m_row--;
        break;
    case DOWN:
        if (m_row < m_pit->rows())
            m_row++;
        break;
    case LEFT:
        if (m_col > 1)
            m_col--;
        break;
    case RIGHT:
        if (m_col < m_pit->cols())
            m_col++;
        break;
    }
}

///////////////////////////////////////////////////////////////////////////
//  Player implementation
///////////////////////////////////////////////////////////////////////////

Player::Player(Pit* pp, int r, int c)
{
    if (pp == nullptr)
    {
        cout << "***** The player must be created in some Pit!" << endl;
        exit(1);
    }
    if (r < 1 || r > pp->rows() || c < 1 || c > pp->cols())
    {
        cout << "**** Player created with invalid coordinates (" << r
            << "," << c << ")!" << endl;
        exit(1);
    }
    m_pit = pp;
    m_row = r;
    m_col = c;
    m_age = 0;
    m_dead = false;
}

int Player::row() const
{
    return m_row;
}

int Player::col() const
{
    return m_col;
}

int Player::age() const
{
    return m_age;
}

void Player::stand()
{
    m_age++;
}

void Player::move(int dir)
{
    m_age++;
 
    int rowDelta, colDelta;
    if (!directionToDeltas(dir, rowDelta, colDelta))
        return;
 
    int adjR = m_row + rowDelta;   // the adjacent square in that direction
    int adjC = m_col + colDelta;
 
    // If the adjacent square is the wall (out of bounds), don't move.
    if (adjR < 1 || adjR > m_pit->rows() || adjC < 1 || adjC > m_pit->cols())
        return;
 
    // If the adjacent square is vacant, just step into it.
    if (m_pit->numberOfScorpionsAt(adjR, adjC) == 0)
    {
        m_row = adjR;
        m_col = adjC;
        return;
    }
 
    // The adjacent square has at least one scorpion: jump over it, killing one.
    int landR = adjR + rowDelta;   // the square on the far side
    int landC = adjC + colDelta;
 
    m_pit->destroyOneScorpion(adjR, adjC);
 
    if (landR >= 1 && landR <= m_pit->rows() &&
        landC >= 1 && landC <= m_pit->cols())
    {
        // There's a spot on the other side: land there.
        m_row = landR;
        m_col = landC;
    }
    else
    {
        // The far side is the wall: fall onto the dead scorpion's spot.
        m_row = adjR;
        m_col = adjC;
    }
 
    // If a (still-living) scorpion is on the landing spot, the player dies.
    if (m_pit->numberOfScorpionsAt(m_row, m_col) > 0)
        setDead();
}

bool Player::isDead() const
{
    return m_dead;
}

void Player::setDead()
{
    m_dead = true;
}

///////////////////////////////////////////////////////////////////////////
//  Pit implementation
///////////////////////////////////////////////////////////////////////////

Pit::Pit(int nRows, int nCols)
{
    if (nRows <= 0 || nCols <= 0 || nRows > MAXROWS || nCols > MAXCOLS)
    {
        cout << "***** Pit created with invalid size " << nRows << " by "
            << nCols << "!" << endl;
        exit(1);
    }
    m_rows = nRows;
    m_cols = nCols;
    m_player = nullptr;
    m_nScorpions = 0;
}

Pit::~Pit()
{
    delete m_player;
    for (int k = 0; k < m_nScorpions; k++)
    {
        delete m_scorpions[k];
    }
}

int Pit::rows() const
{
    return m_rows;
}

int Pit::cols() const
{
    return m_cols;
}

Player* Pit::player() const
{
    return m_player;
}

int Pit::scorpionCount() const
{
    return m_nScorpions;
}

int Pit::numberOfScorpionsAt(int r, int c) const
{
    
    int count = 0;
    for (int k = 0; k < m_nScorpions; k++)
    {
        if (m_scorpions[k]->row() == r && m_scorpions[k]->col() == c)
            count++;
    }
    return count; 
}

void Pit::display() const
{
    // Position (row,col) in the pit coordinate system is represented in
    // the array element grid[row-1][col-1]
    char grid[MAXROWS][MAXCOLS];
    int r, c;

    // Fill the grid with dots
    for (r = 0; r < rows(); r++)
    {
        for (c = 0; c < cols(); c++)
        {
            grid[r][c] = '.';
        }
    }

    // Indicate each scorpion's position
    for (r = 0; r < rows(); r++)
    {
        for (c = 0; c < cols(); c++)
        {
            int n = numberOfScorpionsAt(r + 1, c + 1);
            if (n == 1)
                grid[r][c] = 'S';
            else if (n >= 2 && n <= 8)
                grid[r][c] = '0' + n;
            else if (n >= 9)
                grid[r][c] = '9';
        }
    }

    // Indicate player's position
    if (m_player != nullptr)
    {
        char& gridChar = grid[m_player->row() - 1][m_player->col() - 1];
        if (m_player->isDead())
            gridChar = '*';
        else
            gridChar = '@';
    }

    // Draw the grid
    clearScreen();
    for (r = 0; r < rows(); r++)
    {
        for (c = 0; c < cols(); c++)
            cout << grid[r][c];
        cout << endl;
    }
    cout << endl;

    // Write scorpion and player info
    cout << "There are " << scorpionCount() << " scorpions remaining." << endl;
    if (m_player == nullptr)
        cout << "There is no player." << endl;
    else
    {
        if (m_player->age() > 0)
            cout << "The player has lasted " << m_player->age() << " steps." << endl;
        if (m_player->isDead())
            cout << "The player is dead." << endl;
    }
}

bool Pit::addScorpion(int r, int c)
{
    if (!isPosInBounds(r, c))
        return false;

    // Don't add a scorpion on a spot with a player
    if (m_player != nullptr && m_player->row() == r && m_player->col() == c)
        return false;

    if (scorpionCount() == MAXSCORPIONS)
        return false;

    m_scorpions[scorpionCount()] = new Scorpion(this, r, c);
    m_nScorpions++;

    return true;
}

bool Pit::addPlayer(int r, int c)
{
    if (!isPosInBounds(r, c))
        return false;

    // Don't add a player if one already exists
    if (m_player != nullptr)
        return false;

    // Don't add a player on a spot with a scorpion
    if (numberOfScorpionsAt(r, c) > 0)
        return false;

    // Dynamically allocate new Player and add it to the pit
    m_player = new Player(this, r, c);
    return true;
}

bool Pit::destroyOneScorpion(int r, int c)
{
    for (int k = 0; k < m_nScorpions; k++)
    {
        if (m_scorpions[k]->row() == r && m_scorpions[k]->col() == c)
        {
            delete m_scorpions[k];
            // Fill the hole with the last scorpion, then shrink the array.
            m_scorpions[k] = m_scorpions[m_nScorpions - 1];
            m_nScorpions--;
            return true;
        }
    }
    return false;
}

bool Pit::moveScorpions()
{
    for (int k = 0; k < m_nScorpions; k++)
    {
        m_scorpions[k]->move();
        if (m_scorpions[k]->row() == player()->row() && m_scorpions[k]->col() == player()->col())
            m_player->setDead();
    }

    // return true if the player is still alive, false otherwise
    return !m_player->isDead();
}

bool Pit::isPosInBounds(int r, int c) const
{
    return (r >= 1 && r <= m_rows && c >= 1 && c <= m_cols);
}

///////////////////////////////////////////////////////////////////////////
//  Game implementation
///////////////////////////////////////////////////////////////////////////

Game::Game(int rows, int cols, int nScorpions)
{
    if (nScorpions < 0)
    {
        cout << "***** Cannot create Game with negative number of scorpions!" << endl;
        exit(1);
    }
    if (nScorpions > MAXSCORPIONS)
    {
        cout << "***** Trying to create Game with " << nScorpions
            << " scorpions; only " << MAXSCORPIONS << " are allowed!" << endl;
        exit(1);
    }
    int nEmpty = rows * cols - nScorpions - 1;  // 1 for Player
    if (nEmpty < 0)
    {
        cout << "***** Game created with a " << rows << " by "
            << cols << " pit, which is too small to hold a player and "
            << nScorpions << " scorpions!" << endl;
        exit(1);
    }

    // Create pit
    m_pit = new Pit(rows, cols);

    // Add player
    int rPlayer = randInt(1, rows);
    int cPlayer = randInt(1, cols);
    m_pit->addPlayer(rPlayer, cPlayer);

    // Populate with scorpions
    while (nScorpions > 0)
    {
        int r = randInt(1, rows);
        int c = randInt(1, cols);
        // Don't put a scorpion where the player is
        if (r == rPlayer && c == cPlayer)
            continue;
        m_pit->addScorpion(r, c);
        nScorpions--;
    }
}

Game::~Game()
{
    delete m_pit;
}

void Game::play()
{
    m_pit->display();
    Player* player = m_pit->player();
    if (player == nullptr)
        return;

    while (!player->isDead() && m_pit->scorpionCount() > 0)
    {
        cout << "Move (u/d/l/r/q or nothing): ";
        string action;
        getline(cin, action);
        if (action.size() == 0)
            player->stand();
        else
        {
            switch (action[0])
            {
            default:   // if bad move, nobody moves
                cout << '\a' << endl;  // beep
                continue;
            case 'q':
                return;
            case 'u':
            case 'd':
            case 'l':
            case 'r':
                player->move(decodeDirection(action[0]));
                break;
            }
        }
        if (!player->isDead())
            m_pit->moveScorpions();
        m_pit->display();
    }
    if (player->isDead())
        cout << "You lose." << endl;
    else
        cout << "You win." << endl;
}

///////////////////////////////////////////////////////////////////////////
//  Auxiliary function implementation
///////////////////////////////////////////////////////////////////////////

  // Return a uniformly distributed random int from min to max, inclusive
int randInt(int min, int max)
{
    if (max < min)
        swap(max, min);
    static random_device rndv;
    static default_random_engine generator(rndv());
    uniform_int_distribution<> distro(min, max);
    return distro(generator);
}

int decodeDirection(char dir)
{
    switch (dir)
    {
    case 'u':  return UP;
    case 'd':  return DOWN;
    case 'l':  return LEFT;
    case 'r':  return RIGHT;
    }
    return -1;  // bad argument passed in!
}

bool directionToDeltas(int dir, int& rowDelta, int& colDelta)
{
    switch (dir)
    {
    case UP:     rowDelta = -1; colDelta = 0; break;
    case DOWN:   rowDelta = 1; colDelta = 0; break;
    case LEFT:   rowDelta = 0; colDelta = -1; break;
    case RIGHT:  rowDelta = 0; colDelta = 1; break;
    default:     return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////
//  main()
///////////////////////////////////////////////////////////////////////////

int main()
{
    // Create a game
    // Use this instead to create a mini-game:   Game g(3, 3, 2);

    Game g(9, 10 ,43);

    // Play the game
    g.play();
}

///////////////////////////////////////////////////////////////////////////
//  clearScreen implementation
///////////////////////////////////////////////////////////////////////////

// DO NOT MODIFY OR REMOVE ANYTHING BETWEEN HERE AND THE END OF THE FILE!!!
// THE CODE IS SUITABLE FOR VISUAL C++, XCODE, AND g++/g31 UNDER LINUX.

// Note to Xcode users:  clearScreen() will just write a newline instead
// of clearing the window if you launch your program from within Xcode.
// That's acceptable.  (The Xcode output window doesn't have the capability
// of being cleared.)

#ifdef _WIN32

#pragma warning(disable : 4005)  // redefined macro in <windows.h>
#define WIN32_LEAN_AND_MEAN  // fixes "byte" for pre-Windows SDK 10.0.20348.0
#include <windows.h>

void clearScreen()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    COORD upperLeft = { 0, 0 };
    DWORD dwCharsWritten;
    FillConsoleOutputCharacter(hConsole, TCHAR(' '), dwConSize, upperLeft,
        &dwCharsWritten);
    SetConsoleCursorPosition(hConsole, upperLeft);
}

#else  // not _WIN32

#include <iostream>
#include <cstring>
#include <cstdlib>

void clearScreen()  // will just write a newline in an Xcode output window
{
    static const char* term = std::getenv("TERM");
    if (term == nullptr || std::strcmp(term, "dumb") == 0)
        std::cout << std::endl;
    else
    {
        static const char* ESC_SEQ = "\x1B[";  // ANSI Terminal esc seq:  ESC [
        std::cout << ESC_SEQ << "2J" << ESC_SEQ << "H" << std::flush;
    }
}

#endif

