#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#define GLX_GLXEXT_LEGACY //Must be declared so that our local glxext.h is picked up, rather than the system one


#include <windows.h>
#include "windowOGL.h"
#include "GameConstants.h"
#include "cWNDManager.h"
#include "cInputMgr.h"
#include "cSound.h"
#include "cSprite.h"
#include "cBkGround.h"
#include "cFontMgr.h"
#include "cButton.h"
#include "asteroidsGame.h"
#include "cRocket.h"
#include "cAsteroid.h"

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR cmdLine,
                   int cmdShow)
{

    //Set our window settings
    const int windowWidth = 1024;
    const int windowHeight = 768;
    const int windowBPP = 16;

	int playerScore = 0; // Int used to store the player's score
	int playerLife = 3; // Int used to store the remaining of the player's lives
	bool musicPlaying = false; // Boolean used to determine if music is already being played
	string playerScoreString; // String used to store the player's score after conversion from int -> string
	string playerLifeString; // String used to store the player's remaining lives after conversion from int -> string

    //This is our window
	static cWNDManager* pgmWNDMgr = cWNDManager::getInstance();

	// This is the input manager
	static cInputMgr* theInputMgr = cInputMgr::getInstance();

	// This is the sound manager
	static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();
	
	// This is the Font manager
	static cFontMgr* theFontMgr = cFontMgr::getInstance();


    //The example OpenGL code
    windowOGL theOGLWnd;
	
    //Attach our the OpenGL window
	pgmWNDMgr->attachOGLWnd(&theOGLWnd);

	// Attach the keyboard manager
	pgmWNDMgr->attachInputMgr(theInputMgr);

    //Attempt to create the window
	if (!pgmWNDMgr->createWND(windowWidth, windowHeight, windowBPP))
    {
        //If it fails

        MessageBox(NULL, "Unable to create the OpenGL Window", "An error occurred", MB_ICONERROR | MB_OK);
		pgmWNDMgr->destroyWND(); //Reset the display and exit
        return 1;
    }

	if (!theOGLWnd.initOGL(windowWidth, windowHeight)) //Initialize our example
    {
        MessageBox(NULL, "Could not initialize the application", "An error occurred", MB_ICONERROR | MB_OK);
		pgmWNDMgr->destroyWND(); //Reset the display and exit
        return 1;
    }

	//Clear key buffers
	theInputMgr->clearBuffers(theInputMgr->KEYS_DOWN_BUFFER | theInputMgr->KEYS_PRESSED_BUFFER | theInputMgr->MOUSE_BUFFER);

	// initialize random seed:
	srand((unsigned int)time(NULL));

	// Create vector array of textures
	LPCSTR texturesToUseAstro[] = { "Images\\Fruit1.png", "Images\\Fruit2.png", "Images\\Fruit3.png", "Images\\Fruit4.png"};
	for (int tCount = 0; tCount < 4; tCount++)
	{
		theGameTextures.push_back(new cTexture());
		theGameTextures[tCount]->createTexture(texturesToUseAstro[tCount]);
	}

	// Load Sounds
	LPCSTR gameSounds[3] = { "Audio/Pacman Death.wav","Audio/Pacman Eat.wav", "Audio/Tetris Theme.wav" };

	theSoundMgr->add("fruitCatchFail", gameSounds[0]);
	theSoundMgr->add("fruitCatch", gameSounds[1]);
	theSoundMgr->add("Theme", gameSounds[2]);

	// Load Fonts
	LPCSTR gameFonts[2] = { "Fonts/digital-7.ttf", "Fonts/space age.ttf" };

	theFontMgr->addFont("SevenSeg", gameFonts[0], 24);
	theFontMgr->addFont("Space", gameFonts[1], 24);


	// Create vector array of textures for use as backgrounds.
	vector<cTexture*> textureBkgList;
	LPCSTR bkgTexturesToUse[] = { "Images/RaceTrack.png", "Images/RacingStartBkg.png", "Images/RacingEndBkg.png", "Images/instructionsBkg.png" };
	for (int tCount = 0; tCount < 4; tCount++)
	{
		textureBkgList.push_back(new cTexture());
		textureBkgList[tCount]->createTexture(bkgTexturesToUse[tCount]);
	}

	// Fruit Creation.

		for (int astro = 0; astro < 10; astro++)
		{
			theAsteroids.push_back(new cAsteroid);	// Fruit objects are created.
			theAsteroids[astro]->setSpritePos(glm::vec2((rand() % 471 + 276), 0.0f));		// Fruit objects are given a random position between x-coordinates 276 and 747, no variance on y-coordinates.
			theAsteroids[astro]->setSpriteTranslation(glm::vec2((0), (rand() % 30 + 30)));		// Fruit rotation is given a random fall speed between 1 and 2, along with a random fall speed between 30 and 60.
			int randAsteroid = rand() % 4;		// Fruit is then given a random integer between 0 and 4, this number assigns the texture of the fruit.
			theAsteroids[astro]->setTexture(theGameTextures[randAsteroid]->getTexture());	// Fruit is assigned the texture corresponding to the random integer they were assigned.
			theAsteroids[astro]->setTextureDimensions(theGameTextures[randAsteroid]->getTWidth(), theGameTextures[randAsteroid]->getTHeight());	// Texture dimensions of the fruit are set by looking at dimensions of assigned textures.
			theAsteroids[astro]->setSpriteCentre();	// The centre of the sprite/texture is then found.
			theAsteroids[astro]->setActive(true);	// Fruit's activity status is set to true as the fruit is active.

		}

	// The background texture is set using the texture list array.
	cBkGround spriteBkgd;
	spriteBkgd.setSpritePos(glm::vec2(0.0f, 0.0f));
	spriteBkgd.setTexture(textureBkgList[0]->getTexture());
	spriteBkgd.setTextureDimensions(textureBkgList[0]->getTWidth(), textureBkgList[0]->getTHeight());

	// The starting background is set using the texture list array.
	cBkGround spriteStartBkgd;
	spriteStartBkgd.setSpritePos(glm::vec2(0.0f, 0.0f));
	spriteStartBkgd.setTexture(textureBkgList[1]->getTexture());
	spriteStartBkgd.setTextureDimensions(textureBkgList[1]->getTWidth(), textureBkgList[1]->getTHeight());

	// The end screen background is set using the texture list array.
	cBkGround spriteEndBkgd;
	spriteEndBkgd.setSpritePos(glm::vec2(0.0f, 0.0f));
	spriteEndBkgd.setTexture(textureBkgList[2]->getTexture());
	spriteEndBkgd.setTextureDimensions(textureBkgList[2]->getTWidth(), textureBkgList[2]->getTHeight());

	// The end screen background is set using the texture list array.
	cBkGround instructionsBkgd;
	instructionsBkgd.setSpritePos(glm::vec2(0.0f, 0.0f));
	instructionsBkgd.setTexture(textureBkgList[3]->getTexture());
	instructionsBkgd.setTextureDimensions(textureBkgList[3]->getTWidth(), textureBkgList[2]->getTHeight());

	// Create vector array of button textures
	vector<cTexture*> btnTextureList;
	LPCSTR btnTexturesToUse[] = { "Images/Buttons/exitBtn.png", "Images/Buttons/instructionsBtn.png", "Images/Buttons/loadBtn.png", "Images/Buttons/playBtn.png", "Images/Buttons/saveBtn.png", "Images/Buttons/settingsBtn.png", "Images/Buttons/menuBtn.png"};
	for (int tCount = 0; tCount < 7; tCount++)
	{
		btnTextureList.push_back(new cTexture());
		btnTextureList[tCount]->createTexture(btnTexturesToUse[tCount]);
	}

	// Basket texture is created and stored under rocketTxt.
	cTexture rocketTxt;
	rocketTxt.createTexture("Images\\basketSprite.png");

	// Boundary is created, this texture is transparent.
	cTexture boundaryTexture;
	boundaryTexture.createTexture("Images\\bottomBoundry.png");

	// Boundary is then created as an instance of cSprite, the position is set to the bottom mid of the screen
	// and the texture is set, the texture dimensions are derived from the boundaryTexture created previously
	// and then we set the sprite centre.

	cSprite boundarySprite;
	boundarySprite.setSpritePos(glm::vec2(512.0f, 760.0f));
	boundarySprite.setTexture(boundaryTexture.getTexture());
	boundarySprite.setTextureDimensions(boundaryTexture.getTWidth(), boundaryTexture.getTHeight());
	boundarySprite.setSpriteCentre();

	// Basket object is created and stored under rocketSprite.
	cRocket rocketSprite;
	rocketSprite.attachInputMgr(theInputMgr); // Attach the input manager to the sprite
	rocketSprite.setSpritePos(glm::vec2(512.0f, 668.0f));	// Starting Basket position.
	rocketSprite.setTexture(rocketTxt.getTexture());	// The texture of the basket is then set.
	rocketSprite.setTextureDimensions(rocketTxt.getTWidth(), rocketTxt.getTHeight());	// Texture dimensions of the basket texture are found and stored.
	rocketSprite.setSpriteCentre();	// The centre of the Basket texture is found and stored.

	// Creation of the exit button.
	cButton exitButton;
	exitButton.attachInputMgr(theInputMgr);	// Attaching the input manager to the exit button.
	exitButton.setTexture(btnTextureList[0]->getTexture());	// The texture of the exit button is set using the btnTextureList array index 1.
	exitButton.setTextureDimensions(btnTextureList[0]->getTWidth(), btnTextureList[0]->getTHeight());	// The texture dimensions are found and stored for later use.

	// Creation of the play button.
	cButton playButton;
	playButton.attachInputMgr(theInputMgr);	// Attaching the input manager to the play button object.
	playButton.setTexture(btnTextureList[3]->getTexture());	// The texture of the play button is set using the btnTextureList array index 4.
	playButton.setTextureDimensions(btnTextureList[3]->getTWidth(), btnTextureList[3]->getTHeight());	// The texture dimensions are found and stored for later use.

	// Creation of the instructions button.
	cButton instButton;
	instButton.attachInputMgr(theInputMgr);	// Attaching the input manager to the instructions button object.
	instButton.setTexture(btnTextureList[1]->getTexture());	// The texture of the instructions button is set using the btnTextureList array index 2.
	instButton.setTextureDimensions(btnTextureList[1]->getTWidth(), btnTextureList[1]->getTHeight());	// The texture dimensions are found and stored for later use.

	// Creation of the text rendering string.
	string outputMsg;

	// Creation of the array for the strings displayed.
	string strMsg[] = { "Player Score:","To earn score collect fruit in the Basket!", "Fruitfall", "Thanks for Playing!","GAME OVER!", "Player Lifes:" };

	// Initial gamestate is set to MENU to make the game start at the main menu.
	gameState theGameState = MENU;
	btnTypes theBtnType = EXIT;

	//This is the mainloop, we render frames until isRunning returns false
	while (pgmWNDMgr->isWNDRunning())
    {
		pgmWNDMgr->processWNDEvents(); //Process any window events

        //We get the time that passed since the last frame
		float elapsedTime = pgmWNDMgr->getElapsedSeconds();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// The Vector array for the fruits is created.
		vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin();

		// The integer values of both the player's score and remaining lives are converted to strings
		// for later display using the font manager.
		std::string playerScoreString = std::to_string(playerScore);
		std::string playerLifeString = std::to_string(playerLife);

		//Statement to create different gamestates and what happens in these states, these states act as scenes.
		//Ie. MENU is when the player is in the main menu, PLAYING is in-game and END is the End Menu.

		switch (theGameState)
		{

		// Main Menu gamestate, this is the starting state of the game, buttons are contained within the MENU state that allow the state to
		// be switched to PLAYING or END.
		case MENU:

			spriteStartBkgd.render();

			// Render both the Play button and the End button aswell as the Instructions button.
			playButton.setSpritePos(glm::vec2(400.0f, 300.0f));
			exitButton.setSpritePos(glm::vec2(400.0f, 375.0f));
			instButton.setSpritePos(glm::vec2(400.0f, 450.0f));
			playButton.render();
			exitButton.render();
			instButton.render();

			// When the play button is clicked update the gamestate from MENU to PLAYING or if the Instructions button is pressed go to INSTRUCTIONS.
			theGameState = playButton.update(theGameState, PLAYING);
			theGameState = instButton.update(theGameState, INSTRUCTIONS);
			exitButton.update();
			instButton.update();

			// Render Text (Strings) that are stored in an array that was declared earlier, the array is strMsg[].
			outputMsg = strMsg[2];
			theFontMgr->getFont("Space")->printText(outputMsg.c_str(), FTPoint(470, 225, 0.0f));

			// When the exit button is clicked then a message is sent to the window manager to close the window.
			if (exitButton.getClicked())
			{
				SendMessage(pgmWNDMgr->getWNDHandle(), WM_CLOSE, NULL, NULL);
			}
			break;

		// Instructions screen gamestate, this shows the player how to play the game and the keys to control the basket.
		// From here the player can chose to go ingame or exit the game and close the window.
		case INSTRUCTIONS:

			instructionsBkgd.render();

			// Render both the play and exit buttons and also set the position of the texture.
			playButton.setSpritePos(glm::vec2(400.0f, 300.0f));
			exitButton.setSpritePos(glm::vec2(400.0f, 375.0f));
			playButton.render();
			exitButton.render();

			// When the play button is pressed update the gamestate to PLAYING.
			theGameState = playButton.update(theGameState, PLAYING);
			exitButton.update();

			// Draw the text on the screen associated with the index of the corresponding array (strMsg[]).
			outputMsg = strMsg[2];
			theFontMgr->getFont("Space")->printText(outputMsg.c_str(), FTPoint(470, 125, 0.0f));
			outputMsg = strMsg[1];
			theFontMgr->getFont("Space")->printText(outputMsg.c_str(), FTPoint(285, 500, 0.0f));

			// If the exit button is clicked send a message to the window manager to close the window.
			if (exitButton.getClicked())
			{
				SendMessage(pgmWNDMgr->getWNDHandle(), WM_CLOSE, NULL, NULL);
			}

			break;

		// The following is the PLAYING state, this state is accessed by the user via the play button on the main menu.
		// In this gamestate the game is running and fruit falls and the player can control the basket's movement.

		case PLAYING:

			if (musicPlaying == false)
			{
				theSoundMgr->getSnd("Theme")->playAudio(AL_TRUE);
				musicPlaying = true;
			}

			spriteBkgd.render();

			// The boundary at the bottom of the screen is updated.

			boundarySprite.updateSprite(elapsedTime);

			// Basket sprite updated and key buffers cleared.
			rocketSprite.update(elapsedTime);
			theInputMgr->clearBuffers(theInputMgr->KEYS_DOWN_BUFFER | theInputMgr->KEYS_PRESSED_BUFFER);

			// When the player runs out of lives the gamestate is changed to GAMEOVER.
			if (playerLife < 1)
			{
				theGameState = GAMEOVER;
			}

			// When the player catches all the fruit the gamestate is set to end.
			if (playerScore > 9)
			{
				theGameState = END;
			}
			

			// If the fruit iterator isn't finished but a fruit is marked inactive the fruit is erased, if they're still active update them and then render them
			// then move onto the next index.

			while (asteroidIterator != theAsteroids.end())
			{
				if ((*asteroidIterator)->isActive() == false)
				{
					asteroidIterator = theAsteroids.erase(asteroidIterator);
				}
				else
				{
					(*asteroidIterator)->update(elapsedTime);
					(*asteroidIterator)->render();
					++asteroidIterator;
				}
			}

			rocketSprite.render();

			// Collision process begins with the fruits being iterated through and then a check is performed between the fruit's bounding boxes and the basket's bounding box.
				for (vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin(); asteroidIterator != theAsteroids.end(); ++asteroidIterator)
				{
					if ((*asteroidIterator)->collidedWith((*asteroidIterator)->getBoundingRect(), rocketSprite.getBoundingRect2()))
					{
						
						// The collided fruit is marked as inactive and is ready for erasing, the player's score has 1 added to it and the sound for catching a fruit is played.
						(*asteroidIterator)->setActive(false);

						playerScore++;

						theSoundMgr->getSnd("fruitCatch")->playAudio(AL_TRUE);
						
					}

					// Collision process between both the fruit and the transparent boundary at the bottom of the screen.
					if ((*asteroidIterator)->collidedWith((*asteroidIterator)->getBoundingRect(), boundarySprite.getBoundingRect()))
					{

						// The collided fruit is marked as inactive and is ready for erasing, the player's life total has 1 taken from it and the sound for failing to catch a fruit is played.
						(*asteroidIterator)->setActive(false);

						playerLife--;

						theSoundMgr->getSnd("fruitCatchFail")->playAudio(AL_TRUE);

					}

					
				}
				
			
			// Exit button has coordinates set and is then rendered, if the exit button is pressed the gamestate is set to END.
			exitButton.setSpritePos(glm::vec2(768.0f, 575.0f));
			exitButton.render();
			theGameState = exitButton.update(theGameState, END);

			// The third string in the strMsg is loaded and drawn, in this case the string is "Fruitfall".
			outputMsg = strMsg[2];
			theFontMgr->getFont("Space")->printText(outputMsg.c_str(), FTPoint(25, 25, 0.0f));
			outputMsg = strMsg[0];
			theFontMgr->getFont("Space")->printText(outputMsg.c_str(), FTPoint(25, 175, 0.0f));
			outputMsg = playerScoreString;
			theFontMgr->getFont("Space")->printText(outputMsg.c_str(), FTPoint(200, 175, 0.0f)); // The converted player's score is printed
			outputMsg = strMsg[5];
			theFontMgr->getFont("Space")->printText(outputMsg.c_str(), FTPoint(25, 225, 0.0f));
			outputMsg = playerLifeString;
			theFontMgr->getFont("Space")->printText(outputMsg.c_str(), FTPoint(200, 225, 0.0f)); // The converted player's life total is printed

			break;

		case GAMEOVER:

			spriteStartBkgd.render();

			// Render both the play and exit buttons and also set the position of the texture.
			exitButton.setSpritePos(glm::vec2(400.0f, 375.0f));
			exitButton.render();

			// If the play button is clicked the gamestate is set to PLAYING and the player will be sent back in-game. -- This is not available due to bugs with restarting game, play button is removed.
			exitButton.update();

			outputMsg = strMsg[2];
			theFontMgr->getFont("Space")->printText(outputMsg.c_str(), FTPoint(25, 25, 0.0f));
			outputMsg = strMsg[3];
			theFontMgr->getFont("Space")->printText(outputMsg.c_str(), FTPoint(400, 200, 0.0f));
			outputMsg = strMsg[4];
			theFontMgr->getFont("Space")->printText(outputMsg.c_str(), FTPoint(430, 250, 0.0f)); // This is the game over message
			outputMsg = strMsg[0];
			theFontMgr->getFont("Space")->printText(outputMsg.c_str(), FTPoint(400, 300, 0.0f)); 
			outputMsg = playerScoreString;
			theFontMgr->getFont("Space")->printText(outputMsg.c_str(), FTPoint(600, 300, 0.0f)); // The converted player's score is printed


			// If the exit button is clicked send a message to the window manager to close the window.
			if (exitButton.getClicked())
			{
				SendMessage(pgmWNDMgr->getWNDHandle(), WM_CLOSE, NULL, NULL);
			}

			break;

			// End Screen Menu, players can select to exit the program entirely or start playing again via the buttons on this menu.
		case END:

			spriteEndBkgd.render();

			// Reset the active status of both the play button and the exit button.
			playButton.setClicked(false);
			exitButton.setClicked(false);

			// Set coordinates for the play and exit buttons and then render both.
			//playButton.setSpritePos(glm::vec2(400.0f, 300.0f));
			exitButton.setSpritePos(glm::vec2(400.0f, 375.0f));
			//playButton.render();
			exitButton.render();

			// If the play button is clicked the gamestate is set to PLAYING and the player will be sent back in-game. -- Disabled because replaying doesn't work.
			//theGameState = playButton.update(theGameState, PLAYING);
			exitButton.update();

			// Draw the text that is stored in index 3 and 4.
			outputMsg = strMsg[2];
			theFontMgr->getFont("Space")->printText(outputMsg.c_str(), FTPoint(25, 25, 0.0f));
			outputMsg = strMsg[3];
			theFontMgr->getFont("Space")->printText(outputMsg.c_str(), FTPoint(400, 200, 0.0f));
			outputMsg = strMsg[0];
			theFontMgr->getFont("Space")->printText(outputMsg.c_str(), FTPoint(400, 275, 0.0f));
			outputMsg = playerScoreString;
			theFontMgr->getFont("Space")->printText(outputMsg.c_str(), FTPoint(575, 275, 0.0f)); // The converted player's score is printed

			// If the exit button is clicked then the window manager will close the window.
			if (exitButton.getClicked())
			{
				SendMessage(pgmWNDMgr->getWNDHandle(), WM_CLOSE, NULL, NULL);
			}

			break;
		}

		pgmWNDMgr->swapBuffers();
		//theInputMgr->clearBuffers(theInputMgr->KEYS_DOWN_BUFFER | theInputMgr->KEYS_PRESSED_BUFFER | theInputMgr->MOUSE_BUFFER);
    }

	theOGLWnd.shutdown(); //Free any resources
	pgmWNDMgr->destroyWND(); //Destroy the program window

    return 0; //Return success
}
