# project_mario_vollyball
This project has 3 main part: game_init, game_run and game_destroy.
1. game_init:
   Install keyboard, mouse, timer, font etc.,so that we can use these specific features in this game.
   If any error occurs during installation, we should also be notified.
2. game_destroy:
   Use allegro_destroy to destroy those features installed before so that the unused part do not occupied memory space.
3. game_run:
   This is the most important part of the whole project. It deals with the state transitions and the logic of how the game works.
   state 0: a intro image in which the player need to press enter to enter the main menu.
   state 1: In this state, the player can select either one of 2 mode of game playing, help, or exit by the mouse.
   state 2: The first game play mode which is the 2 player mode. This part deals with the how the volleyball interact with those two characters like event of collision.
            This part also control the logic of how fast the ball move in x-direction or y-direction.
   state 3: The second play mode, which is the practice mode(one player). Most of this game play mode is the same as the first one except that the second character will move by itself instead of moving by a second player.
   state 4: This is the "help" mode. It will show how to play this game on the screen.
   state 5 and state 6: the winning image of either player.
   state 8: The image shown if exited.
   state 9: the "pause" state.
![image](https://github.com/hwyang/project_mario_vollyball/blob/master/volleyball_gameplay.gif)
