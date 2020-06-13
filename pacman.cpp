//Using SDL, SDL_image, standard math, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>

//declare classes
class Player;
class Tile;
class Board;
class LTexture;

//Direction constants
enum Directions
{
    RIGHT,
    LEFT,
    UP,
    DOWN,
    DEFAULT
};

//Screen dimension constants
const int SCREEN_WIDTH = 224;
const int SCREEN_HEIGHT = 248;

//Player class
class Entity
{
    public:
        //initializes variables
        Entity();

        //Deallocates entity
        ~Entity();

        //Frees memory
        void free();

        //gets coordinates and direction
        int getXLoc();
        int getYLoc();
        int getOrientation();

        //gets correct sprite
        int currentSprite();

        //sets coordinates and direction
        void setYLoc(int y);
        void setXLoc(int x);
        void setOrientation(int dir);

        //checks if mouth is open or closed
        bool getMouthOpen();

        //opens or closes mouth
        void setMouthOpen(bool b);

        //sets next turn
        void setNextTurn(int turn);

        //updates location
        void update(Board b);

        //where entity will try to turn
        int nextTurn;

        //activate/deactivate scatter mode
        void setScatter(bool b)
        {
            scatterMode = b;
        }

        //sets fleeMode
        void setFlee(bool b)
        {
            fleeMode = b;
        }

        //retrieve scatterMode
        bool getScatter()
        {
            return scatterMode;
        }

        //retrieves fleeMode
        bool getFlee()
        {
            return fleeMode;
        }

        //returns if two entities share tile
        bool sharingTile(Entity e)
        {
            return (xLoc - 4) / 8 == (e.xLoc - 4) / 8 && (yLoc - 4) / 8 == (e.yLoc - 4) / 8;
        }

    private:
        //coordinates and direction of movement
        int xLoc;
        int yLoc;
        int orientation;

        //mouth open or closed
        bool mouthOpen;

        //scatter mode activated
        bool scatterMode;

        //flee mode activated
        bool fleeMode;
};

class Player : public Entity
{
    public:
        //initializes variables
        Player()
        {
            //Sets player at starting point and direction
            setXLoc(15);
            setYLoc(13);
            setOrientation(RIGHT);
            setNextTurn(DEFAULT);
            death = 0;
        }

        int getDeathState()
        {
            return death;
        }

        //kills player
        void kill()
        {
            if (death < 11)
            {
                death++;
            }
        }
    private:
        //records if player is dead or dying
        int death;
};

//the red ghost
class Blinky : public Entity
{
    public:
        Blinky();
        void decideTurn(Board b, Player p);
};

//the pink ghost
class Pinky : public Entity
{
    public:
        Pinky();
        void decideTurn(Board b, Player p);
};

//the blue ghost
class Inky : public Entity
{
    public:
        Inky();
        void decideTurn(Board board, Player p, Blinky b);
};

//the blue ghost
class Clyde : public Entity
{
    public:
        Clyde();
        void decideTurn(Board b, Player p);
};


//board tile class
class Tile
{
    public:
        //initializes variables
        Tile();

        //check if tile has been crossed
        bool getCrossed();

        //crosses of tile
        void crossOff();

        //check if player can move in each direction
        bool canGoUp();
        bool canGoDown();
        bool canGoLeft();
        bool canGoRight();

        //sets directions player can move in
        void setGoDown(bool b);
        void setGoUp(bool b);
        void setGoLeft(bool b);
        void setGoRight(bool b);

    private:
        //keeps track if tile has been crossed
        bool hasCrossed;

        //keeps track of entrances/exits
        bool goUp;
        bool goDown;
        bool goLeft;
        bool goRight;
};

//Game board class
class Board
{
    public:
        //initializes variables
        Board();

        //updates board
        void updateBoard(Player player);

        //grabs tile
        Tile getTile(int i, int j);

    private:
        //keeps track of tiles player has been on
        Tile tiles[31][28];
};

//Texture wrapper class
class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadFromFile( std::string path );

		//Deallocates texture
		void free();

		//Renders texture at given point
		void render( int x, int y, SDL_Rect* clip = NULL );

		//Gets image dimensions
		int getWidth();
		int getHeight();

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene sprites
SDL_Rect gBoard;
SDL_Rect gPlayer[8];
SDL_Rect gBoardPieces[31][28];
SDL_Rect gBoardPiecesAlt[31][28];
SDL_Rect gBlinky[8];
SDL_Rect gPinky[8];
SDL_Rect gInky[8];
SDL_Rect gClyde[8];
SDL_Rect gMiscGhost[8];
SDL_Rect gPlayerDeath[12];
LTexture gSpriteSheetTexture;

Tile::Tile()
{
    hasCrossed = false;
    goUp = false;
    goDown = false;
    goLeft = false;
    goRight = false;
}

void Tile::setGoDown(bool b)
{
    goDown = b;
}

void Tile::setGoUp(bool b)
{
    goUp = b;
}

void Tile::setGoLeft(bool b)
{
    goLeft = b;
}

void Tile::setGoRight(bool b)
{
    goRight = b;
}

bool Tile::canGoDown()
{
    return goDown;
}

bool Tile::canGoUp()
{
    return goUp;
}

bool Tile::canGoLeft()
{
    return goLeft;
}

bool Tile::canGoRight()
{
    return goRight;
}

bool Tile::getCrossed()
{
    return hasCrossed;
}

void Tile::crossOff()
{
    hasCrossed = true;
}

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0, 0 ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::render( int x, int y, SDL_Rect* clip )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopy( gRenderer, mTexture, clip, &renderQuad );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

Entity::Entity()
{
    mouthOpen = true;
}

Entity::~Entity()
{
    //Deallocate entity
    free();
}

void Entity::free()
{
    xLoc = 0;
    yLoc = 0;
    orientation = 0;
    mouthOpen = 0;
}

void Entity::update(Board b)
{
    //Changes mouth to open or closed, or the other way
    setMouthOpen(!getMouthOpen());

    //Moves entity coordinates based on direction of movement
    //entity will traverse exactly one tile (8 pixels) at a time
    //distances are offset by starting location
    int xTile = (getXLoc() - 4) / 8;
    int yTile = (getYLoc() - 4) / 8;
    for(int i = 0; i < 2; i++)
    {
        switch(getOrientation())
        {
        //max added for wraparound
        case(RIGHT):
        if (b.getTile(yTile, std::max(xTile, 0)).canGoRight() || ((getXLoc()- 15) % 8 != 0) || ((getYLoc() - 13) % 8 != 0))
        {
            setXLoc(getXLoc() + 1);
        }
        break;

        //min added for wraparound
        case(LEFT):
        if (b.getTile(yTile, std::min(xTile, 27)).canGoLeft() || ((getXLoc() - 15) % 8 != 0) || ((getYLoc() - 13) % 8 != 0))
        {
            setXLoc(getXLoc() - 1);
        }
        break;

        case(UP):
        if (b.getTile(yTile, xTile).canGoUp() || ((getXLoc() - 15) % 8 != 0) || ((getYLoc() - 13) % 8 != 0))
        {
            setYLoc(getYLoc() - 1);
        }
        break;

        case(DOWN):
        if (b.getTile(yTile, xTile).canGoDown() || ((getXLoc() - 15) % 8 != 0) || ((getYLoc() - 13) % 8 != 0))
        {
            setYLoc(getYLoc() + 1);
        }
        break;

        default:
        break;
        }
    }

    //wrap around
    if (getXLoc() > 224)
    {
        setXLoc(-1);
    }
    else if (getXLoc() < 0)
    {
        setXLoc(221);
    }

    //reset theser variables
    yTile = (getYLoc() - 4) / 8;
    xTile = (getXLoc() - 4) / 8;

    //change direction if you can
    //direction will be changed as soon as entity is fully on a tile that can exit in that direction
    if ((getXLoc() - 15) % 8 == 0 && (getYLoc() - 13) % 8 == 0)
    {
        switch(nextTurn)
        {
            case(UP):
            if (b.getTile(yTile, xTile).canGoUp())
            {
                setOrientation(UP);
            }
            break;
            case(DOWN):
            if (b.getTile(yTile, xTile).canGoDown())
            {
                setOrientation(DOWN);
            }
            break;
            case(LEFT):
            if (b.getTile(yTile, xTile).canGoLeft())
            {
                setOrientation(LEFT);
            }
            break;
            case(RIGHT):
            if (b.getTile(yTile, xTile).canGoRight())
            {
                setOrientation(RIGHT);
            }
            break;
            default:
            break;
        }
    }
}

void Entity::setXLoc(int x)
{
    xLoc = x;
}

void Entity::setYLoc(int y)
{
    yLoc = y;
}

void Entity::setOrientation(int dir)
{
    orientation = dir;
}

int Entity::getXLoc()
{
    return xLoc;
}

int Entity::getYLoc()
{
    return yLoc;
}

int Entity::getOrientation()
{
    return orientation;
}

bool Entity::getMouthOpen()
{
    return mouthOpen;
}

void Entity::setMouthOpen(bool b)
{
    mouthOpen = b;
}

int Entity::currentSprite()
{
    return (getOrientation() * 2) + mouthOpen;
}

void Entity::setNextTurn(int turn)
{
    nextTurn = turn;
}

Blinky::Blinky()
{
    //spawns near ghost house
    setXLoc(111);
    setYLoc(93);
    setOrientation(UP);
}

Pinky::Pinky()
{
    setXLoc(127);
    setYLoc(93);
    setOrientation(UP);
}

Inky::Inky()
{
    setXLoc(143);
    setYLoc(93);
    setOrientation(UP);
}

Clyde::Clyde()
{
    setXLoc(159);
    setYLoc(93);
    setOrientation(UP);
}

void Blinky::decideTurn(Board b, Player p)
{
    //finds distance between blinky and player
    int yTile = (getYLoc() - 4) / 8;
    int xTile = (getXLoc() - 4) / 8;
    Tile currentTile = b.getTile(yTile, xTile);
    int goalY;
    int goalX;
    if (!getScatter())
    {
        goalY = (p.getYLoc() - 4) / 8;
        goalX = (p.getXLoc() - 4) / 8;
    }
    else
    {
        goalY = 0;
        goalX = 27;
    }

    //blinky will always try to go towards the player but can only turn in 90 degree angles and only thinks one space ahead
    int best = DEFAULT;
    int hamilton = abs(goalY - yTile) + abs(goalX - xTile) + 20;
    int hypotheticalDistance;
    if (currentTile.canGoDown() && getOrientation() != UP)
    {
        hypotheticalDistance = abs(goalY - (yTile + 1)) + abs(goalX - xTile);
        if (hypotheticalDistance < hamilton)
        {
            best = DOWN;
            hamilton = hypotheticalDistance;
        }
    }
    if (currentTile.canGoUp() && getOrientation() != DOWN)
    {
        hypotheticalDistance = abs(goalY - (yTile - 1)) + abs(goalX - xTile);
        if (hypotheticalDistance < hamilton)
        {
            best = UP;
            hamilton = hypotheticalDistance;
        }
    }
    if (currentTile.canGoLeft() && getOrientation() != RIGHT)
    {
        hypotheticalDistance = abs(goalY - yTile) + abs(goalX - (xTile - 1));
        if (hypotheticalDistance < hamilton)
        {
            best = LEFT;
            hamilton = hypotheticalDistance;
        }
    }
    if (currentTile.canGoRight() && getOrientation() != LEFT)
    {
        hypotheticalDistance = abs(goalY - yTile) + abs(goalX - (xTile + 1));
        if (hypotheticalDistance < hamilton)
        {
            best = RIGHT;
            hamilton = hypotheticalDistance;
        }
    }
    setNextTurn(best);
}

void Pinky::decideTurn(Board b, Player p)
{
    //finds distance between pinky and goal (front of players mouth)
    int yTile = (getYLoc() - 4) / 8;
    int xTile = (getXLoc() - 4) / 8;
    Tile currentTile = b.getTile(yTile, xTile);
    int goalX;
    int goalY;
    if (!getScatter())
    {
        goalY = (p.getYLoc() - 4) / 8;
        goalX = (p.getXLoc() - 4) / 8;

        switch(p.getOrientation())
        {
            case LEFT:
                goalX -= 4;
                break;
            case RIGHT:
                goalX += 4;
                break;
            case UP:
                goalY -= 4;
                break;
            case DOWN:
                goalY += 4;
                break;
            default:
                break;
        }
    }
    else
    {
        goalY = 0;
        goalX = 0;
    }

    //pinky will always try to go towards the goal but can only turn in 90 degree angles and only thinks one space ahead
    int best = DEFAULT;
    int hamilton = abs(goalY - yTile) + abs(goalX - xTile) + 20;
    int hypotheticalDistance;
    if (currentTile.canGoDown() && getOrientation() != UP)
    {
        hypotheticalDistance = abs(goalY - (yTile + 1)) + abs(goalX - xTile);
        if (hypotheticalDistance < hamilton)
        {
            best = DOWN;
            hamilton = hypotheticalDistance;
        }
    }
    if (currentTile.canGoUp() && getOrientation() != DOWN)
    {
        hypotheticalDistance = abs(goalY - (yTile - 1)) + abs(goalX - xTile);
        if (hypotheticalDistance < hamilton)
        {
            best = UP;
            hamilton = hypotheticalDistance;
        }
    }
    if (currentTile.canGoLeft() && getOrientation() != RIGHT)
    {
        hypotheticalDistance = abs(goalY - yTile) + abs(goalX - (xTile - 1));
        if (hypotheticalDistance < hamilton)
        {
            best = LEFT;
            hamilton = hypotheticalDistance;
        }
    }
    if (currentTile.canGoRight() && getOrientation() != LEFT)
    {
        hypotheticalDistance = abs(goalY - yTile) + abs(goalX - (xTile + 1));
        if (hypotheticalDistance < hamilton)
        {
            best = RIGHT;
            hamilton = hypotheticalDistance;
        }
    }
    setNextTurn(best);
}

void Inky::decideTurn(Board b, Player p, Blinky blink)
{
    //finds distance between inky and goal
    int yTile = (getYLoc() - 4) / 8;
    int xTile = (getXLoc() - 4) / 8;
    Tile currentTile = b.getTile(yTile, xTile);
    int goalX;
    int goalY;
    if (!getScatter())
    {
        goalY = (p.getYLoc() - 4) / 8;
        goalX = (p.getXLoc() - 4) / 8;
        int blinkyX = blink.getXLoc();
        int blinkyY = blink.getYLoc();

        switch(p.getOrientation())
        {
            case LEFT:
                goalX -= 2;
                break;
            case RIGHT:
                goalX += 2;
                break;
            case UP:
                goalY -= 2;
                break;
            case DOWN:
                goalY += 2;
                break;
            default:
                break;
        }

        goalX += (goalX - blinkyX);
        goalY += (goalY - blinkyY);
    }
    else
    {
        goalX = 0;
        goalY = 30;
    }

    //inky will always try to go towards the goal but can only turn in 90 degree angles and only thinks one space ahead
    int best = DEFAULT;
    int hamilton = abs(goalY - yTile) + abs(goalX - xTile) + 20;
    int hypotheticalDistance;
    if (currentTile.canGoDown() && getOrientation() != UP)
    {
        hypotheticalDistance = abs(goalY - (yTile + 1)) + abs(goalX - xTile);
        if (hypotheticalDistance < hamilton)
        {
            best = DOWN;
            hamilton = hypotheticalDistance;
        }
    }
    if (currentTile.canGoUp() && getOrientation() != DOWN)
    {
        hypotheticalDistance = abs(goalY - (yTile - 1)) + abs(goalX - xTile);
        if (hypotheticalDistance < hamilton)
        {
            best = UP;
            hamilton = hypotheticalDistance;
        }
    }
    if (currentTile.canGoLeft() && getOrientation() != RIGHT)
    {
        hypotheticalDistance = abs(goalY - yTile) + abs(goalX - (xTile - 1));
        if (hypotheticalDistance < hamilton)
        {
            best = LEFT;
            hamilton = hypotheticalDistance;
        }
    }
    if (currentTile.canGoRight() && getOrientation() != LEFT)
    {
        hypotheticalDistance = abs(goalY - yTile) + abs(goalX - (xTile + 1));
        if (hypotheticalDistance < hamilton)
        {
            best = RIGHT;
            hamilton = hypotheticalDistance;
        }
    }
    setNextTurn(best);
}

Board::Board()
{
    //create initial tiles
    //this was BY FAR the most tedious part
    for (int i = 0; i < 31; i++)
    {
        for (int j = 0; j < 28; j++)
        {
            tiles[i][j] = Tile();
        }
    }

    //add in barriers
    for (int i = 1; i < 27; i++)
    {
        if (i != 14 && i != 13)
        {
            tiles[1][i].setGoRight(true);
            tiles[1][i].setGoLeft(true);
            tiles[20][i].setGoLeft(true);
            tiles[20][i].setGoRight(true);
        }

    }
    tiles[1][12].setGoRight(false);
    tiles[1][15].setGoLeft(false);
    tiles[1][1].setGoLeft(false);
    tiles[1][26].setGoRight(false);
    tiles[20][12].setGoRight(false);
    tiles[20][15].setGoLeft(false);
    tiles[20][1].setGoLeft(false);
    tiles[20][26].setGoRight(false);

    for (int i = 1; i < 27; i++)
    {
        if (i != 1)
        {
            tiles[5][i].setGoLeft(true);
        }
        if (i != 26)
        {
            tiles[5][i].setGoRight(true);
        }
    }

    for (int i = 1; i < 9; i++)
    {
        if (i != 1)
        {
            tiles[i][1].setGoUp(true);
            tiles[i][26].setGoUp(true);
        }
        if (i != 8)
        {
            tiles[i][1].setGoDown(true);
            tiles[i][26].setGoDown(true);
        }
    }

    for (int i = 1; i < 7; i++)
    {
        if (i != 1)
        {
            tiles[8][i].setGoLeft(true);
            tiles[26][i].setGoLeft(true);
        }
        if (i != 6)
        {
            tiles[8][i].setGoRight(true);
            tiles[26][i].setGoRight(true);
        }
    }

    tiles[8][9].setGoRight(true);
    tiles[8][10].setGoRight(true);
    tiles[8][11].setGoRight(true);
    tiles[8][10].setGoLeft(true);
    tiles[8][11].setGoLeft(true);
    tiles[8][12].setGoLeft(true);

    tiles[8][15].setGoRight(true);
    tiles[8][16].setGoRight(true);
    tiles[8][17].setGoRight(true);
    tiles[8][16].setGoLeft(true);
    tiles[8][17].setGoLeft(true);
    tiles[8][18].setGoLeft(true);

    tiles[26][9].setGoRight(true);
    tiles[26][10].setGoRight(true);
    tiles[26][11].setGoRight(true);
    tiles[26][10].setGoLeft(true);
    tiles[26][11].setGoLeft(true);
    tiles[26][12].setGoLeft(true);

    tiles[26][15].setGoRight(true);
    tiles[26][16].setGoRight(true);
    tiles[26][17].setGoRight(true);
    tiles[26][16].setGoLeft(true);
    tiles[26][17].setGoLeft(true);
    tiles[26][18].setGoLeft(true);

    for (int i = 21; i < 27; i++)
    {
        if (i != 21)
        {
            tiles[8][i].setGoLeft(true);
        }
        if (i != 26)
        {
            tiles[8][i].setGoRight(true);
        }
    }

    for (int i = 1; i < 27; i++) {
        if (i != 1)
        {
            tiles[i][6].setGoUp(true);
            tiles[i][21].setGoUp(true);
            tiles[29][i].setGoLeft(true);
        }
        if (i != 26)
        {
            tiles[i][6].setGoDown(true);
            tiles[i][21].setGoDown(true);
            tiles[29][i].setGoRight(true);
        }
    }

    for (int i = 0; i < 10; i++)
    {
        tiles[14][i].setGoLeft(true);
        if (i != 9)
        {
            tiles[14][i].setGoRight(true);
        }
    }

    for (int i = 18; i < 28; i++)
    {
        tiles[14][i].setGoRight(true);
        if (i != 18)
        {
            tiles[14][i].setGoLeft(true);
        }
    }

    for (int i = 9; i < 19; i++)
    {
        if (i != 9)
        {
            tiles[11][i].setGoLeft(true);
            tiles[17][i].setGoLeft(true);
        }
        if (i != 18)
        {
            tiles[11][i].setGoRight(true);
            tiles[17][i].setGoRight(true);
        }
    }

    for (int i = 11; i < 21; i++)
    {
        if (i != 11)
        {
            tiles[i][9].setGoUp(true);
            tiles[i][18].setGoUp(true);
        }
        if (i != 20)
        {
            tiles[i][9].setGoDown(true);
            tiles[i][18].setGoDown(true);
        }
    }

    for (int i = 1; i < 6; i++)
    {
        if (i != 1)
        {
            tiles[i][12].setGoUp(true);
            tiles[i][15].setGoUp(true);
        }
        if (i != 5)
        {
            tiles[i][12].setGoDown(true);
            tiles[i][15].setGoDown(true);
        }
    }

    for (int i = 5; i < 9; i++)
    {
        if (i != 5)
        {
            tiles[i][9].setGoUp(true);
            tiles[i][18].setGoUp(true);
        }
        if (i != 8)
        {
            tiles[i][9].setGoDown(true);
            tiles[i][18].setGoDown(true);
        }
    }

    for (int i = 8; i < 12; i++)
    {
        if (i != 8)
        {
            tiles[i][12].setGoUp(true);
            tiles[i][15].setGoUp(true);
        }
        if (i != 11)
        {
            tiles[i][12].setGoDown(true);
            tiles[i][15].setGoDown(true);
        }
    }

    for (int i = 20; i < 24; i++)
    {
        if (i != 20)
        {
            tiles[i][1].setGoUp(true);
            tiles[i][12].setGoUp(true);
            tiles[i][15].setGoUp(true);
            tiles[i][26].setGoUp(true);
        }
        if (i != 23)
        {
            tiles[i][1].setGoDown(true);
            tiles[i][12].setGoDown(true);
            tiles[i][15].setGoDown(true);
            tiles[i][26].setGoDown(true);
        }
    }

    for (int i = 6; i < 22; i++) {
        if (i != 6)
        {
            tiles[23][i].setGoLeft(true);
        }
        if (i != 21)
        {
            tiles[23][i].setGoRight(true);
        }
    }

    for (int i = 23; i < 27; i++)
    {
        if (i != 23)
        {
            tiles[i][9].setGoUp(true);
            tiles[i][18].setGoUp(true);
            tiles[i][3].setGoUp(true);
            tiles[i][24].setGoUp(true);
        }
        if (i != 26)
        {
            tiles[i][9].setGoDown(true);
            tiles[i][18].setGoDown(true);
            tiles[i][3].setGoDown(true);
            tiles[i][24].setGoDown(true);
        }
    }

    for (int i = 21; i < 27; i++)
    {
        if (i != 26)
        {
            tiles[26][i].setGoRight(true);
        }
        if (i != 21)
        {
            tiles[26][i].setGoLeft(true);
        }
    }

    tiles[23][1].setGoRight(true);
    tiles[23][2].setGoRight(true);
    tiles[23][2].setGoLeft(true);
    tiles[23][3].setGoLeft(true);

    tiles[23][24].setGoRight(true);
    tiles[23][25].setGoRight(true);
    tiles[23][25].setGoLeft(true);
    tiles[23][26].setGoLeft(true);

    for (int i = 26; i < 30; i++)
    {
        if (i != 26)
        {
            tiles[i][1].setGoUp(true);
            tiles[i][12].setGoUp(true);
            tiles[i][15].setGoUp(true);
            tiles[i][26].setGoUp(true);
        }
        if (i != 29)
        {
            tiles[i][1].setGoDown(true);
            tiles[i][12].setGoDown(true);
            tiles[i][15].setGoDown(true);
            tiles[i][26].setGoDown(true);
        }
    }
}

void Board::updateBoard(Player p)
{
    //finds tile player is on
    int xTile = p.getXLoc() / 8;
    int yTile = p.getYLoc() / 8;

    //crosses off that tile if it has not been
    if (!tiles[yTile][xTile].getCrossed())
    {
        tiles[yTile][xTile].crossOff();
    }
}

Tile Board::getTile(int i, int j)
{
    return tiles[i][j];
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "Pac-Man", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0, 0, 0, 0 );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_mage Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

int abs(int n)
{
    return n > 0 ? n : -n;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load sprite sheet texture
	if( !gSpriteSheetTexture.loadFromFile( "pacman.png" ) )
	{
		printf( "Failed to load sprite sheet texture!\n" );
		success = false;
	}
	else
	{
		//Set board sprite
		gBoard.x =   0;
		gBoard.y =   0;
		gBoard.w = 224;
		gBoard.h = 248;

		//Set player sprites
		for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                int index = (2 * i) + j;
                gPlayer[index].x = 456 + (16 * j);
                gPlayer[index].y = (16 * i);
                gPlayer[index].w = 16;
                gPlayer[index].h = 16;
            }
		}

		//Split up board into tiles
		for (int i = 0; i < 31; i++)
        {
            for (int j = 0; j < 28; j++)
            {
                gBoardPieces[i][j].y = i * 8;
                gBoardPieces[i][j].x = j * 8;
                gBoardPieces[i][j].w = 8;
                gBoardPieces[i][j].h = 8;
            }
        }

        //Do the same for alternate board
        for (int i = 0; i < 31; i++)
        {
            for (int j = 0; j < 28; j++)
            {
                gBoardPiecesAlt[i][j].y = i * 8;
                gBoardPiecesAlt[i][j].x = (j * 8) + 228;
                gBoardPiecesAlt[i][j].w = 8;
                gBoardPiecesAlt[i][j].h = 8;
            }
        }

        //Set sprites for ghosts
        for (int i = 0; i < 8; i++)
        {
            gBlinky[i].y = 64;
            gBlinky[i].x = 456 + (16 * i);
            gBlinky[i].w = 16;
            gBlinky[i].h = 16;

            gPinky[i].y = 80;
            gPinky[i].x = 456 + (16 * i);
            gPinky[i].w = 16;
            gPinky[i].h = 16;

            gInky[i].y = 96;
            gInky[i].x = 456 + (16 * i);
            gInky[i].w = 16;
            gInky[i].h = 16;

            gClyde[i].y = 112;
            gClyde[i].x = 456 + (16 * i);
            gClyde[i].w = 16;
            gClyde[i].h = 16;
        }

        //set sprites for fleeing ghosts
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                int index = (4 * i) + j;
                gMiscGhost[index].y = 64 + (16 * i);
                gMiscGhost[index].x = 584 + (16 * j);
                gMiscGhost[index].w = 16;
                gMiscGhost[index].h = 16;
            }
        }

        //set sprites for death sequence
        for (int i = 0; i < 12; i++)
        {
            gPlayerDeath[i].y = 0;
            gPlayerDeath[i].x = 488 + (16 * i);
            gPlayerDeath[i].w = 16;
            gPlayerDeath[i].h = 16;
        }
	}

	return success;
}

void close()
{
	//Free loaded images
	gSpriteSheetTexture.free();

	//Destroy window
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//Current frame
			int frame = 0;

			//frame when fleeing starts
			int fleeFrame = frame;

			//create board
			Board board;

			//create player
			Player player;

			//create ghosts
			Blinky blinky;
			Pinky pinky;
			Inky inky;

			//While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}
					//User presses a key
					else if( e.type == SDL_KEYDOWN )
                    {
                        //Change player's direction to key press
                        //make sure there is no wall in respective cases
                        switch( e.key.keysym.sym )
                        {
                            case SDLK_RIGHT:
                            player.setNextTurn(RIGHT);
                            break;

                            case SDLK_LEFT:
                            player.setNextTurn(LEFT);
                            break;

                            case SDLK_UP:
                            player.setNextTurn(UP);
                            break;

                            case SDLK_DOWN:
                            player.setNextTurn(DOWN);
                            break;

                            default:
                            break;
                        }
                    }
				}
				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0, 0, 0, 0 );
				SDL_RenderClear( gRenderer );

				//Render game board
				//gSpriteSheetTexture.render( 0, 0, &gBoard );
				for (int i = 0; i < 31; i++)
                {
                    for (int j = 0; j < 28; j++)
                    {
                        //if tile has been crossed, use alt
                        if (board.getTile(i, j).getCrossed())
                        {
                            gSpriteSheetTexture.render(j * 8, i *8, &gBoardPiecesAlt[i][j]);
                        }
                        else
                        {
                            gSpriteSheetTexture.render(j * 8, i *8, &gBoardPieces[i][j]);
                        }
                    }
				}

				//Render player
				//offset sprite, x and y location refer to center of player
				if (player.getDeathState())
                {
                    gSpriteSheetTexture.render(player.getXLoc() - 10, player.getYLoc() - 8, &gPlayerDeath[player.getDeathState()]);
                }
                else
                {
                    gSpriteSheetTexture.render(player.getXLoc() - 10, player.getYLoc() - 8, &gPlayer[player.currentSprite()]);
                }

				//render ghosts
				if (blinky.getFlee())
                {
                    if (frame > fleeFrame + 420 && (frame / 20) % 2 == 0) //ghosts will blink near end of flee mode
                    {
                        gSpriteSheetTexture.render(blinky.getXLoc() - 10, blinky.getYLoc() - 8, &gMiscGhost[blinky.getMouthOpen() + 2]);
                        gSpriteSheetTexture.render(pinky.getXLoc() - 10, pinky.getYLoc() - 8, &gMiscGhost[pinky.getMouthOpen() + 2]);
                        gSpriteSheetTexture.render(inky.getXLoc() - 10, inky.getYLoc() - 8, &gMiscGhost[inky.getMouthOpen() + 2]);
                    }
                    else
                    {
                        gSpriteSheetTexture.render(blinky.getXLoc() - 10, blinky.getYLoc() - 8, &gMiscGhost[blinky.getMouthOpen()]);
                        gSpriteSheetTexture.render(pinky.getXLoc() - 10, pinky.getYLoc() - 8, &gMiscGhost[pinky.getMouthOpen()]);
                        gSpriteSheetTexture.render(inky.getXLoc() - 10, inky.getYLoc() - 8, &gMiscGhost[inky.getMouthOpen()]);
                    }
                }
                else
                {
                    gSpriteSheetTexture.render(blinky.getXLoc() - 10, blinky.getYLoc() - 8, &gBlinky[blinky.currentSprite()]);
                    gSpriteSheetTexture.render(pinky.getXLoc() - 10, pinky.getYLoc() - 8, &gPinky[pinky.currentSprite()]);
                    gSpriteSheetTexture.render(inky.getXLoc() - 10, inky.getYLoc() - 8, &gInky[inky.currentSprite()]);
                }

				//Update board
				board.updateBoard(player);

                //check if power ball is eaten
                int xTile = (player.getXLoc() - 4) / 8;
                int yTile = (player.getYLoc() - 4) / 8;
                if (yTile == 3 && (xTile == 1 || xTile == 26))
                {
                    blinky.setFlee(true);
                    pinky.setFlee(true);
                    inky.setFlee(true);
                    fleeFrame = frame;
                }
                else if (yTile == 23 && (xTile == 1 || xTile == 26))
                {
                    blinky.setFlee(true);
                    pinky.setFlee(true);
                    inky.setFlee(true);
                    fleeFrame = frame;
                }

                if (fleeFrame != 0 && frame == fleeFrame + 600)
                {
                    blinky.setFlee(false);
                    pinky.setFlee(false);
                    inky.setFlee(false);
                }

				if (frame % 3 == 0)
                {
                    //checks if player is dead/dying
                    if (player.getDeathState() || ((player.sharingTile(blinky) || player.sharingTile(pinky) || player.sharingTile(inky)) && !blinky.getFlee()))
                    {
                        player.kill();
                        //ends game once death animation is over
                        if (player.getDeathState() == 11)
                        {
                            quit = true;
                        }
                    }
                    else
                    {
                        //Update player
                        player.update(board);
                    }
				}

                //update ghosts
				if (frame % 4 == 0 && !player.getDeathState())
                {
                    if (frame % 1620 == 0)
                    {
                        blinky.setScatter(true);
                        pinky.setScatter(true);
                        inky.setScatter(true);
                    }
                    else if ((frame + 1200) % 1620 == 0)
                    {
                        blinky.setScatter(false);
                        pinky.setScatter(false);
                        inky.setScatter(false);
                    }
                    blinky.decideTurn(board, player);
                    blinky.update(board);
                    pinky.decideTurn(board, player);
                    pinky.update(board);
                    inky.decideTurn(board, player, blinky);
                    inky.update(board);
                }
                frame++;

				//Update screen
				SDL_RenderPresent( gRenderer );
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
