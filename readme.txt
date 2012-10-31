
/**
 *
 * @author praveen_kulkarni
 */

#1 check out the files and execute follow instructions
sudo apt-get install freeglut3 freeglut3-dev
sudo apt-get install binutils-gold
g++ -lGL -lglut -lm -lGLU display.cpp -o test
./test

#2 controls
F1              - for full screen
Esc             - to exit application
'p'             - decrease particle size
Shift+'p'       - increase particle size
right arrow key - step through simulation
Shift+'>'       - to visualise mapping of clusters on points forming a cylinder
Shift+'<'       - decrease the rings used for mapping

