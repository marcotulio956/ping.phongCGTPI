//version 1.6
#include <stdlib.h>
#include <stdio.h>
#include <time.h>//gerar numeros aleatorios
#include <string.h>//usar strlen
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <Windows.h>// usado para teclas 

#include "SOIL.h"
#pragma comment( lib,"SOIL.lib")
//#include <SDL/SDL_mixer.h>//Nao consegui instalar a biblioteca
//#pragma comment( lib,  <SDL_mixer.lib>)
//#include "include/SOIL.h" 
//#include "include/SDL.h"
//#include "include/SDL_mixer.h"
//#pragma comment( lib,  "SDL_mixer.lib")
#define VK_W 0x57
#define VK_S 0x53
#define VK_O 0x4F
#define VK_L 0x4C
#define MENU 1
#define START 2
#define OPTIONS 3
#define LEADERBOARD 4
#define IMOUT 5
#define PAUSE 6
#define GETNAMES 7
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
	int inP;
	char* name;
}barrier;
typedef struct ball {
	float posx, posy;
	float vx, vy;
}ball;
typedef struct mice {
	int posx;
	int posy;
	int	button;
	int state;
}mice;
typedef struct buttonBox {//nao ha animacao aqui
	int posx;
	int posy;
	int width;
	int height;
	int mouseOver;
	GLuint id;
}buttonBox;
typedef struct tex_or_sprite {
	int posx;
	int posy;
	int height;//2x distancia do centro a borda superior
	int width;// || inferior
	int qntFrames;//para qualquer sprite
	float atualFrame;//para qualquer sprite
	GLuint id;
}tex_or_sprite;
typedef struct rank {
	char name[20];
	int score;
}rank;

barrier player1, player2;//player 1 eh o 'de baixo' e eh verde, player 2 vermelho
ball pomo;//bola
mice mouse;

buttonBox start, options, leaderboard, imout;// go back era para ser um botao voltar, mas estou sem tempo de insirar coordenadas para-o
buttonBox dif_normal, dif_fast, dif_asian;//aumenta tanto a velocidade da bolinha quanto a dos players no caso asian
buttonBox set3, set7, set9, set11;//so para deixar o jogo comprido
buttonBox p1p2, p1cpu;

tex_or_sprite lbSprite, pomoTexture, player1Texture, player2Texture;
tex_or_sprite leaderboardTexture, menuTexture, pauseTexture, optionsTexture;
tex_or_sprite player1won, player2won;
tex_or_sprite coolSpace, triforce, crown, arrow;
tex_or_sprite player1String, player2String, cpuString;

int RIVAL = P2;
int MODE = MENU;//sempre inicia no menu
int dif = 3; //padrão 3, rapido 5, asian 7 //alteravel (em jogo)
int qntSets = 1;//alteravel (em jogo)/3,7,9,11
int inputMode1;//para entrar com os nomes
int inputMode2;
float fps = 1000 / 120;
rank* players;//para ordenarcao na leaderboard
FILE* arch;//para leaderboard
int qntInArquivo = 0;
/*
Mix_Music* menuMusic = NULL;
Mix_Music* startMusic = NULL;
Mix_Music* hitBarSound = NULL;
Mix_Music* hitWallSound = NULL;
Mix_Music* finishSound = NULL;
*/
GLuint putTexID(char name[50]) {//so devolve o id
	GLuint tex = SOIL_load_OGL_texture(name, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (tex == 0) {
		printf("SOIL error: '%s' in name: '%s'\n", SOIL_last_result(), name);
	}
	return tex;
}
void putCoordinatedTex(GLuint id, int posx, int posy, int width, int height) {//Era so alterar o putBackground mas quis ter um especifico
	//glClear(GL_TEXTURE_2D);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	{
		glBindTexture(GL_TEXTURE_2D, id);
		glBegin(GL_QUADS);
		{
			glTexCoord2f(0, 0);
			glVertex2f(posx, posy);

			glTexCoord2f(1, 0);
			glVertex2f(posx + width, posy);

			glTexCoord2f(1, 1);
			glVertex2f(posx + width, posy + height);

			glTexCoord2f(0, 1);
			glVertex2f(posx, posy + height);
		}
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
}
void putBackground(GLuint texName) {//coloca tex como todo o fundo
	glEnable(GL_TEXTURE_2D);
	{
		glBindTexture(GL_TEXTURE_2D, texName);
		glBegin(GL_QUADS);
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
}
void wInScreen(char string[100], int posx, int posy) {
	glRasterPos2f(posx, posy); //onde o texto aparece
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, string);
}
void infoDebug() { //informacoes sobre posicoes do pomo em START SOMENTE
	char string[100];
	sprintf_s(string, 100, "pomo.x: %0.2f pomo.y: %0.2f\npomo.vx: %0.2f pomo.vy: %0.2f\np1x:%d p2x:%d", pomo.posx, pomo.posy, pomo.vx, pomo.vy, (int)player1.x, (int)player2.x);
	wInScreen(string, w_width * 0.27, w_height * 0.091);
}
void showStatistics() {
	char PlayerXStatistics[75];
	sprintf_s(PlayerXStatistics, 75, "AB:%d Points:%d\nPB:%d Sets:%d", player1.inP, player1.points, player1.maxP, player1.sets);
	wInScreen(PlayerXStatistics, 375, w_height * 0.103);
	sprintf_s(PlayerXStatistics, 75, "AB:%d Points:%d\nPB:%d Sets:%d", player2.inP, player2.points, player2.maxP, player2.sets);
	wInScreen(PlayerXStatistics, 375, w_height * 0.910);
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
	player1.sets = 0;
	player2.sets = 0;
	player1.vx = (dif == 7 ? 20 : 2 * dif);
	player2.vx = (dif == 7 ? 20 : 2 * dif);
	player1.maxP = 0;
	player1.maxP = 0;
	player1.name = malloc(sizeof(char) * 100);
	player2.name = malloc(sizeof(char) * 100);
	for (int i = 0; i < 100; i++) {
		player1.name[i] = ' '; player2.name[i] = ' ';
	}
}
void setThingsAssets() {
	start.posx = 185;
	start.posy = 580;
	start.width = 175;
	start.height = 86;
	start.mouseOver = 0;

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

	pauseTexture.width = 217;
	pauseTexture.height = 79;

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

	triforce.height = 20;
	triforce.width = 20;

	player1String.width = 170;
	player1String.height = 48;

	player2String.width = 170;
	player2String.height = 44;

	cpuString.width = 67;
	cpuString.height = 36;

	crown.width = 60;
	crown.height = 60;
}
void setTextures_and_Sprites() {
	menuTexture.id = putTexID("backgrounds/menub.png");//nenhuma precisa de iniciar outros parametros pos atuam como
	optionsTexture.id = putTexID("backgrounds/options.png");//
	leaderboardTexture.id = putTexID("backgrounds/leaderboard.png");//
	coolSpace.id = putTexID("backgrounds/space.png");
	triforce.id = putTexID("floatingTex/triforce.png");

	start.id = putTexID("floatingTex/startS.png");
	options.id = putTexID("floatingTex/optionsS.png");
	leaderboard.id = putTexID("floatingTex/leaderboardS.png");
	imout.id = putTexID("floatingTex/exitS.png");

	player1won.id = putTexID("backgrounds/player1won.png");
	player2won.id = putTexID("backgrounds/player2won.png");
	player1Texture.id = putTexID("floatingTex/paddle1.png");
	player2Texture.id = putTexID("floatingTex/paddle2.png");

	pomoTexture.id = putTexID("floatingTex/pomo.png");

	pauseTexture.id = putTexID("floatingTex/pauseSolto.png");//

	player1String.id = putTexID("floatingTex/player1.png");
	player2String.id = putTexID("floatingTex/player2.png");
	cpuString.id = putTexID("floatingTex/cpu.png");
	crown.id = putTexID("floatingTex/crown.png");
	arrow.id = putTexID("floatingTex/arrow.png");

	lbSprite.id = putTexID("sprites/fundo.png");//fumacinha da leaderboard
	lbSprite.atualFrame = 0;
	lbSprite.qntFrames = 75;

}
void setMusic() {
	/*if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
	}
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	startMusic = Mix_LoadMUS("music/bfg.wav");
	if (startMusic == NULL){
		printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
	}
	menuMusic = Mix_LoadMUS("music/moonlight.wav");
	if (menuMusic == NULL) {
		printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
	}
	hitBarSound = Mix_LoadMUS("music/barHit.wav");
	if (hitBarSound == NULL) {
		printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
	}
	hitWallSound = Mix_LoadMUS("music/wallHit.wav");
	if (hitWallSound == NULL) {
		printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
	}
	finishSound = Mix_LoadMUS("music/finishSound.ogg");
	if (finishSound == NULL) {
		printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
	}
	*/
}
void startScene() {//inicilizacao geral
	resetPomoPos();
	resetPlayers();
	setThingsAssets();
	setTextures_and_Sprites();
	//setMusic();

	players = (rank*)malloc(sizeof(rank) * 32);
	for (int k = 0; k < 30; k++) {
		for (int h = 0; h < 20; h++) {
			players[k].name[h] = ' ';
		}
		players[k].score = 0;
	}
	fopen_s(&arch, "leaderboardRecords", "rb");
	char name[30];
	int score = 0;
	fseek(arch, 0, SEEK_SET);
	for (int k = 0; k < 30; k++) {
		fread(name, sizeof(char), 20, arch);
		fread(&score, sizeof(int), 1, arch);
		if (feof(arch)) {
			break;
		}
		//strcpy_s(players[k].name, 20,name );
		for (int l = 0; l < 20; l++) {
			players[k].name[l] = name[l];
		}
		players[k].score = score;
		qntInArquivo++;
		for (int l = 0; l < 30; l++)
			name[l] = ' ';
	}
	fclose(arch);
}
void archiveRecords() {
	fopen_s(&arch, "leaderboardRecords", "wb");
	printf("abriu para reescrever na mem\n");
	fseek(arch, 0, SEEK_SET);
	for (int k = 0; k < 30; k++) {
		if (player1.maxP > players[k].score) {
			for (int l = 30; l != k; l--) {
				//strcpy_s(players[l].name, 20, players[l - 1].name);
				for (int m = 0; m < 20; m++) {
					players[l].name[m] = players[l - 1].name[m];
				}
				players[l].score = players[l - 1].score;
			}
			//strcpy_s(players[k].name,20,player1.name);
			for (int m = 0; m < 20; m++) {
				players[k].name[m] = player1.name[m];
			}
			players[k].score = player1.maxP;
			break;
		}
	}
	for (int k = 0; k < 30; k++) {
		if (player2.maxP > players[k].score) {
			for (int l = 30; l >= k; l--) {
				//strcpy_s(players[l].name, 20, players[l - 1].name);
				for (int m = 0; m < 20; m++) {
					players[l].name[m] = players[l - 1].name[m];
				}
				players[l].score = players[l - 1].score;
			}
			//strcpy_s(players[k].name, 20, player2.name);
			for (int m = 0; m < 20; m++) {
				players[k].name[m] = player2.name[m];
			}
			players[k].score = player2.maxP;
			break;
		}
	}
	printf("substituiu na memoria\n"); int k = 0;
	do {
		fwrite(players[k].name, sizeof(char), 20, arch);
		fwrite(&players[k].score, sizeof(int), 1, arch);
		printf("reescreveu %s %d    e k:%d\n", players[k].name, players[k].score, k);
		k++;
	} while (k < 30);
	fclose(arch);
}
char bufferzao[50];
void readArchived() {
	for (int k = 0; players[k].score != 0; k++) {
		printf("k:%d\n", k + 1);
		for (int l = 0; l < strlen(bufferzao); l++) {
			bufferzao[l] = ' ';
		}
		if (k + 1 < 10) {//para os numeros ficarem alinhados na leaderboard
			snprintf(bufferzao, 50, "%dº   # %d - %s", k + 1, players[k].score, players[k].name);
		}
		else if (k + 1 >= 10) {//n>9 ocupa + digitos que n<10
			snprintf(bufferzao, 50, "%dº # %d - %s", k + 1, players[k].score, players[k].name);
		}
		printf("exbindo: %s\n", bufferzao);
		//wInScreen(bufferzao, -140 + w_width / 2, 760 - (25 * k));
		glRasterPos2f(-150 + w_width / 2, 760 - (25 * k)); //onde o texto aparece
		glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, bufferzao);
		glutPostRedisplay();
	}
}
int vBoxEmBotao(buttonBox botao, int x, int y) {//verifica hitbox
	if ((x <= (botao.posx + botao.width)) && (x >= (botao.posx))
		&& (y <= (botao.posy + botao.height)) && (y >= (botao.posy))) {
		return 1;
	}
	return 0;
}
void mousesaoDinamico(int x, int y) {
	mouse.posx = x;
	mouse.posy = w_height - y;
	if (MODE == MENU) {
		if (vBoxEmBotao(start, mouse.posx, mouse.posy)) {//efeito hover em start
			start.mouseOver = 1;
		}
		else {
			start.mouseOver = 0;
		}
		if (vBoxEmBotao(options, mouse.posx, mouse.posy)) {//efeito hover em start
			options.mouseOver = 1;
		}
		else {
			options.mouseOver = 0;
		}
		if (vBoxEmBotao(leaderboard, mouse.posx, mouse.posy)) {//efeito hover em start
			leaderboard.mouseOver = 1;
		}
		else {
			leaderboard.mouseOver = 0;
		}
		if (vBoxEmBotao(imout, mouse.posx, mouse.posy)) {//efeito hover em start
			imout.mouseOver = 1;
		}
		else {
			imout.mouseOver = 0;
		}
	}
}
void mousesao(int button, int state, int x, int y) {
	mouse.button = button; mouse.state = state; mouse.posx = x; mouse.posy = w_height - y;
	printf("click posx: %d posy: %d state: %d button: %d\n", mouse.posx, mouse.posy, mouse.state, mouse.button);
	if (MODE == MENU) {
		if (((mouse.button == 0) && (mouse.state == 0)) && (vBoxEmBotao(start, mouse.posx, mouse.posy))) {//verifica start
			MODE = START; glutPostRedisplay();
		}
		if (((mouse.button == 0) && (mouse.state == 0)) && (vBoxEmBotao(options, mouse.posx, mouse.posy))) {//verifica options
			MODE = OPTIONS; glutPostRedisplay();
		}
		if (((mouse.button == 0) && (mouse.state == 0)) && (vBoxEmBotao(leaderboard, mouse.posx, mouse.posy))) {//verifica leaderboard
			MODE = LEADERBOARD; glutPostRedisplay();
		}
		if (((mouse.button == 0) && (mouse.state == 0)) && (vBoxEmBotao(imout, mouse.posx, mouse.posy))) {//verifica sair
			exit(0);
		}
	}
	if (MODE == OPTIONS) {
		if (((mouse.button == 0) && (mouse.state == 0)) && (vBoxEmBotao(dif_normal, mouse.posx, mouse.posy))) {
			dif = 3; resetPlayers(); resetPomoPos(); printf("setou dif: %d\n", dif);
		}
		if (((mouse.button == 0) && (mouse.state == 0)) && (vBoxEmBotao(dif_fast, mouse.posx, mouse.posy))) {
			dif = 5; resetPlayers(); resetPomoPos(); printf("setou dif: %d\n", dif);
		}
		if (((mouse.button == 0) && (mouse.state == 0)) && (vBoxEmBotao(dif_asian, mouse.posx, mouse.posy))) {
			dif = 7; resetPlayers(); resetPomoPos(); printf("setou dif: %d\n", dif);
		}
		if (((mouse.button == 0) && (mouse.state == 0)) && (vBoxEmBotao(p1p2, mouse.posx, mouse.posy))) {
			RIVAL = P2; printf("setou riv: %d\n", RIVAL);
		}
		if (((mouse.button == 0) && (mouse.state == 0)) && (vBoxEmBotao(p1cpu, mouse.posx, mouse.posy))) {
			RIVAL = CPU; printf("setou riv: %d\n", RIVAL);
		}
		if (((mouse.button == 0) && (mouse.state == 0)) && (vBoxEmBotao(set3, mouse.posx, mouse.posy))) {
			qntSets = 3; resetPlayers(); resetPomoPos(); printf("setou sets 3: %d\n", qntSets);
		}
		if (((mouse.button == 0) && (mouse.state == 0)) && (vBoxEmBotao(set7, mouse.posx, mouse.posy))) {
			qntSets = 7; resetPlayers(); resetPomoPos(); printf("setou sets 7: %d\n", qntSets);
		}
		if (((mouse.button == 0) && (mouse.state == 0)) && (vBoxEmBotao(set9, mouse.posx, mouse.posy))) {
			qntSets = 9; resetPlayers(); resetPomoPos(); printf("setou sets 9: %d\n", qntSets);
		}
		if (((mouse.button == 0) && (mouse.state == 0)) && (vBoxEmBotao(set11, mouse.posx, mouse.posy))) {
			qntSets = 11; resetPlayers(); resetPomoPos(); printf("setou sets 11: %d\n", qntSets);
		}
	}
}
void intoMenu() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear(GL_TEXTURE_2D);
	putBackground(menuTexture.id);
	if (start.mouseOver) {
		putCoordinatedTex(start.id, start.posx, start.posy, start.width, start.height);
	}
	else if (options.mouseOver) {
		putCoordinatedTex(options.id, options.posx, options.posy, options.width, options.height);
	}
	else if (leaderboard.mouseOver) {
		putCoordinatedTex(leaderboard.id, leaderboard.posx, leaderboard.posy, leaderboard.width, leaderboard.height);
	}
	else if (imout.mouseOver) {
		putCoordinatedTex(imout.id, imout.posx, imout.posy, imout.width, imout.height);
	}
	glutSwapBuffers();
}
void intoStart() {
	glColor3f(1, 1, 1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear(GL_TEXTURE_2D);

	putBackground(coolSpace.id);
	glClear(GL_TEXTURE_2D);

	if (RIVAL == P2) {//coloca string do rival
		putCoordinatedTex(player2String.id, 74, 970 - 118, player2String.width, player2String.height);
	}
	else {
		putCoordinatedTex(cpuString.id, 74, 970 - 118, cpuString.width, cpuString.height);
	}
	putCoordinatedTex(player1String.id, 74, 970 - 856 - 48, player1String.width, player1String.height);//string do player 1

	if (player1.sets > player2.sets) {//ve em quem poem a coroa
		putCoordinatedTex(crown.id, 27, 970 - 856 - 48, crown.width, crown.height);
	}
	if (player1.sets == player2.sets) {
		if (player1.points > player2.points) {
			putCoordinatedTex(crown.id, 27, 970 - 856 - 48, crown.width, crown.height);
		}
		if (player1.points < player2.points) {
			putCoordinatedTex(crown.id, 27, 970 - 118, crown.width, crown.height);
		}
	}
	if ((player1.sets < player2.sets)) {
		putCoordinatedTex(crown.id, 27, 970 - 118, crown.width, crown.height);
	}//

	putCoordinatedTex(player1Texture.id, player1.x + 4, (0.134 * w_height) - 5, 100, 10);//paddle 1
	glClear(GL_TEXTURE_2D);

	putCoordinatedTex(player2Texture.id, player2.x + 4, (0.865 * w_height) - 5, 100, 10);//paddle 2
	glClear(GL_TEXTURE_2D);

	putCoordinatedTex(pomoTexture.id, pomo.posx - 13, pomo.posy - 13, 26, 26);//pomo
	glClear(GL_TEXTURE_2D);

	glColor3f(1, 1, 1);//cor da letra de estatistica

	//infoDebug();//comentar na versao final

	showStatistics();//mostrar pontuacoes

	glutSwapBuffers();
}
void intoOptions() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear(GL_TEXTURE_2D);
	putBackground(optionsTexture.id);
	switch (dif) {
	case 3: {
		putCoordinatedTex(triforce.id, dif_normal.posx - 35, dif_normal.posy + 11, triforce.width, triforce.height); break;
	}
	case 5: {
		putCoordinatedTex(triforce.id, dif_fast.posx - 35, dif_fast.posy + 11, triforce.width, triforce.height); break;
	}
	case 7: {
		putCoordinatedTex(triforce.id, dif_asian.posx - 35, dif_asian.posy + 11, triforce.width, triforce.height); break;
	}
	}
	switch (RIVAL) {
	case P2: {
		putCoordinatedTex(triforce.id, p1p2.posx - 45, p1p2.posy, triforce.width, triforce.height); break;
	}
	case CPU: {
		putCoordinatedTex(triforce.id, p1cpu.posx - 45, p1cpu.posy, triforce.width, triforce.height); break;
	}
	}
	switch (qntSets) {
	case 3: {
		putCoordinatedTex(triforce.id, set3.posx + 5, set3.posy - 25, triforce.width, triforce.height); break;
	}
	case 7: {
		putCoordinatedTex(triforce.id, set7.posx + 5, set3.posy - 25, triforce.width, triforce.height); break;
	}
	case 9: {
		putCoordinatedTex(triforce.id, set9.posx + 5, set3.posy - 25, triforce.width, triforce.height); break;
	}
	case 11: {
		putCoordinatedTex(triforce.id, set11.posx + 10, set3.posy - 25, triforce.width, triforce.height); break;
	}
	}
	glutSwapBuffers();
}
int i = 0, j = 0;//para aumentar o tam. dos nomes
void intoGetNames() {
	;
	glColor3f(1, 1, 1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear(GL_TEXTURE_2D);
	if (player1.sets == qntSets) {
		putBackground(player1won.id);
	}
	else if (player2.sets == qntSets) {
		putBackground(player2won.id);
	}
	if (inputMode1 == 1) {
		putCoordinatedTex(arrow.id, 2, 970 - 637, 36, 36);//player 1
	}
	if (inputMode1 == 2) {
		putCoordinatedTex(arrow.id, 2, 970 - 384, 36, 36);//player 2
	}
	wInScreen(player1.name, 209, 970 - 635);
	wInScreen(player2.name, 209, 970 - 381);
	/*for(int k=0;k<20;k++) {
		glRasterPos2d(209, 970 - 635); //onde o texto aparece
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, player1.name[k]);
		glRasterPos2d(209, 970 - 381); //onde o texto aparece
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, player2.name[k]);
	}*/
	if (inputMode2 == 0 && inputMode1 == 0) {
		archiveRecords();//guarda as pontuacoes dessa partida
		resetPlayers();//resetara o nome ,fazer as coisas da leaderboard antes disso
		resetPomoPos();
		i = 0, j = 0;//reseta o tam. dos nomes para os prox. players
		MODE = LEADERBOARD;
		glutPostRedisplay();
	}
	glutSwapBuffers();
}
void intoLeaderboard() {
	glClear(GL_TEXTURE_2D);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	{
		glBindTexture(GL_TEXTURE_2D, lbSprite.id);
		{
			//glTranslatef(posx, posy, 0);
			glBegin(GL_POLYGON);
			{
				glTexCoord3f(lbSprite.atualFrame / 75, 0, 0);
				glVertex3f(0, 0, 0);

				glTexCoord3f((lbSprite.atualFrame + 1) / 75, 0, 0);
				glVertex3f(w_width, 0, 0);

				glTexCoord3f((lbSprite.atualFrame + 1) / 75, 1, 0);
				glVertex3f(w_width, w_height, 0);

				glTexCoord3f(lbSprite.atualFrame / 75, 1, 0);
				glVertex3f(0, w_height, 0);

			}
			glEnd();
		}
	}
	lbSprite.atualFrame++;
	if (lbSprite.atualFrame == lbSprite.qntFrames) {
		lbSprite.atualFrame = 0;
	}
	glDisable(GL_TEXTURE_2D);
	printf("%d %d %d \n", lbSprite.atualFrame, lbSprite.atualFrame / 75, (lbSprite.atualFrame + 1) / 75);
	putBackground(leaderboardTexture.id);
	readArchived();
	glutSwapBuffers();
}
void mainFrame() {
	//printf("modo: %d\n", MODE);
	//glClear(GL_COLOR_BUFFER_BIT);
	//glColor4f(1, 1, 1, 1);
	switch (MODE) {
	case MENU: {
		intoMenu();
		break;
	}
	case START: {
		intoStart();
		break;
	}
	case OPTIONS: {
		intoOptions();
		break;
	}
	case LEADERBOARD: {
		intoLeaderboard();
		break;
	}
	case GETNAMES: {
		intoGetNames();
		break;
	}
	case PAUSE: {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClear(GL_TEXTURE_2D);
		putCoordinatedTex(pauseTexture.id, (int)w_width / 2 - pauseTexture.width / 2, (int)w_height / 2, pauseTexture.width, pauseTexture.height);
		printf("pos pause tex\n");
		glutSwapBuffers();
		break;
	}
	}
}
void reshape(int width, int height) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 600, 0, 970, -1, 1);
	float razaoAspectoJanela = ((float)width) / height;
	float razaoAspectoMundo = ((float)600) / 970;
	// barra chao e teto
	if (razaoAspectoJanela < razaoAspectoMundo) {
		float hViewport = width / razaoAspectoMundo;
		float yViewport = (height - hViewport) / 2;
		glViewport(0, yViewport, width, hViewport);
	}
	// barra lados
	else if (razaoAspectoJanela > razaoAspectoMundo) {
		float wViewport = ((float)height) * razaoAspectoMundo;
		float xViewport = (width - wViewport) / 2;
		glViewport(xViewport, 0, wViewport, height);
	}
	else {//se perfeitamente alinhado janela e mundo
		glViewport(0, 0, width, height);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void kb_pressed(unsigned char key) {
	key == 27 ? exit(0) : 0;//sempre possivel sair do jogo
	if (inputMode1 == 2) {//so uma questao de logica para alterar entre o preenchimento dos nomes
		inputMode2 = 1;//agr eh a vez do player 2
	}
	if (inputMode1 == 1) {
		if (key != 13) {//13 carriage return como enter
			if (key == 8) {//para permitir apagar a letra anterior
				player1.name[i - 1] = ' ';
				i--;
			}
			else {
				player1.name[i] = key;
				i++;
			}
		}
		else if (key == 13) {
			inputMode1 = 2;//muda o valor para que na prox chamada seja a vez do player2
			printf("%d    a   %d\n", inputMode1, inputMode2);
		}
		glutPostRedisplay();
	}
	if (inputMode2 == 1) {
		if (RIVAL == P2) {
			if (key != 13) {
				if (key == 8) {
					player2.name[j - 1] = ' ';
					j--;
				}
				else {
					player2.name[j] = key;
					j++;
				}
			}
			if (key == 13) {//se o 2nd player acabar de preencher
				inputMode1 = 0;//condicao de ir para leaderboard
				inputMode2 = 0;//||
			}
		}
		else {
			inputMode1 = 0;
			inputMode2 = 0;
		}
		glutPostRedisplay();
	}
	printf("%d	%d\n", inputMode1, inputMode2);
	if (inputMode1 == 0 && inputMode2 == 0) {//enquanto os players preenchen algumas teclas sao desativadas
		switch (key) {
		case 8: {//backspace, func d voltar
			if (MODE != MENU) {
				MODE = MENU;
				printf("go back MODE: %d\n", MODE);
				glutPostRedisplay();
				break;
			}
		}
		case 'P':
		case 'p': {
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
			resetPlayers();
			printf("reseted!\n");
			break;
		}
		default: {
			break;
		}
		}
	}
}
void mov_p1_logic() {
	if (GetAsyncKeyState(VK_W)) {
		if ((player1.x - player1.vx) >= (dif == 3 ? 75 : (dif == 5 ? 65 : 60))) {
			player1.x = player1.x - player1.vx;
		}
	}
	if (GetAsyncKeyState(VK_S)) {
		if (player1.x + 100 + player1.vx <= (dif == 3 ? 523 : (dif == 5 ? 528 : 530))) {
			player1.x = player1.x + player1.vx;
		}
	}
}
void mov_p2_logic() {
	if (GetAsyncKeyState(VK_O)) {
		if ((player2.x - player2.vx) >= (dif == 3 ? 75 : (dif == 5 ? 65 : 60))) {
			player2.x = player2.x - player2.vx;
		}
	}
	if (GetAsyncKeyState(VK_L)) {
		if (player2.x + 100 + player2.vx <= (dif == 3 ? 523 : (dif == 5 ? 528 : 530))) {
			player2.x = player2.x + player2.vx;
		}
	}
}
float mov_cpu_hab(int dif) {//na dificuldade normal eh comum fazer pontos, em fast um pouco mais dificil e em asian o tempo para marcar eh o exato
//que o cpu tem de percorrer sua ponta de um canto ou outro oposto, ou seja, so se marca se a bola e o cpu estiver em cantos diferentes
	return(dif == 3 ? (0.57 * w_height) : (dif == 5 ? (0.55 * w_height) : 0));
}
void mov_cpu_logic() {//a linha de baixo faz um deley para o cpu mover-se
	if (((pomo.posy > (mov_cpu_hab(dif))) && (pomo.vy > 0)) || (dif == 7 ? (1 && pomo.vy > 0) : 0)) {//a logica do asian vem do operador 'or'
		if (pomo.posx > player2.x + 50) {
			if (player2.x + 100 + player2.vx <= 525) {
				player2.x += player2.vx;
			}
		}
		if (pomo.posx < player2.x + 50) {
			if (player2.x - player2.vx >= 75) {
				player2.x -= player2.vx;
			}
		}
	}
}
void colisions_logic() {
	if (pomo.posy >= 830 && pomo.posx >= player2.x && pomo.posx <= player2.x + 100) {
		pomo.vy *= (-1);
		player2.inP++;
		if (player2.inP >= player2.maxP) {
			player2.maxP = player2.inP;
		}
	}
	if (pomo.posy < 140 && pomo.posx >= player1.x && pomo.posx <= player1.x + 100) {
		pomo.vy *= (-1);
		player1.inP++;
		if (player1.inP >= player1.maxP) {
			player1.maxP = player1.inP;
		}
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
		player2.inP = 0;
		resetPomoPos();//volta com o pomo para uma aleatoria posx no centro
	}
	if (pomo.posy < 130) {//se passou pelo player 1
		player2.points++;
		if (player2.points == 11) {
			player2.sets++;
			player2.points = 0;
		}
		player1.inP = 0;
		resetPomoPos();
	}
}
void someone_won() {
	if (player1.sets == qntSets || player2.sets == qntSets) {
		inputMode1 = 1;
		if (RIVAL == CPU) {
			player2.name = "THE MACHINE";
		}
		MODE = GETNAMES;
		glutPostRedisplay();
	}
}
void idle() {//quis deixar aqui bem limpo
	if (MODE == START) {
		mov_p1_logic();
		if (RIVAL == 2) {
			mov_p2_logic();
		}
		else if (RIVAL == 3) {
			mov_cpu_logic();
		}
		colisions_logic();
		points_manager();
		someone_won();
		//Pomo position's increment
		pomo.posx += pomo.vx;
		pomo.posy += pomo.vy;
		//
	}
	glutPostRedisplay();
}
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitContextVersion(1, 1);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 970);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("glorioso phong");
	glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);

	startScene();
	glutDisplayFunc(mainFrame);
	glutReshapeFunc(reshape);

	//glutSpecialFunc(kb_pressed);
	glutKeyboardFunc(kb_pressed);
	glutMouseFunc(mousesao);//para clicks
	glutPassiveMotionFunc(mousesaoDinamico);//para efeito hover
	//glutIgnoreKeyRepeat(1);
	//glutSpecialUpFunc(kb_released);
	glutIdleFunc(idle);
	glutTimerFunc(fps, mainFrame, 0);

	glutMainLoop();
}