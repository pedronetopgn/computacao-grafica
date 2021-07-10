#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <GL/glut.h>
#include <mmsystem.h>

#define UP 1
#define Down 2
#define LEFT 3
#define RIGHT 4

//variáveis de status
GLint lvl = 1;
GLint points = 0;
GLint size = 0;
GLbyte gameOver = true;
GLbyte EnableLight = true;

//variáveis da cobra
GLint bodyPos[2][100] = {{}}; //inicializando matriz de posição do corpo
GLint _x = 5;
GLint _z = 10;
GLint _oldX[2] = {}; //inicializando vetores
GLint _oldZ[2] = {};
GLbyte direction = 0;

//variáveis da comida
GLint _bx = 0;
GLint _bz = 0;

//variáveis de tela
GLint _w = 800;
GLint _h = 550;
GLint _Giw = 0; //variáveis para configurar o espaço do plano em que a cobra vai andar
GLint _Gih = 0;
GLint _Gfw = 150;
GLint _Gfh = 150;

//variáveis de angulos da camera
static GLfloat view_rotx = 45.0F;
static GLfloat view_roty = 0.0F;
static GLfloat view_rotz = 0.0F;

static GLfloat headRotation = 90.0F;
static GLfloat zoom = -300.0f; //zoom 

//configuração de luz
void initLight() {
	//add ambient light
	GLfloat ambientColor[] = {0.3f,0.4f,0.8f,1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	
	//add positioned light
	GLfloat lightColor0[] {0.5f,0.5f,0.5f,1.0f};
	GLfloat lightPos0[] {4.0f,0.0f,8.0f,1.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	
	//add directed light
	GLfloat lightColor1[] = {0.5f,0.2f,0.2f,1.0f};
	GLfloat lightPos1[] = {-1.0f,0.5f,0.5f,0.0f};
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
}

//inicializa configurações
void initialize(void){
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.4f,0.8f,1.0f,1.0f);
	if(EnableLight){
		glEnable(GL_COLOR_MATERIAL); //ativa cores
		glEnable(GL_LIGHTING); //ativa iluminação
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1); 
		glEnable(GL_NORMALIZE);
	}
}

//configura resize da janela
void resize(int w, int h) {
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0,(double)w/(double)h,1,800.0);
}

void Write(char*string) { //escreve string na tela
	while(*string)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *string++);
}

//angulo de visualização do plano
void ManipulateViewAngle(){
	glRotatef(view_rotx,1.0,0.0,0.0);
	glRotatef(view_roty,0.0,1.0,0.0);
	glRotatef(view_rotz,0.0,0.0,1.0);
}

//reseta o tamanho da cobra e localização
void Reset(){
	_x = 5;
	_z = 10;
	direction = 0;
	lvl = 1;
	points = 0;
	size = 0;
	gameOver = false;
	view_rotx = 45.0F;
	view_roty = 0.0F;
	view_rotz = 0.0F;
	headRotation = 90.0F;
}

//Mostra tela de bem vindo
void WelcomeScreen(){
	char tmp_str[40];
	
	glRasterPos2f(-40,20);
	Write("================ Bem Vindo(a) ================");
	glRasterPos2f(-60,10);
	Write("================ Utilize as setinhas para jogar ==============");
	glRasterPos2f(-60,0);
	Write("=============== Para comecar pressione 'S' ================");
}

void DrawSnake(){	
	int i;
	glPushMatrix();
	ManipulateViewAngle();
	//desenha o plano 
	glPushMatrix();
	glColor3f(0.0,0.6,0.2);
	glTranslatef(75.0,-16.0,75.0);
	glScalef(155,5.0,155);
	glutSolidCube(1);
	glPopMatrix();
	//desenha a cabeça da cobra
	glColor3f(1,0,0);
	glTranslatef(_x,-10.0,_z);
	glScalef(0.5,0.5,0.5);
	glutSolidSphere(10,20,20);
	glRotatef(headRotation, 0.0, 1.0, 0.0);
	glColor3f(1,0,0);
	glTranslatef(0,0.0,6.0);
	glScalef(0.8,1.0,1.0);
	glutSolidCone(10,10,20,20);
	glColor3f(1,1,1);
	glTranslatef(-4.0,10.0,0.0);
	glScalef(0.3,0.3,0.3);
	glutSolidSphere(10,20,20);
	glTranslatef(26.0,0.0,0.0);
	glutSolidSphere(10,20,20);
	glPopMatrix();
	//desenha o corpo
	for(i=0; i<size; i++){
		glPushMatrix();
		ManipulateViewAngle();
		glTranslatef(bodyPos[0][i], -10.0, bodyPos[1][i]);
		glColor3f(1,0,0);
		glScalef(0.5,0.5,0.5);
		glutSolidSphere(7,20,20);
		glPopMatrix();
	}
}

void DrawFood() {
	//desenha a esfera que representa a comida
	glPushMatrix();
	ManipulateViewAngle();
	glTranslatef(_bx,-10.0,_bz);
	glColor3f(1,1,0);
	glScalef(0.5,0.5,0.5);
	glutSolidSphere(7,20,20);
	glPopMatrix();
}

void GameStatus(){
	char tmp_str[40];
	
	glColor3f(0.8,0.2,0);
	glRasterPos2f(5,20);
	sprintf(tmp_str, "Snake Game");
	Write(tmp_str);
	//printa o status do jogo na tela
	glColor3f(0,0,0);
	glRasterPos2f(5,10);
	sprintf(tmp_str, "Level: %d Points: %d", lvl, points);
	Write(tmp_str);
}

//gera números aleatórios para o posicionamento da comida
int RandomNumber(int high, int low){
	return (rand()%(high-low))+low;
}

//gera uma nova comida
void newFood(){
	_bx = RandomNumber(_Gfw-_Giw,_Giw+10);
	_bz = RandomNumber(_Gfh-_Gih,_Gih+10);
}

//Checa se houve colisão
bool collision(){
	int i;
	
	for(i=0; i<size; i++){
		if(((bodyPos[0][i] == _x) && (bodyPos[1][i] == _z)) ||
		(((bodyPos[0][i] == _x) && (bodyPos[0][i] == _x+5)) &&
		((bodyPos[1][i] == _z) && (bodyPos[1][i] == _z+5))))
		return true;
	}
	return false;
}

//movimenta a cobra
void Run(int value){
	int i;
	
	_oldX[1] = _x;
	_oldZ[1] = _z;
	switch(direction){
		case RIGHT:
			headRotation = 90;
			_x += 6;
			if(_x>_Gfw-2) _x = _Giw-1;
			break;
		case LEFT:
			headRotation = -90;
			_x -= 6;
			if(_x<0) _x = _Gfw-2;
			break;
		case UP:
			headRotation = 0;
			_z += 6;
			if(_z>_Gfh-2) _z = _Gih-1;
			break;
		case Down:
			headRotation = 180;
			_z -= 6;
			if(_z<2) _z = _Gfh-2;
			break;
	}
	//checa a colisão e finaliza o jogo
	if(collision()){
		gameOver = true;
		PlaySound("mixkit-arcade-retro-game-over-213.wav", NULL, SND_ASYNC|SND_FILENAME);
	} 
	
	//checa se a cobra comeu
	//se sim, aumenta a pontuação, tamanho da cobra e cria uma nova comida
	if((_x==_bx && _z==_bz) ||
	((_x>=_bx)&&(_x<=_bx+4)&&(_z>=_bz)&&(_z<=_bz+4)) ||
	((_x<=_bx)&&(_x>=_bx-4)&&(_z<=_bz)&&(_z>=_bz-4)) ||
	((_x<=_bx)&&(_x>=_bx-4)&&(_z>=_bz)&&(_z<=_bz+4)) ||
	((_x>=_bx)&&(_x<=_bx+4)&&(_z<=_bz)&&(_z>=_bz-4))){
		points++;
		if(points<100) size++; //aumenta o tamanho da cobra enquanto points<100
		if(points%5==0 && lvl<15) lvl++; //aumenta o level enquanto lvl<15
		//PlaySound("Crunch.wav-10098-Free-Loops.com.mp3", NULL, SND_ASYNC|SND_FILENAME);
		newFood();
	}
	
	for(i=0; i<size; i++){ //modifica o posicionamento do corpo
		_oldX[0] = _oldX[1];
	   	_oldZ[0] = _oldZ[1];	
	   	_oldX[1] = bodyPos[0][i];
	   	_oldZ[1] = bodyPos[1][i];
	   	bodyPos[0][i] = _oldX[0];
	   	bodyPos[1][i] = _oldZ[0];
	}
	
	//seta o timer (velocidade)
	glutTimerFunc(130, Run, 0);
}

void Display(void){
	//clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if(EnableLight) initLight();
	
	glTranslatef(-60.0,40.0,zoom); //configuração de visualização
	
	//checa se o jogo não terminou
	if(!gameOver){
		GameStatus();
		DrawFood();
		DrawSnake();
	}else{
		WelcomeScreen();
	}
	
	//atualiza a tela
	glutPostRedisplay();
	glutSwapBuffers();
}

//função para receber teclado especial, verificar quais setas foram pressionadas
void Special(int key, int x, int y){
	switch(key){
		case GLUT_KEY_RIGHT:
			if(direction!=LEFT)
				direction = RIGHT;
			break;
		case GLUT_KEY_LEFT:
			if(direction!=RIGHT)
				direction = LEFT;
			break;
		case GLUT_KEY_UP:
			if(direction!=UP)
				direction = Down;
			break;
		case GLUT_KEY_DOWN:
			if(direction!=Down)
				direction = UP;
			break;
	}
}

//função para verificar se clicou em S para começar o jogo ou esc para fechar o jogo.
void keyboard(unsigned char key, int x, int y){
	switch(key){
		case 'S': 
			Reset();
			glutPostRedisplay();
		   	break;
		case 's': 
			Reset();
			glutPostRedisplay();
			break;
		case 27: //esc
			exit(0);
			break;
		default:
			break;
	}
}

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(_w,_h);
	glutInitWindowPosition(500,200);
	glutCreateWindow("Snake Game");
	initialize();
	glutSpecialFunc(Special);
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(Display);
	glutReshapeFunc(resize);
	newFood();
	Run(0);
	glutMainLoop();
}






