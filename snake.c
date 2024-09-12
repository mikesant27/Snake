#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <curses.h>
#include <fcntl.h>

int score = 0;                                                                // Store score of current game
int highScore = 0;                                                            // Store overall high score
int direction = 0;                                                            // Store the direction of the snake
int centerX = 0;                                                              // Store center of screen in the x axis
char credits[] = "Created by: Michael Santoro, Bradley Morales, Ryan Tyimok"; // String to hold credits
int halfPerimeter = 0;                                                        // Store value of half the perimeter for win condition
bool gameOver = false;                                                        // Flag for if game over conditions are met
bool win = false;                                                             // Flag for if player has won
int fd;                                                                       // File descriptor for load/change function
char loadbuffer[1024];                                                        // Buffer for load function
char changebuffer[1024];                                                      // Buffer for update function
unsigned int bytes_written;                                                   // Used to write to into file
unsigned int bytes_read;                                                      // Used to read the file

// Struct for each "Node" of the snake
struct snakeNode{
    int x,y;
};

// Struct for the snake itself
// "List of snake nodes"
struct snake{
    int length;
    struct snakeNode node[256];
};

//Struct for trohpies
struct trophy{
    int x;
    int y;
    time_t timePlaced;                      // Time that the trophy was placed on the screen
    int expirationTime;                     // The amount of seconds the trophy stays on screen before expiring
    int amount;                             // Value of the trophy / How many lenghths are added to the snake when eaten
};

struct snake snake;                         // Creating the Snake
struct trophy trophy;                       // Create the Trophy

void refreshBoard();
void initGame();
void moveSnake();
void moveTrophy();
bool isGameOver();
bool promptRestart();
void loadHigh();
void changeHigh();

// Primary Author: Michael Santoro
// Initialize curses, colors, make calculations for win condtion, and logic for restarting the game
int main(){
    bool restart = true;                    // Flag for restarting game
    srand(time(NULL));                      // Seed for random

    initscr();                              // Initialize curses library
    start_color();                          // Start color functionality
    init_pair(1, COLOR_GREEN, COLOR_BLACK); // Initialize color pair 1 to green with black background (For Snake Body)
    init_pair(2, COLOR_RED, COLOR_BLACK);   // Initialize color pair 2 to red with black background (for Snake Head)
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);// Initialize color pair 3 to yellow with black background (for Trophy)
    init_pair(4, COLOR_CYAN, COLOR_BLACK);  // Initialize color pair 4 to cyan with black backgound (for Border)
    halfPerimeter = (COLS + LINES) - 8;     // Calculate half the perimeter and store in halfperimeter (-8 at the end due to border size)
    centerX = COLS/2;                       // Calculate center of screen on the x axis
                      
    loadHigh();

    // Determine if the player wants to restart the game after winning or losing
    while(restart){
        direction = rand() % 4 + 1;         // Give the snake a random direction
        initGame();                         // Initialize the game

        restart = promptRestart();
    } 

    changeHigh();
    endwin();                               // Turn off curses library
    return 0;
}

// Primary Author: Ryan Tyimok
// Moves the snake by moving the head using deltaCol and deltaLines, then the rest of the snake
// Then Refreshes the board to print the new snake
void moveSnake(){
    int deltaCol; //Chanage in the X-axis 
    int deltaLines; //Change in the Y-axis 

    // Determine the deltaCol and deltaLines based on the current direction
    // Direction flag was given to us in InitGame()
    switch(direction){
        case 0: // No Direction (Used for When Game is over)
            deltaCol = 0;
            deltaLines = 0;
            break;
        case 1: // UP, move head of snake (-1, 0)
            deltaCol = -1; 
            deltaLines =0;
            break;
        case 2: // Down, move head of snake (1, 0)  
            deltaCol = 1;
            deltaLines = 0;
            break;
        case 3: // Left, move head of snake (0, -1)
            deltaCol = 0;
            deltaLines =-1;
            break;
        case 4: // Right, move head of snake (0, 1)
            deltaCol = 0;
            deltaLines = 1;
            break;
    }

    // Moves the body of the Snake up by 1
    //interates thorugh the snake moving the body part up 1 as it goes through 
    for(int i = snake.length - 1; i > 0; i--) {
        snake.node[i] = snake.node[i-1];
    }
    
    // Moves the head of the Snake based on deltaCol and deltaLines
    snake.node[0].x += deltaCol;
    snake.node[0].y += deltaLines;


    // Increase the Score, Length, and move the Trophy when "eaten" by the snake
    if(snake.node[0].x == trophy.y && snake.node[0].y == trophy.x){

        //Loop until trophy.amount is no longer greater that i 
        for(int i = 0; i < trophy.amount; i++){
            snake.node[snake.length] = snake.node[snake.length-1];      //Add new part to snake(node) to the snake 
            snake.length ++;                                            //Increment the snakes length counter
            score++;                                                    //Increment players score counter 
        }

        moveTrophy();                                                   //Move trophy because it was "eaten"

        //compare current score to high score 
        //set highscore to score if its greater 
        if(score > highScore)
            highScore = score;
    }

    refreshBoard(); // Calls refreshBoard() to show snakes new postion
}

// Primary Author: Ryan Tyimok
// Moves the Trophy to a random part of the screen
void moveTrophy(){
    int trophymaxX = COLS-4;                    //borders of trophy spawn X axis 
    int trophymaxY = LINES-4;                   //borders of trophy spawn Y axis 
    trophy.expirationTime = (rand() % 9) + 1;   //rand trophy spawn in time attatched to instance of Trophy trophy 
    trophy.timePlaced = time(NULL);             //records time placed using time(NULL) that records current time when called 
    trophy.amount = (rand() % 9) + 1;           //rand amount given to trophy attatched to instance of Trophy trophy 
    bool occupied = false;                      //boolean variable to avoid trophy spawning in part of snake 

    // Detection if selected spot for trophy is occupied by the snake itself
    // Detection Written by Bradley Morales, Random X/Y Value Logic by Ryan Tyimok
    do{
        occupied = false;                                   //set occupied to false assuming spawn in is valid
        trophy.x = 3 + (rand() % ((trophymaxX - 3) + 1));   //giving trophy.x coords a rand() spawn within its range + 1
        trophy.y = 3 + (rand() % ((trophymaxY - 3) + 1));   //giving trophy.y coords a rand() spawn within its range + 1

        //Iterates through parts of the snake(nodes) using a for-loop 
        for(int i = 0; i < snake.length; i++){
            //if coords of the part of the snake(node) == the coords of the trophy  
            if(snake.node[i].x == trophy.x && snake.node[i].y == trophy.y){
                occupied = true;                      //set occupied equals true so it can go back in loop to find new location
                break;                               //break from loop because you dont have to continue looping through snake 
            }
 
        }
    } while(occupied); 
}

// Primary Author: Michael Santoro
// Initialize the game, reset variables, and print border/credits
void initGame(){
    score = 0;                            // Reset score
    gameOver = false;                     // Reset game over flag
    win = false;                          // Reset the win flag
    clear();                              // Reset the Screen
    curs_set(0);                          // Hide the Cursor
    noecho();                             // Do not echo user input to the screen
    nodelay(stdscr,TRUE);                 // Turn on No Delay
    keypad(stdscr,TRUE);                  // Enable arrow keys

    snake.length = 5;                     // Reset the Snake Length
    snake.node[0].x = COLS/2;             // Set Starting X Position
    snake.node[0].y = LINES/2;            // Set Starting Y Position

    moveTrophy();                         // Initialize Trophy to a random spot on the screen

    int maxSpeed = 120000;                // Maximum Speed
    int minSpeed = 65000;                 // Minumum Speed
    int speed = 0;                        // Speed of Snake (Initialized to 0, set to max, min, or increment when moving)

    int startX = snake.node[0].y;
    int startY = snake.node[0].x;

    // Primary Author: Bradley Morales  
    // Initialize position of snake based off starting direction
    for (int i = 0; i < snake.length; i++) {
        switch (direction) {
            case 1: // Up
                snake.node[i].x = startX + i;
                snake.node[i].y = startY;
                break;
            case 2: // Down
                snake.node[i].x = startX - i;
                snake.node[i].y = startY;
                break;
            case 3: // Left
                snake.node[i].x = startX;
                snake.node[i].y = startY + i;
                break;
            case 4: // Right
                snake.node[i].x = startX;
                snake.node[i].y = startY - i;
                break;
        }
    }
    
    // Prints title and credits of game
    move(0,centerX-2);                    // Move cursor to center - offset where offset = "snake"'s length/2
    addstr("Snake");                      // Print the String
    move(1,centerX-(sizeof(credits)/2));  // Move cursor to center - offset of credits string
    addstr(credits);                      // Print the String

    // Draws the border of the board with a 1 character border
    // between the edge of the terminal and border of the snake pit
    attron(COLOR_PAIR(4));
    for(int i = 2; i < LINES - 2; i++){
        for(int j = 2; j < COLS - 2; j++){
            if(i == 2 || j == 2|| i == LINES-3 || j == COLS-3){
                move(i,j);
                printw("#");
            }
        }
    }
    attroff(COLOR_PAIR(4));

    refreshBoard();                      // Call refreshBoard

    // Primary Author: Ryan Tyimok
    // Handles User Input and changes direction and gameOver tabs as needed
    while(!gameOver){
        int ch = getch(); // Store user input in ch 
        // Primary Authors: Ryan Tyimok and Bradley Morales
        //                  Ryan: Use the user-inputted arror keys to determine direction
        //                  Bradley: Implemented detection for if snake tries to reverse itself
        switch(ch){
            case 'q': // Quit Game
                gameOver = true;
                break;
            case KEY_UP:
                if(direction == 2){
                    direction = 0;
                    gameOver = true;
                    break;
                }
                else{
                    direction = 1; //Set direction equal to 1 meaning UP
                    break;
                }
            case KEY_DOWN:
                if(direction == 1){
                    direction = 0;
                    gameOver = true;
                    break;
                }
                else{
                    direction = 2; //Set direction equal to 2 meaning DOWN
                    break;
                }
            case KEY_LEFT:
                if(direction == 4){
                    direction = 0;
                    gameOver = true;
                    break;
                }
                else{
                    direction = 3; //Set direction equal to 3 meaning LEFT
                    break;
                }
            case KEY_RIGHT:
                if(direction == 3){
                    direction = 0;
                    gameOver = true;
                    break;
                }
                else{
                    direction = 4; //Set direction equal to 4 meaning RIGHT
                    break;
                }
        }
        moveSnake();       // Call moveSnake   
        gameOver = isGameOver();
        
        //Create varible of time_t called currentTime 
        //if the difference between the current time and trophy.timeplaced >= to the trophies 
        //rand() generated exipriation time, call moveTrophy()
        time_t currentTime = time(NULL);
        if (difftime(currentTime, trophy.timePlaced) >= trophy.expirationTime) {
            moveTrophy();
        }

        // Speed Proportional to its length (incrementSpeed) with a min (minSpeed) and max (maxSpeed)
        int incrementSpeed = 1300000 / (snake.length + 6);

        // Determine if incrementSpeed is inside the bounds of minSpeed and maxSpeed
        if(incrementSpeed > maxSpeed)
            speed = maxSpeed;
        else if(incrementSpeed < minSpeed)
            speed = minSpeed;
        else
            speed = incrementSpeed;

        // Delay between frames using speed
        usleep(speed);

        // Win Condition
        if(snake.length > halfPerimeter){
            gameOver = true;
            win = true;
        }
        refreshBoard();  
    }
    nodelay(stdscr,FALSE); // Turn off no delay
    keypad(stdscr,FALSE);  // Disable arrow keys at game loss
    direction = 0;         // Set direction to 0 so snake freezes

    sleep(1);              // Wait for 1 second before exiting the game
}

// Primary Author: Michael Santoro
// Redraws the board and refreshes the screen to show changes in snake, fruits, and score
void refreshBoard(){

    // Clears the Board by replacing everything inside the pit with spaces
    for(int i = 3; i < LINES - 3; i++) {
        for(int j = 3; j < COLS - 3; j++) {
            move(i, j);
            printw(" ");
        }
    }

    // Print the Trophy
    attron(COLOR_PAIR(3));
    move(trophy.y,trophy.x);
    printw("%d",trophy.amount);
    attroff(COLOR_PAIR(3));

    // Print the Snake
    attron(COLOR_PAIR(1));
    for(int i = snake.length - 1; i >= 0; i--){
        if(i == 0){ // Head of the Snake
            attron(COLOR_PAIR(2));
            move(snake.node[i].x,snake.node[i].y);
            printw("@");
            attroff(COLOR_PAIR(2));
        }
        else{       // Body of the Snake
            attron(COLOR_PAIR(1));
            move(snake.node[i].x,snake.node[i].y);
            printw("o");
            attroff(COLOR_PAIR(1));
        }
    }
    attroff(COLOR_PAIR(1));

    // Prints the player's score
    move(LINES - 2,2);                  // Move cursor to bottom left corner
    addstr("Score:");                   // Print the score label
    printw("%d",score);                 // Print the score

    // Prints the High Score
    move(LINES - 2,COLS-15);            // Move cursor to bottom left corner
    addstr("High Score:");              // Print the score label
    printw("%d",highScore);             // Print the score

    refresh();                          // Refresh the Screen
}

// Primary Author: Bradley Morales
// Check if Snake has run into a wall
bool isGameOver(){
    int headX = snake.node[0].x;       // Get the X Coordinate of the Head and store in headX
    int headY = snake.node[0].y;       // Get the Y Coordinate of the Head and store in headY
    
    // Check if Snake has run into a wall
    if(headY >= COLS - 3 || headY <= 2 || headX >= LINES - 3 || headX <= 2){
        return true;
    }
    
    // Check if Snake has run into itself
    for (int i = 1; i < snake.length; i++) {
        if (snake.node[i].x == headX && snake.node[i].y == headY) {
            return true;
        }
    }

    return false;
}

// Primary Author: Bradley Morales
// Prompt the User to restart the game
bool promptRestart(){
    int ch;

    if(win){  // If the player has won
        move(10,centerX-12);
        printw("Congrats! You Have Won!");
        move(11,centerX-8);
        printw("Play Again? (y/n)");
    }
    else{     // If the player has lost
        move(10,centerX-8);
        printw("Sorry! You Lost!");
        move(11,centerX-8);
        printw("Play Again? (y/n)");
    }
    refresh();

    while(true){
        ch = getch();
        if(ch == 'y' || ch == 'Y')
            return true;
        else if(ch == 'n' || ch == 'N')
            return false;
    }
}

// Primary Author: Ryan Tyimok
// load the high score from highscore.txt
void loadHigh(){
    //Opens txt to load Highscore from
    fd = open("highscore.txt", O_RDONLY);               //Opens it with Read-Only permissions 
    if(fd == -1){
        fd = creat("highscore.txt", 0644);              //If file doesnt exist create file with rw-r--r-- permisions (lect9-slide6)
        if (fd == -1) {
            perror("Couldnt load high");                     //Error check using perror 
            return; 
        }  
        close(fd);        
    }
    fd = open("highscore.txt", O_RDONLY);              //reopen file with read only permissions 
    bytes_read = read(fd, loadbuffer, sizeof(loadbuffer));  //read into loadbuffer from Highscore.txt
    highScore = atoi(loadbuffer);                           //Converts contents of Highscore.txt to integer and sets highscore equal to it 
    close(fd);                                              //Closes Highscore.txt
}

// Primary Author: Ryan Tyimok
// update highscore in file
void changeHigh(){
    //Opens txt to update Highscore 
    fd = open("highscore.txt", O_WRONLY);                      //Opens it with Write-Only permissions 
    if(fd == -1){           
        perror("Couldnt load high");                                //Error check using perror
        return;
    }
    char changebuffer[1024];                                        //Create a buffer to write from called "changebuffer"
    sprintf(changebuffer, "%d", highScore);                         //Converts contents in highscore to string and stores into "changebuffer"
    bytes_written = write(fd, changebuffer, sizeof(changebuffer));  //Write to file from changebuffer
    close(fd);                                                      //Closes Highscore.txt
}
