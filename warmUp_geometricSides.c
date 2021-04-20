#include <glew.h>
#include <freeglut.h>
#include <math.h>
#include <string.h> //numero de lados para uma string
#define PI 3.14159265359
#define RADIUS 400
#define RESX 1980  //Coloque aqui sua resolução atual pois o prog. entrará em um fullscreen :D
#define RESY 1080 
int qnt_lados = 2; //1 reta, um ponto em 90 e outro em 270 
void startScene() {
	glClearColor(0.1, 0.1, 0.1, 1);
}
void sidesCounterDisplay(){ //Para exibir a quantidade de lados da figura atual 
	char string[42];
	sprintf_s(string,42,"Numbers of sides: %d", qnt_lados);//coloca o int na string

	glRasterPos2f(-(RESX/2)+(RESX*0.10), -(RESY / 2) + (RESY * 0.10)); //onde o texto aparece
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, string);
}
void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT);

	glutFullScreen(); // Coloque o glOrtho do mesmo tamanho de sua resolução e seus eixos div por 2 (como preserva proporcao?)
				      // Sei que não era necessario, é uma feature :v 
	glColor3f(0.63, 0.42, 0.12);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(1);

	glBegin(GL_LINE_LOOP);
		double alpha = 0;
		for (int i = 0;i<qnt_lados ;i++, alpha = ((2 * PI) / qnt_lados)*i) {
				glVertex3d(-RADIUS*sin(alpha), -RADIUS * cos(alpha),0);
		}//com o raio negativo no lugar do eixo y, a figura em uma 'base' em (-raio,0)
		//para diferenciar um pouco...
	glEnd();
	
	sidesCounterDisplay(qnt_lados);

	glFlush();
}
void reSize(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-RESX/2, RESX/2, -RESY/2, RESY/2, -1, 1); //Para funcionar o fullscreen, ( -(res_y)/2,(res_y)/2,-(res_x)/2,(res_x)/2, -1,1 )
										            //No meu caso:                    1980/2   ,        ...         ,1080/2 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void kb_event(unsigned char key, int x, int y) {
	switch (key) {
		case 27: {
			exit(0);
			break;
		}
		case '+': {
			qnt_lados++;
			glutPostRedisplay();
			break;
		}
		case '-': {
			qnt_lados--;
			glutPostRedisplay();
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
	glutInitWindowSize(RESX, RESY);
	glutInitWindowPosition(0, 0);

	glutCreateWindow("lados_maneirissimos");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(reSize);
	glutKeyboardFunc(kb_event);

	startScene();

	glutMainLoop();
}