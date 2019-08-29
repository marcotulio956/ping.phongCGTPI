#include <stdlib.h>
#include <stdio.h>
#include <glew.h>
#include <freeglut.h>
#include <math.h>
#include <string.h> //numero de lados para uma string
#define PI 3.14159265359
#define RADIUS 400
typedef struct barrier {
	float x;
	int points;
	int pwins;
}barrier;
typedef struct ball {
	int posx,posy;
	int vx,vy;
	int crossed;
}ball;
barrier player1;
barrier player2;
ball pomo;
void startScene() {
	pomo.posx = 300;
	pomo.posy = 485;
	pomo.vx = 10;
	pomo.vy = 20;
	player1.x = 250;
	player2.x = 250;
	glClearColor(0.1, 0.1, 0.1, 1);
}
void sidesCounterDisplay() { //Para exibir a quantidade de lados da figura atual 
	char string[100];
	sprintf_s(string, 100, "pomo.x: %d pomo.y: %d\npomo.vx: %d pomo.vy: %d", pomo.posx, pomo.posy,pomo.vx,pomo.vy);//coloca o int na string

	glRasterPos2f(0,50); //onde o texto aparece
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, string);
}

void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT);
	
	//glutFullScreen(); 
	glColor3f(0.63, 0.42, 0.12);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBegin(GL_POLYGON);//quadro
		glVertex3f(75, 121.25, -0.5);
		glVertex3f(525, 121.25, -0.5);
		glVertex3f(525, 848.75, -0.5);
		glVertex3f(75, 848.75, -0.5);
	glEnd();
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
	glLineWidth(10);
	//glEnable(GL_LINE_SMOOTH);

	glColor3f(1, 0, 0);//player 1
	glBegin(GL_LINES);
		glVertex3f(player1.x   ,130,0);
		glVertex3f(player1.x+100,130,0);
	glEnd();
	
	glColor3f(0, 1, 0);//player 2
	glBegin(GL_LINES);
		glVertex3f(player2.x   ,840,0);
		glVertex3f(player2.x+100,840,0);
	glEnd();
	
	glPointSize(10);
	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	glBegin(GL_POINTS);
		glColor3f(1, 1, 1);
		if (pomo.posy <= 145) {
			pomo.vy = pomo.vy * (-1);
		}
		if (pomo.posy >= 825) {
			pomo.vy = pomo.vy * (-1);
		}
		if (pomo.posx <= 80 || pomo.posx >= 520) {
			pomo.vx *= -1;
		}
		pomo.posx += pomo.vx;
		pomo.posy += pomo.vy;
		glVertex3f(pomo.posx,pomo.posy,0);
		glVertex3f(600, 400, 1);
	glEnd();

	sidesCounterDisplay();

	glFlush();
}
void reSize(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 600 , 0, 970, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void kb_event(unsigned char key, int x, int y) {
	switch (key) {
		case 27: {
			exit(0);
			break;
		}
		case 'd': {
			if ((player1.x-13) >= 75) {
				player1.x = player1.x - 13;
			}
			glutPostRedisplay();
			break;
		}
		case 'f': {
			if (player1.x + 100 + 13 <= 523) {
				player1.x = player1.x + 13;
			}
			glutPostRedisplay();
			break;
		}
		case 'j': {
			if ((player2.x - 13) >= 75) {
				player2.x = player2.x - 13;
			}
			glutPostRedisplay();
			break;
		}
		case 'k': {
			if (player2.x+100+13<=523) {
				player2.x = player2.x + 13;
			}
			glutPostRedisplay();
			break;
		}
		default: {
			break;
		}
	}
}
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitContextVersion(1, 1);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(600, 970);
	glutInitWindowPosition(0, 0);

	glutCreateWindow("projeto");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(reSize);
	glutKeyboardFunc(kb_event);

	startScene();

	glutMainLoop();
}
