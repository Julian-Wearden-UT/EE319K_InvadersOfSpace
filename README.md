# EE319K Introduction to Embedded Systems Final Project

## Academic Integrity Disclaimer

*This repository was uploaded as documentation of work completed by our team in EE319K. Students that are currently enrolled in the course are not authorized to view the contents of this repository at risk of violating the University of Texas Honor Code. If you would like more information about this project, please email me at julianwearden@utexas.edu*

## Project Description
### Objectives
1. To design, test, and debug a large C program
2. To review I/O interfacing techniques
3. To design a system that performs a useful task

### Overview
This project is an embedded system that allows users to play our copy of Space Invaders developed in C. Using the slide potentiometer to move the spaceship left and right, players shoot incoming enemy alien ships. The second button will pause/resume the game. Enemy ships are worth a variety of points, and the total sum will be kept track of and displayed as the score at the end of the game. The game ends when an enemy ship reaches the bottom of the screen. 

### Features
Two buttons: one to shoot at the enemy and one to pause/resume game
Slide potentiometer: to move the ship left and right
Enemy ships and player’s ship are sprites
Shooting sounds and music
Score will be kept based on enemy ships destroyed 
Languages: English and Spanish

### Project Requirements
Two buttons: shoot, pause/resume
Slide pot: move left/right
3 Sprites: player ship, enemy ships, and bullets
Shooting sound created when the user shoots
Three ISRs: one to shoot bullets (edge-triggered interrupt), one to move the ship (periodic SysTick interrupt), and one to pause the game (periodic interrupt, highest priority)
The score is displayed at the end of the game
Language options (English and Spanish) are displayed at the beginning of the game

## Project Video Submission
The following is a short promotional video showcasing our product for the classroom wide design competition at the end of the year:
https://youtu.be/YG223pXihJg

*Disclaimer: This video was made during the beginning of the COVID-19 pandemic, when I had maybe a little too much time on my hands*

##
###### This project was completed at UT Austin in the Spring of 2019 and approved for GitHub publishing by:
###### Julian Wearden • Tuan (Anthony) Pham


