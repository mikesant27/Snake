# Snake
## Description:
Snake game written in C using the curses library.

## Controls
- [x] Left Arrow - Turn Snake Left
- [x] Right Arrow - Turn Snake Right
- [x] Up Arrow - Turn Snake Up
- [x] Down Arrow - Turn Snake Down

## Project Requirements:

### The Snake:
- [x] The initial length of the Snake is three characters
- [x] The initial direction of the Snake's movement is random
- [x] The User can press the arrow keys to change the direction of the snake
- [x] The Snake's speed is proportional to its length

### The Snake Pit:
- [x] The Snake Pit is the area where the Snake can move.
- [x] The Snake Pit utilizes all available space of the terminal window
- [x] The Snake Pit has a visible border  

### The Trophies / Fruit:
- [x] Trophies are represented by a digit randomly chosen from 1 to 9
- [x] When the Snake eats a Trophy, its length is increased by the corresponding value of the Trophy
- [x] There is always exactly one Trophy in the Snake Pit at a given moment
- [x] A Trophy expired after a random interval of 1 to 9 seconds
- [x] A new Trophy is moved to a random location in the Snake Pit if the Trophy has expired, or was eated by the snake
- [x] Trophies cannot spawn inside the Snake 

### Gameplay:
The Snake dies and the game ends if:
- [x] The Snake runs into the border; or
- [x] The Snake runs into itself; or
- [x] The Player attempts to reverse the Snake's direction
The Snake wins if:
- [x] The Snake's length is greater than or equal to half of the perimeter of the Snake Pit

## Score:
- [x] The Player's score is equal to the sum of all the Trophies and Snake eats
- [x] When the game ends, if a new high score has been achieved, it will be stored in highscore.txt
- [x] When the game opens, the high score will be pulled from highscore.txt

## Replaying the Game:
- [x] When the Snake dies, the Player will be prompted to play again
- [x] If the Player would like to play again, press the 'Y' key and the game will reset
- [x] If the Player would not like to play again, press the 'N' key and the game will exit to the terminal 
