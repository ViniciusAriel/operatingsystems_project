# Terminal Executor - Operating Systems and Exercises - Final Project

This project implements a server and player code for a "Hangman's Game". It can be played by multiple people together.

## Game Description

In Hangman's Game, there is a secret word that the players must guess. The only information available from the start is the number of letters in the word.

Each player can guess a letter. If the letter is not contained in the secret word, a life is lost. If the letter is contained in the secret word, every position in which this letter appear in is revealed.

The players win if they are able to guess all the letters of the secret word. Alternatively, they lose if they run out of lives. The lives are always shared between the players;

## How this project works

In our code, we chose to give the players 6 lifes. Also the secret word is always 6 letters long.

When a player code is executed, it tries to connect to a server. If it is able to, it can start sending guesses. The current information know about the secret word (the letters that were already discovered) is send to all connected players everytime a guess is made. Also, the guess is shared between the players.

When the players either win or lose, the game is considered over and the server socket is closed.

## How to compile

> gcc -o hangman_server hangman_server.c -pthread

> gcc -o hangman_player hangman_player.c -pthread

## How to play

First, initialize the server
> ./hangman_server

With the server is properly running in the port, initialize the players one at a time
> ./hangman_player

Once both are running, it's possible to send each letter by the terminal to make a guess.
