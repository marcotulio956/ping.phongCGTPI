//version 1.6
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <glew.h>
#include <time.h>
#include <freeglut.h>
#include <string.h>
#include <SOIL.h>
#pragma comment( lib,  "SOIL.lib");
//#pragma comment ( lib, "SDL2.lib")
#define VK_W 0x57
#define VK_S 0x53
#define VK_O 0x4F
#define VK_L 0x4C
//#define VK_SCAPE 0x1B
#define MENU 1
#define START 2
#define OPTIONS 3
#define LEADERBOARD 4
#define IMOUT 5
#define PAUSE 6
#define w_width 600
#define w_height 970
#define P1 1
#define P2 2
#define CPU 3
typedef struct barrier {
	float x;
	float vx;
	int points;
	int sets;
	int maxP;
	GLuint barrierTexture;
}barrier;
typedef struct ball {
	float posx, posy;
	float vx, vy;
	int crossed;
	GLuint pomoTexture;
}ball;
typedef struct mice {
	int posx;
	int posy;
	int	button;
	int state;
}mice;
typedef struct buttonBox {//nao ha texturas aqui, pois estao no background
	int posx;
	int posy;
	float width;
	float height;
}buttonBox;
typedef struct tex_or_sprite {
	GLuint id;
	int posx;
	int posy;
	int height;
	int width;
	int qntFrames;
	int atualFrame;
}tex_or_sprite;

barrier player1, player2;//player 1 eh o 'de baixo'
ball pomo;
mice mouse;

buttonBox start, options, leaderboard, imout, goback;// go back era para ser um botao voltar, mas estou sem tempo de insirar coordenadas para-o
buttonBox dif_normal, dif_fast, dif_asian;//aumenta tanto a velocidade da bolinha quanto a dos players
buttonBox set3, set7, set9, set11;
buttonBox p1p2, p1cpu;

tex_or_sprite startBackGroundSprite, pomoSprite;

int RIVAL = CPU;
int MODE = MENU;//sempre inicia no menu
int previousMODE;
int frameCounter=0;
int dif = 3; //padrão 3, rapido 5, asian 7
float fps = 1000 / 120;
int qntSets = 3;//passivel a ser alterado em opcoes (em jogo)
GLuint leaderboardTexture, menuTexture, pauseTexture, optionsTexture, menuSTexture;


GLuint putTexID(char name[50]) {//so devolve o id
	GLuint tex = SOIL_load_OGL_texture(name, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (tex == 0) {
		printf("SOIL error: '%s'\n", SOIL_last_result());
	}
	return tex;
}
void putTex_or_SpriteWC(tex_or_sprite tos, int posx, int posy) {//coloca texturas ou sprites com coordenadas
	if (tos.qntFrames != 1) {//se for uma animacao
		//pinta animacao
	}
	else {//se for apenas textura com sua coordenada
		//pinta tex
	}
}
void putBackground(GLuint texName) {//coloca todo o fundo como tex
	glClear(GL_TEXTURE_2D);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	{
		glBindTexture(GL_TEXTURE_2D, texName);
		glBegin(GL_TRIANGLE_FAN);
		{
			glTexCoord2f(0, 0);
			glVertex2f(0, 0);

			glTexCoord2f(1, 0);
			glVertex2f(w_width, 0);

			glTexCoord2f(1, 1);
			glVertex2f(w_width, w_height);

			glTexCoord2f(0, 1);
			glVertex2f(0, w_height);
		}
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
	glutSwapBuffers();
}
void wInScreen(char* string, int posx, int posy) {
	glRasterPos2f(posx, posy); //onde o texto aparece
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, string);
}
void infoDebug() { //informacoes sobre posicoes do pomo em START SOMENTE
	char string[100];
	sprintf_s(string, 100, "pomo.x: %0.2f pomo.y: %0.2f\npomo.vx: %0.2f pomo.vy: %0.2f\np1x:%d p2x:%d", pomo.posx, pomo.posy, pomo.vx, pomo.vy,(int)player1.x,(int)player2.x);//coloca o int na string

	wInScreen(string, w_width * 0.28, w_height * 0.091);
}
void showStatistics() {// em START SOMENTE
	char PlayerXStatistics[75];

	sprintf_s(PlayerXStatistics, 75, "Points %d\nSets %d\n", player1.points, player1.sets);//coloca o int na string
	wInScreen(PlayerXStatistics, 0, w_height * 0.103);

	sprintf_s(PlayerXStatistics, 75, "Points %d\nSets %d\n", player2.points, player2.sets);
	wInScreen(PlayerXStatistics, 0, w_height * 0.974);
}

void resetPomoPos() {
	pomo.posx = (rand() % (int)(0.75 * w_width)) + 0.125 * w_width;//aleatorio em um certo range
	pomo.posy = w_height / 2;
	pomo.vx = 2 * dif; //frenetico melhores valores
	pomo.vy = 2.5 * dif * ((int)rand() % 2 == 1 ? 1 : -1);//aleatorio em direc y
}
void resetPlayers() {
	player1.x = 250;
	player2.x = 250;
	player1.points = 0;
	player2.points = 0;
	player1.sets = 1;
	player2.sets = 1;
	player1.vx = 2 * dif;
	player2.vx = 2 * dif;
	player1.maxP = 0;
	player1.maxP = 0;
}
void setButtonsAssets() {
	start.posx = 185;
	start.posy = 580;
	start.width =175;
	start.height = 86;

	options.posx = 150;
	options.posy = 445;
	options.width = 275;
	options.height = 105;

	leaderboard.posx = 110;
	leaderboard.posy = 350;
	leaderboard.width = 365;
	leaderboard.height = 80;

	imout.posx = 215;
	imout.posy = 167;
	imout.width = 150;
	imout.height = 64;

	dif_normal.posx = 214;
	dif_normal.posy = 730;
	dif_normal.width = 174;
	dif_normal.height = 27;;

	dif_fast.posx = 242;
	dif_fast.posy = 672;
	dif_fast.width = 116;
	dif_fast.height = 43;

	dif_asian.posx = 221;
	dif_asian.posy = 612;
	dif_asian.width = 149;
	dif_asian.height = 44;

	p1p2.posx = 168;
	p1p2.posy = 429;
	p1p2.width = 263;
	p1p2.height = 44;

	p1cpu.posx = 166;
	p1cpu.posy = 371;
	p1cpu.width = 276;
	p1cpu.height = 41;

	set3.posx = 185;
	set3.posy = 176;
	set3.width = 27;
	set3.height = 43;

	set7.posx = 242;
	set7.posy = 176;
	set7.width = 29;
	set7.height = 42;

	set9.posx = 300;
	set9.posy = 176;
	set9.width = 31;
	set9.height = 40;

	set11.posx = 359;
	set11.posy = 176;
	set11.width = 56;
	set11.height = 41;
}
void setTextures_and_Sprites() {

	menuTexture = putTexID("menub.png");//tex
	menuSTexture = putTexID("menubS.png");
	pauseTexture = putTexID("pause.png");
	optionsTexture = putTexID("options.png");
	leaderboardTexture = putTexID("leaderboard.png");

	startBackGroundSprite.id = putTexID("fundo.png");//fumacinha

}

void startScene() {//inicilizacao geral
	resetPomoPos();
	resetPlayers();

	setButtonsAssets();
	setTextures_and_Sprites();
}
int vBoxEmBotao(buttonBox botao, int x, int y) {//verifica hitbox
	if (   (x <= (botao.posx + botao.width)) && (x >= (botao.posx))
		&& (y <= (botao.posy + botao.height)) && (y >= (botao.posy))) {
		printf("clickou em botao QUALQUER %d %d %d %d\n", mouse.posx, mouse.posy, mouse.button, mouse.state);
		return 1;
	}
	return 0;
}
void mousesao(int button, int state, int x, int y) {
	mouse.button = button;
	mouse.state = state;
	mouse.posx = x;
	mouse.posy = w_height - y;
	printf("click posx: %d posy: %d state: %d button: %d\n", mouse.posx, mouse.posy, mouse.state, mouse.button);
	if (MODE == MENU) {
		//if (vBoxEmBotao(start, mouse.posx, mouse.posy)) {//colocar em passivemotion
			//p/rintf("mouse em cima de START %d %d \n", mouse.posx, mouse.posy);
			//putBackground(menuSTexture);
			//glutPostRedisplay();
		//}
		if (((mouse.button == 0) && (mouse.state == 0)) && (vBoxEmBotao(start, mouse.posx, mouse.posy))) {//verifica start
			printf("clickou em START %d %d \n", mouse.posx, mouse.posy);
			MODE = START;
			glutPostRedisplay();
		}
		if (((mouse.button == 0) && (mouse.state == 0)) && (vBoxEmBotao(options, mouse.posx, mouse.posy))) {//verifica options
			printf("clickou em OPTIONS %d %d \n", mouse.posx, mouse.posy);
			MODE = OPTIONS;
			glutPostRedisplay();
			
		}
		if (((mouse.button == 0) && (mouse.state == 0)) && (vBoxEmBotao(leaderboard, mouse.posx, mouse.posy))) {//verifica leaderboard
			printf("clickou em LEADERBOARD %d %d \n", mouse.posx, mouse.posy);
			MODE = LEADERBOARD;
			glutPostRedisplay();
		}
		if (((mouse.button == 0) && (mouse.state == 0)) && (vBoxEmBotao(imout, mouse.posx, mouse.posy))) {//verifica sair
			printf("quitou %d %d \n", mouse.posx, mouse.posy);
			exit(0);
		}
	}
	/*if (MODE == START || MODE == OPTIONS || MODE == LEADERBOARD) { // colocar botao de voltar
		if (((mouse.button == 0) && (mouse.state == 0)) && (vBoxEmBotao(goback, mouse.posx, mouse.posy))) {
			printf("clickou em goback %d %d", mouse.posx, mouse.posy);
			MODE = previousMODE;
		}
	}*/
	if (MODE == OPTIONS) {
		if (((mouse.button == 0) && (mouse.state == 0)) && (vBoxEmBotao(dif_normal, mouse.posx, mouse.posy))) {
			dif = 3;
			printf("setou dif: %d\n", dif);
		}
		if (((mouse.button == 0) && (mouse.state == 0)) && (vBoxEmBotao(dif_fast, mouse.posx, mouse.posy))) {
			dif = 5;
			printf("setou dif: %d\n", dif);
		}
		if (((mouse.button == 0) && (mouse.state == 0)) && (vBoxEmBotao(dif_asian, mouse.posx, mouse.posy))) {
			dif = 7;
			printf("setou dif: %d\n", dif);
		}
		if (((mouse.button == 0) && (mouse.state == 0)) && (vBoxEmBotao(p1p2, mouse.posx, mouse.posy))) {
			RIVAL = P2;
			printf("setou riv: %d\n", RIVAL);
		}
		if (((mouse.button == 0) && (mouse.state == 0)) && (vBoxEmBotao(p1cpu, mouse.posx, mouse.posy))) {
			RIVAL = CPU;
			printf("setou riv: %d\n", RIVAL);
		}
		if (((mouse.button == 0) && (mouse.state == 0)) && (vBoxEmBotao(set3, mouse.posx, mouse.posy))) {
			qntSets = 3;
			printf("setou sets 3: %d\n", qntSets);
		}
		if (((mouse.button == 0) && (mouse.state == 0)) && (vBoxEmBotao(set7, mouse.posx, mouse.posy))) {
			qntSets = 7;
			printf("setou sets 7: %d\n", qntSets);
		}
		if (((mouse.button == 0) && (mouse.state == 0)) && (vBoxEmBotao(set9, mouse.posx, mouse.posy))) {
			qntSets = 9;
			printf("setou sets 9: %d\n", qntSets);
		}
		if (((mouse.button == 0) && (mouse.state == 0)) && (vBoxEmBotao(set11, mouse.posx, mouse.posy))) {
			qntSets = 11;
			printf("setou sets 11: %d\n", qntSets);
		}
	}
}
void intoMenu() {/*
	unsigned char string2[] = "START GAME";
	unsigned char string3[] = "OPTIONS";
	unsigned char string4[] = "CREDITS";
	unsigned char string5[] = "LEADER BOARD";*/;
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//player1.idTextureBarrier = carregaTextura("p1");
	//welcomeTexture = putTexID("wc.png");
	//glColor3f(0.4, 0.1, 1);
	//implentar aqui as coisas para texturizar o menu
	//glDisable(GL_TEXTURE_2D);
	/*
	glRasterPos2f(300, 800); //onde o texto aparece
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, string2);

	glRasterPos2f(300, 700); //onde o texto aparece
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, string3);

	glRasterPos2f(300, 600); //onde o texto aparece
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, string4);

	glRasterPos2f(300, 500); //onde o texto aparece
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, string5);
	*//*
	glColor3f(0.1, 0.5, 0.9);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//para preencher todo o quadro

	glBegin(GL_POLYGON);//quadro
	glVertex3f(75, 121.25, -0.5);
	glVertex3f(525, 121.25, -0.5);
	glVertex3f(565, 485, -0.5);//extremo direito
	glVertex3f(525, 848.75, -0.5);
	glVertex3f(75, 848.75, -0.5);
	glVertex3f(35, 485, -0.5);//extremo esquerdo
	glEnd();
	*/
	putBackground(menuTexture);
	//glutSwapBuffers();
}
void intoStart() {

	/*glPushMatrix();                 // Importante!! //gg coutinho
		glTranslatef(nave.x, nave.y, 0);
		glBegin(GL_TRIANGLE_FAN);
			glVertex2f(-nave.larg / 2, -nave.alt / 2); // v3---v2
			glVertex2f(nave.larg / 2, -nave.alt / 2); // |     |
			glVertex2f(nave.larg / 2, nave.alt / 2); // |     |
			glVertex2f(-nave.larg / 2, nave.alt / 2); // v0---v1
		glEnd();
	glPopMatrix();//TRANSLACOES PARA POSICIONAMENTO*/
	if (MODE == START) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//para preencher todo o quadro
		//glColor3f(0.63, 0.42, 0.12); dourado
		glColor3f(0.1, 0.1, 0.1);

		//tirar o quadro e substituir por uma textura aproveitando a posicao atual

		glBegin(GL_POLYGON);//quadro
		{
			glVertex3f(75, 121.25, 0);
			glVertex3f(525, 121.25, 0);
			glVertex3f(525, 848.75, 0);
			glVertex3f(75, 848.75, 0);
		}
		glEnd();

		//tirar as barras e substituir por texturas aproveitando as posicoes
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);//para fazer as barras
		glLineWidth(10);

		glColor3f(1, 0, 0);//player 1 é uma linha grossa
		//glEnable(GL_LINE_SMOOTH); //antialising caso sejam inclinadas

		glBegin(GL_LINES);//player 1
		{
			glVertex3f(player1.x, 0.134 * w_height, 0); //130
			glVertex3f(player1.x + 100, 0.134 * w_height, 0);
		}
		glEnd();

		glColor3f(0, 1, 0);//player 2

		glBegin(GL_LINES);
		{
			glVertex3f(player2.x, 0.865 * w_height, 0); //840
			glVertex3f(player2.x + 100, 0.865 * w_height, 0);
		}
		glEnd();

		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);//pomo
		glPointSize(10);
		glColor3f(1, 1, 1);

		glBegin(GL_POINTS);//projetar pomo
		{
			glVertex3f(pomo.posx, pomo.posy, 0);
		}
		glEnd();

	}
	if (MODE == PAUSE) {
		//aparece pause
		putBackground(pauseTexture);
	}
	
	infoDebug();//comentar na versao final

	showStatistics();//mostrar pontuacoes
	glutSwapBuffers();
}
void intoOptions() {
	putBackground(optionsTexture);
}
void intoLeaderboard() {
	putBackground(leaderboardTexture);
}
void mainFrame() {
	printf("modo: %d\n", MODE);
	glClear(GL_COLOR_BUFFER_BIT);
	//glColor4f(1, 1, 1, 1);
	switch (MODE) {
		case MENU: {
			intoMenu();
			//glutPostRedisplay();
			break;
		}
		case START: {
			intoStart();
			//glutPostRedisplay();
			//se 'pause' ent aparece imagemzinha no intoStart
			break;
		}
		case OPTIONS: {
			intoOptions();
			//glutPostRedisplay();
			break;
		}
		case LEADERBOARD: {
			intoLeaderboard();
			//glutPostRedisplay();
			break;
		}
		default: {
			printf("which/mode_: %d\n",MODE);
		}
	}
}
void reshape(int width, int height) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 600, 0, 970, -1, 1);
	float razaoAspectoJanela = ((float)width) / height;
	float razaoAspectoMundo = ((float)600) / 970;
	// se a janela está menos larga do que o mundo (16:9)...
	if (razaoAspectoJanela < razaoAspectoMundo) {
		// vamos colocar barras verticais (acima e abaixo)
		float hViewport = width / razaoAspectoMundo;
		float yViewport = (height - hViewport) / 2;
		glViewport(0, yViewport, width, hViewport);
	}
	// se a janela está mais larga (achatada) do que o mundo (16:9)...
	else if (razaoAspectoJanela > razaoAspectoMundo) {
		// vamos colocar barras horizontais (esquerda e direita)
		float wViewport = ((float)height) * razaoAspectoMundo;
		float xViewport = (width - wViewport) / 2;
		glViewport(xViewport, 0, wViewport, height);
	}
	else {
		glViewport(0, 0, width, height);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void kb_pressed(unsigned char key) {
	switch (key) {
		case 8: {//backspace, func d voltar
			if (MODE != MENU) {
				MODE = MENU;
			}
			printf("go back MODE: %d\n", MODE);
		 	glutPostRedisplay();
			break;
		}
		case 'P':
		case 'p': {
			//glutTimerFunc(10000, drawScene, 0);
			/*
			if (fps !=(int)1000/30) {
				fps = 5;
				printf("jogo foi pausado\n");
			}
			else if(fps==(int)1000/30){
				printf("jogo foi des-pausado\n");
				fps = 1000 / 120;
			}*/
			if (MODE == START) {
				MODE = PAUSE;
			}
			else if (MODE == PAUSE) {
				MODE = START;
			}
			glutPostRedisplay();
			break;
		}
		case 'R':
		case 'r': {
			startScene();
			break;
		}
		case 27: {
			exit(0);
			break;
		}
		default: {
			break;
		}
	}
}
void mov_p1_logic() {
	if (GetAsyncKeyState(VK_W)) {
		if ((player1.x - player1.vx) >= 75) {
			player1.x = player1.x - player1.vx;
		}
	}
	if (GetAsyncKeyState(VK_S)) {
		if (player1.x + 100 + player1.vx <= 523) {
			player1.x = player1.x + player1.vx;
		}
	}
}
void mov_p2_logic() {
	if (GetAsyncKeyState(VK_O)) {
		if ((player2.x - player2.vx) >= 75) {
			player2.x = player2.x - player2.vx;
		}
	}
	if (GetAsyncKeyState(VK_L)) {
		if (player2.x + 100 + player2.vx <= 523) {
			player2.x = player2.x + player2.vx;
		}
	}
}
void mov_cpu_logic() {
	if ((pomo.posy > w_height * 0.5)&&(pomo.vy>0)) {
		if (pomo.posx > player2.x+50) {
			if (player2.x + 100 + player2.vx <= 525) {
				player2.x += player2.vx;
			}
		}
		if (pomo.posx < player2.x + 50) {
			if (player2.x - player2.vx >= 75) {
				player2.x -= player2.vx;
			}
			//}
		}
	}
}
void colisions_logic() {
	if (pomo.posy >= 830 && pomo.posx >= player2.x && pomo.posx <= player2.x + 100) {
		pomo.vy *= (-1);
		player2.maxP++;//quando defende a pontuacao do P2 almenta
	}
	if (pomo.posy < 140 && pomo.posx >= player1.x && pomo.posx <= player1.x + 100) {
		pomo.vy *= (-1);
		player1.maxP++;
	}
	if (pomo.posx <= 80 || pomo.posx >= 520) {
		pomo.vx *= (-1);
	}
}
void points_manager() {
	if (pomo.posy > 840) {//se passou pelo player 2
		player1.points++;
		if (player1.points == 11) {
			player1.sets++;
			player1.points = 0;
		}
		resetPomoPos();
	}
	if (pomo.posy < 130) {//se passou pelo player 1
		player2.points++;
		if (player2.points == 11) {
			player2.sets++;
			player2.points = 0;
		}
		resetPomoPos();
	}
}
int someone_won() {
	if (player1.sets == qntSets || player2.sets == qntSets) {
		return 1;
		//reinicia o jogo e fala quem ganhou
	}
	return 0;
}
void ranking() {
	//da opcoes para entrada de nomes e grava em um arquivo
	//organizar uma maneira no gerenciador de pontos para ver a qnt max que cada player ficou sem deixar a bola passar
}
void idle() {
	if (MODE == START) {
		mov_p1_logic();
		if (RIVAL == 2) {
			mov_p2_logic();
		} else if(RIVAL==3){
			mov_cpu_logic();
		}
		colisions_logic();
		points_manager();
		if ( someone_won() ) {
			ranking(); //intoLeaderboard sera chamana so na ranking
		}
		//Pomo position's increment
		pomo.posx += pomo.vx;
		pomo.posy += pomo.vy;
		//
		frameCounter++;
		if (frameCounter == 120) {//poderia ser qualquer par maior que 2 
			frameCounter = 0;
		}
		glutPostRedisplay();
	}
	if (MODE == MENU) {
		/*if (GetAsyncKeyState(VK_SCAPE)) {
			exit(0);
		}*/
		//glutPostRedisplay();
	}
}
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitContextVersion(1, 1);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 970);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("projeto");
	//glEnable(GL_DEPTH_TEST);

	
	startScene();
	glutDisplayFunc(mainFrame);
	glutReshapeFunc(reshape);

	//glutSpecialFunc(kb_pressed);
	glutKeyboardFunc(kb_pressed);
	glutMouseFunc(mousesao);
	// here are the new entries
	//glutIgnoreKeyRepeat(1);
	//glutSpecialUpFunc(kb_released);
	glutIdleFunc(idle);
	glutTimerFunc(fps, mainFrame, 0);
	//glutTimerFunc(16, idle, 0);

	glutMainLoop();
}