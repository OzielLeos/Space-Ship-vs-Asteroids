#include <windows.h>
#include"gdipload.h"
#include<stdlib.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
//#include <mmsystem.h>
#include "resource2.h"
#include <string.h>
//Variables constantes
const int ANCHO_VENTANA = 800;
const int ALTO_VENTANA = 600;
const int BPP = 4;
const int TICK = /*100*/30;
const unsigned int BLUE = 0xFF0000FF;
const unsigned int GREEN = 0xFF00FF00;
const unsigned int RED = 0xFFFF0000;

struct cuadro{
	int x,y;
	int ancho,alto;
};//Similar a la estructura rect de winapi

bool game_over = false;

cuadro *pcuadros;

int frameactual;
cuadro *HaciaDer;
cuadro *HaciaIzq;
cuadro *HaciaRecto;
cuadro *FondoMov;
cuadro *Golpe;
cuadro *corazones;
cuadro *number;
cuadro *bala;
int frameDer;
int framegolpe;
bool MovDer=false;
bool MovIzq=false;
bool MovRecto=false;
bool NoMov=true;
bool golpe=false;
bool disparo=false;
int vidas;
int score;
bool Start=false;
struct POSITION {
	int X;
	int Y;
};

struct DIMENSION {
	int ANCHO;
	int ALTO;
};

struct Input
{
	enum Keys
	{
		Backspace = 0x08, Tab,
		Clear = 0x0C, Enter,
		Shift = 0x10, Control, Alt,
		Escape = 0x1B,
		Space = 0x20, PageUp, PageDown, End, Home, Left, Up, Right, Down,
		Zero = 0x30, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,
		A = 0x41, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		NumPad0 = 0x60, NumPad1, NumPad2, NumPad3, NumPad4, NumPad5, NumPad6, NumPad7, NumPad8, NumPad9, 
		F1 = 0x70, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12
	};
}input;
bool fondomirror=false;
//Variables Globales
int *ptrBuffer;
unsigned int anchoBmp;
unsigned int altoBmp;
DIMENSION dmnFigura;
POSITION posFigura;
POSITION posBala;
POSITION PosAst[6];
bool KEYS[256];
Rect NaveHit;
Rect AstHit;
Rect balaHit;
miBitmap mbmp,background,asteroide,skull,Marco,vida;
int tonoazul,tonoverde,tonorojo;
//Definicion de funciones
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void LimpiarFondo(int *ptrBuffer, unsigned int color, int area);
void DibujaFigura(int *ptrBuffer, unsigned int color, int anchoWnd, int altoWnd, DIMENSION dimension, POSITION position);
void imageblt(int *punteroDestino, int *punteroOrigen, int inicioXDestino, int inicioYDestino, int inicioXOrigen, int inicioYOrigen, int ancho, int alto, int anchodefondo, int anchodeorigen);
void transparentblt      (int* punteroDestino, int* punteroOrigen, int inicioXDestino, int inicioYDestino, int inicioXOrigen, int inicioYOrigen, int ancho, int alto, int anchodefondo, int anchodeorigen, int colortransparente);
void mirrorxblt(int* punteroDestino, int* punteroOrigen, int inicioXDestino, int inicioYDestino, int inicioXOrigen, int inicioYOrigen, int ancho, int alto, int anchodefondo, int anchodeorigen);
void mirroryblt(int* punteroDestino, int* punteroOrigen, int inicioXDestino, int inicioYDestino, int inicioXOrigen, int inicioYOrigen, int ancho, int alto, int anchodefondo, int anchodeorigen);
void mirrorxyblt(int* punteroDestino, int* punteroOrigen, int inicioXDestino, int inicioYDestino, int inicioXOrigen, int inicioYOrigen, int ancho, int alto, int anchodefondo, int anchodeorigen);
void scaleblt(int* punteroDestino, int* punteroOrigen, int inicioXDestino, int inicioYDestino, int inicioXOrigen, int inicioYOrigen, int ancho, int alto, int anchodefondo, int anchodeorigen, int escalaX, int escalaY);
void cambiarTonalidadAzul(int* punteroImagen, int ancho, int alto, unsigned char color);
void cambiarTonalidadVerde(int* punteroImagen, int ancho, int alto, unsigned char color);
void cambiarTonalidadRojo(int* punteroImagen, int ancho, int alto, unsigned char color);
void autotransparentblt      (int* punteroDestino, int* punteroOrigen, int inicioXDestino, int inicioYDestino, int inicioXOrigen, int inicioYOrigen, int ancho, int alto, int anchodefondo, int anchodeorigen);
	
void Asteroids();
void MainRender(HWND hWnd);
void Init();
void KeysEvents();
void BarradeVida();
void actual_game();

int WINAPI wWinMain(HINSTANCE hInstance, 
					 HINSTANCE hPrevInstance, 
					 PWSTR pCmdLine, 
					 int nCmdShow)
{
	WNDCLASSEX wc;									// Windows Class Structure
	HWND hWnd;
	MSG msg;

	TCHAR szAppName[] = TEXT("Leos Game");		
	TCHAR szAppTitle[] = TEXT("SpaceShip vs Ast");

	hInstance = GetModuleHandle(NULL);				// Grab An Instance For Our Window

	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance	
	wc.hIcon			= (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1 ), IMAGE_ICON, 60,57,0);		// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= (HBRUSH) (COLOR_WINDOW + 1);			// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu	
	wc.lpszClassName	= szAppName;							// Set The Class Name
	wc.hIconSm			= (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1 ), IMAGE_ICON, 16,16,0);
	
	if (!RegisterClassEx(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,
			L"Fallo al registrar clase (Failed To Register The Window Class).",
			L"ERROR",
			MB_OK|MB_ICONEXCLAMATION);
		return 0;
	}

	hWnd = CreateWindowEx(	
		WS_EX_CLIENTEDGE | WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,	// Extended Style For The Window
		szAppName,							// Class Name
		szAppTitle,							// Window Title
		WS_OVERLAPPEDWINDOW |				// Defined Window Style
		WS_CLIPSIBLINGS |					// Required Window Style
		WS_CLIPCHILDREN,					// Required Window Style
		0, 0,								// Window Position
		ANCHO_VENTANA,						// Calculate Window Width
		ALTO_VENTANA,						// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		hInstance,							// Instance
		NULL);								// Pass this class To WM_CREATE								

	if(hWnd == NULL) {
		MessageBox(NULL, 
			L"Error al crear ventana (Window Creation Error).", 
			L"ERROR", 
			MB_OK|MB_ICONEXCLAMATION);
		return 0;
	}
		
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	Init();
	ShowWindow(hWnd, nCmdShow);
	SetFocus(hWnd);

	SetTimer(hWnd, TICK, TICK, NULL);
	ZeroMemory(&msg, sizeof(MSG));

	while(GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return(int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	switch(uMsg)									// Check For Windows Messages
	{
		case WM_CREATE:
			tonoazul = 0;
			tonoverde=0;
			tonorojo=0;
		break;
		case WM_TIMER:
			if(wParam == TICK)
			{
				
				MainRender(hWnd);
			}
			break;
		case WM_PAINT:
			{
				HDC hdc; 
				PAINTSTRUCT ps;
				hdc = BeginPaint(hWnd, &ps);
				// TODO: agregar código de dibujo aquí...

				//BITMAP bm;
				HBITMAP h_CMC = CreateBitmap(ANCHO_VENTANA, ALTO_VENTANA, 1, 32, ptrBuffer);
				HDC hdcMem = CreateCompatibleDC(hdc);
				HBITMAP hbmOld = (HBITMAP) SelectObject(hdcMem, h_CMC);
				//GetObject(h_CMC, sizeof(bm), &bm);

				BitBlt(hdc, 0, 0, ANCHO_VENTANA, ALTO_VENTANA, hdcMem, 0, 0, SRCCOPY);

				DeleteObject(h_CMC);
				SelectObject(hdcMem, hbmOld);
				DeleteDC(hdcMem);
				//DeleteObject(hbmOld);
			}
			break;		
		case WM_KEYDOWN:							
			{
				switch(wParam){
			case input.B:
				tonoazul++;
				break;
			case input.N:
				tonoazul--;
				break;

			
			}

				KEYS[ wParam ] = true;
			}
			break;
		case WM_KEYUP:
			{
				KEYS[ wParam ] = false;
			}
			break;
		case WM_CLOSE: 
			{
				DestroyWindow(hWnd);
			}
			break;
		case WM_DESTROY: //Send A Quit Message
			{
				KillTimer(hWnd, TICK);
				PostQuitMessage(0);
			}
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}
int delayy[9];
/* Inicializacion de variables y reserva de memoria.
	*/
void Init() 
{	 
	vidas=3;score=0;
	posFigura.X = (ANCHO_VENTANA-84)/2;
	posFigura.Y = 600-150;
	dmnFigura.ANCHO = 10;
	dmnFigura.ALTO = 10;
	for(int r=0;r<6;r++ ){
		PosAst[r].X=rand()%800;PosAst[r].Y=0;
	
	delayy[r]=rand()%50;}
	for(int i = 0; i < 256; i++)
	{
		KEYS[i] = false;
	}
	ptrBuffer = new int[ANCHO_VENTANA * ALTO_VENTANA];
	mbmp = gdipLoad(L"SHIP.bmp");
	background =gdipLoad(L"bkground.bmp");
	asteroide=gdipLoad(L"AST.bmp");
	Marco=gdipLoad(L"Score.bmp");
	vida=gdipLoad(L"vida.bmp");
		corazones =new cuadro[4];
		corazones[0].x = 0; corazones[0].y = 0;
		corazones[0].ancho = 73; corazones[0].alto = 61;
		corazones[1].x = 73; corazones[1].y = 0;
		corazones[1].ancho = 73; corazones[1].alto = 61;
		corazones[2].x = 0; corazones[2].y = 64;
		corazones[2].ancho = 149; corazones[2].alto = 41;
		corazones[3].x = 1; corazones[3].y = 114;
		corazones[3].ancho = 423+44; corazones[3].alto = 121;
		number =new cuadro[10];
		for(int j=0;j<10;j++){
		number[j].x = 15+(j*33); number[j].y = 248;
		number[j].ancho = 22; number[j].alto = 40;
		}

		HaciaDer= new cuadro[2];
		HaciaDer[0].x = 85; HaciaDer[0].y = 10;
		HaciaDer[0].ancho = 84; HaciaDer[0].alto = 70;
		HaciaDer[1].x = 85; HaciaDer[1].y = 92;
		HaciaDer[1].ancho = 84; HaciaDer[1].alto = 70;
		frameDer = 0;

		HaciaIzq= new cuadro[2];
	HaciaIzq[0].x = 85; HaciaIzq[0].y = 325;
	HaciaIzq[0].ancho = 84; HaciaIzq[0].alto = 70;
	HaciaIzq[1].x = 85; HaciaIzq[1].y = 405;
	HaciaIzq[1].ancho = 84; HaciaIzq[1].alto = 70;

	HaciaRecto =new cuadro[2];
	HaciaRecto[0].x = 85; HaciaRecto[0].y = 170;
	HaciaRecto[0].ancho = 84; HaciaRecto[0].alto = 70;
	HaciaRecto[1].x = 85; HaciaRecto[1].y = 240;
	HaciaRecto[1].ancho = 84; HaciaRecto[1].alto = 69;

	Golpe=new cuadro[4];
	Golpe[0].x =5;       Golpe[0].y = 132;
	Golpe[0].ancho = 57; Golpe[0].alto = 65;
	Golpe[1].x =5;       Golpe[1].y = 196;
	Golpe[1].ancho = 57; Golpe[1].alto = 65;
	Golpe[2].x =5;       Golpe[2].y = 260;
	Golpe[2].ancho = 57; Golpe[2].alto = 65;
	Golpe[3].x =5;       Golpe[3].y = 335;
	Golpe[3].ancho = 57; Golpe[3].alto = 65;
	framegolpe=0;
		frameDer = 0;

	bala=new cuadro[1];
	bala[0].x =170;       bala[0].y = 4;
	bala[0].ancho = 14; bala[0].alto = 21;
	
		FondoMov=new cuadro[(background.alto-800)];
		for(int i=0; i<(background.alto-800)-1;i++){
		FondoMov[i].x = 85; FondoMov[i].y = 170+i;
		FondoMov[i].ancho = 800; FondoMov[i].alto = 600;
		}

	frameactual = ( (background.alto-800)-2);
	
}

/* Funcion principal. Encargada de hacer el redibujado en pantalla cada intervalo (o "tick") del timer que se haya creado.
	@param hWnd. Manejador de la ventana.
	*/


void Nave_F(){
	
	
	if (golpe){
		MovDer=false;
 MovIzq=false;
 MovRecto=false;
 NoMov=false;
	autotransparentblt(ptrBuffer,(mbmp.pixeles),posFigura.X+13,posFigura.Y+3,Golpe[framegolpe].x,Golpe[framegolpe].y,Golpe[framegolpe].ancho,Golpe[framegolpe].alto,800,mbmp.ancho/*,0xFF00FF00*/);
			framegolpe++;
			if(framegolpe>3){
			framegolpe=0;
			}	
			
			golpe=false;	

   	}
	if (MovDer){
	NoMov=false;
	MovIzq=false;
 MovRecto=false;
 golpe=false;
	autotransparentblt(ptrBuffer,(mbmp.pixeles),
	posFigura.X,posFigura.Y,
					HaciaDer[frameDer].x,HaciaDer[frameDer].y,
					HaciaDer[frameDer].ancho,HaciaDer[frameDer].alto,
					800,mbmp.ancho);
	
		
	frameDer++;
	if(frameDer > 1 ){ frameDer = 0;}
	MovDer=false;
	

 
	}

	if(MovIzq){
	NoMov=false;
	MovIzq=false;
 MovDer=false;
 MovRecto=false;
 golpe=false;
	autotransparentblt(ptrBuffer,(mbmp.pixeles),
	posFigura.X,posFigura.Y,
					HaciaIzq[frameDer].x,HaciaIzq[frameDer].y,
					HaciaIzq[frameDer].ancho,HaciaIzq[frameDer].alto,
					800,mbmp.ancho);
	
		
	frameDer++;
	if(frameDer > 1 ){ frameDer = 0;}
 
	}
    if((!MovIzq)&&(!MovDer)&& MovRecto ){
	NoMov=false;
	MovDer=false;
	MovIzq=false;
	golpe=false;
	
	autotransparentblt(ptrBuffer,(mbmp.pixeles),
	posFigura.X,posFigura.Y,
					HaciaRecto[frameDer].x,HaciaRecto[frameDer].y,
					HaciaRecto[frameDer].ancho,HaciaRecto[frameDer].alto,
					800,mbmp.ancho);
	frameDer++;
	if(frameDer > 1 ){ frameDer = 0;}
	 MovRecto=false;


	}

	if (NoMov){
	MovRecto=false;
	MovDer=false;
	MovIzq=false;
	golpe=false;

	autotransparentblt(ptrBuffer,(mbmp.pixeles),
	posFigura.X,posFigura.Y,
					HaciaRecto[frameDer].x,HaciaRecto[frameDer].y,
					HaciaRecto[frameDer].ancho,HaciaRecto[frameDer].alto,
					800,mbmp.ancho);
	frameDer++;
	if(frameDer > 1 ){ frameDer = 0;}
	NoMov=false;
	}
	
	

}
int cuadroFondo=0;
void Fondo(){

	mirrorxblt(ptrBuffer,(background.pixeles),0,0,0,FondoMov[cuadroFondo].y,800,600,800,background.ancho);
	


	cuadroFondo-=8;
	if(cuadroFondo < 0){ cuadroFondo =( (background.alto-800)-2);}
}
void  Nave_vs_Ast()	{

for(int j=0;j<6;j++){
	for(int i=0;i<75;i++)
	{for(int k=0;k<70;k++){
	NaveHit=Rect(posFigura.X, posFigura.Y, k, k);
	AstHit=Rect(PosAst[j].X, PosAst[j].Y, i,i);// 75*83
	if (NaveHit.IntersectsWith(AstHit))
	{
		PlaySound(L"explosion-02.wav",NULL, SND_FILENAME | SND_ASYNC );
	
		vidas--;
		NoMov=false;
		MovRecto=false;
		MovIzq=false;
		MovDer=false;
		golpe=true;
		PosAst[j].X=rand()%800;PosAst[j].Y=0;

			}

	}
	}}
}

void  bala_vs_Ast()	{

for(int j=0;j<6;j++){
	for(int i=0;i<75;i++)
	
	AstHit=Rect(PosAst[j].X, PosAst[j].Y, i,i);// 75*83
	balaHit=Rect(posBala.X,posBala.Y,bala[0].ancho,bala[0].alto);
	if (balaHit.IntersectsWith(AstHit))
	{
		PlaySound(L"explosion-02.wav",NULL, SND_FILENAME | SND_ASYNC );
	 posBala.Y=0;
	posBala.X=0;
		disparo=false;
		PosAst[j].X=rand()%800;PosAst[j].Y=0;

			}


	}
}


char  puntuaciondos[6]="00000";
void actual_game(){
		
	Start=false;
	if(tonoazul != 0)
	cambiarTonalidadAzul(asteroide.pixeles,asteroide.ancho,asteroide.alto,tonoazul);
	if(tonoazul == 0)
		cambiarTonalidadAzul(asteroide.pixeles,asteroide.ancho,asteroide.alto,100);
KeysEvents();
	
	NoMov=true;


	Asteroids();
	transparentblt(ptrBuffer,(Marco.pixeles),0,0,0,0,800,600,800,Marco.ancho,0xFF00FF00);
	
	if (game_over ==false)
	{
	score++;
	if(score>99999){
	score=99999;
		
	}
	Nave_vs_Ast();
	Nave_F();
	}
    BarradeVida();

	
	if (vidas<=0){
	game_over=true;	
    transparentblt(ptrBuffer,(vida.pixeles),736+corazones[3].ancho/2,300-corazones[3].alto/2,corazones[3].x,corazones[3].y,corazones[3].ancho,corazones[3].alto,800,vida.ancho,0xFF00FF00);
	for(int u=0;u<5;u++){
		
	scaleblt(ptrBuffer,(vida.pixeles),(736+corazones[3].ancho/*+40*/)+(u*22*2),280,number[puntuaciondos[u]-48].x,number[puntuaciondos[u]-48].y,number[puntuaciondos[u]-48].ancho,number[puntuaciondos[u]-48].alto,800,vida.ancho,2,1);
	
	if(Start==true){
		strcpy(puntuaciondos,"00000");
		score=0;vidas=3;
		
		
	game_over=false;
	Start=false;
	score=0;vidas=3;
	}
	}
	}
	
	
}
void BarradeVida(){

	transparentblt(ptrBuffer,(vida.pixeles),(760-corazones[2].ancho)-(corazones[0].ancho*3),15,corazones[2].x,corazones[2].y,corazones[2].ancho,corazones[2].alto,800,vida.ancho,0xFF00FF00);

for(int i=0;i<3;i++){
	
transparentblt(ptrBuffer,(vida.pixeles),(760-corazones[1].ancho)-(corazones[1].ancho*i),10,corazones[1].x,corazones[1].y,corazones[1].ancho,corazones[1].alto,800,vida.ancho,0xFF00FF00);
	}

	int coordenada=(760-corazones[1].ancho)-(corazones[1].ancho*2);
	for(int j=0;j<vidas;j++){
		
	transparentblt(ptrBuffer,(vida.pixeles),coordenada+corazones[1].ancho*j,10,corazones[0].x,corazones[0].y,corazones[0].ancho,corazones[0].alto,800,vida.ancho,0xFF00FF00);
	}


	char puntuacion[6];
	
	
itoa (score,puntuacion,10);
int tamaño=strlen(puntuacion);
for(int s=0;s<tamaño;s++){
	puntuaciondos[5-tamaño+s]=puntuacion[s];
}
for(int u=0;u<5;u++){
	
	transparentblt(ptrBuffer,(vida.pixeles),160+(u*22),20,number[puntuaciondos[u]-48].x,number[puntuaciondos[u]-48].y,number[puntuaciondos[u]-48].ancho,number[puntuaciondos[u]-48].alto,800,vida.ancho,0xFF00FF00);

	}
	
}
void Menu(){
	KeysEvents();
	Nave_F();
	NoMov=true;
}
void bala_f(){
if (disparo==true){
		posBala.Y=posFigura.Y;
	posBala.X=posFigura.X+35;
	balaHit=Rect(posBala.X,posBala.Y,bala[0].ancho,bala[0].alto);
	disparo=false;

if (posBala.Y<=58){
	disparo=false;
	
}
	
}
	if (posBala.Y>58){
	posBala.Y-=9;
	
}
	


transparentblt(ptrBuffer,(mbmp.pixeles),posBala.X,posBala.Y,bala[0].x,bala[0].y,bala[0].ancho,bala[0].alto,800,mbmp.ancho,0xFF00FF00);
};
void MainRender(HWND hWnd) 
{
LimpiarFondo(ptrBuffer, 000000, (ANCHO_VENTANA * ALTO_VENTANA));
	
	
	Fondo();
	
	mciSendString(L"play \Xilent_Boss_Wave.mp3 repeat",0,0,0);

	bala_f();
	 bala_vs_Ast();
actual_game( );




InvalidateRect(hWnd, NULL, FALSE);
	UpdateWindow(hWnd);
}

void Asteroids(){
	

	for(int i=0;i<6;i++){

	if(delayy[i]<0){
	if(PosAst[i].Y>600-asteroide.alto-15){
	PosAst[i].Y=1;
	PosAst[i].X=rand()%800/*400*/;
	}
	autotransparentblt(ptrBuffer,(asteroide.pixeles),PosAst[i].X,PosAst[i].Y+=15,0,0,asteroide.ancho,asteroide.alto,800,asteroide.ancho);
     	delayy[i]=rand()%1;
	
	}
	delayy[i]--;
	}
}

void KeysEvents() 
{
	if(KEYS[input.M ] ){
		mirrorxblt(ptrBuffer,(background.pixeles),0,0,0,FondoMov[cuadroFondo].y,800,600,800,background.ancho);
	
	}
	
	if(KEYS[input.Space ] ){
	Start=true;
	if(posBala.Y<=58){
	disparo=true;
	}
	
	}

	if(KEYS[input.F2] ){
		if(vidas<10)
		vidas++;
	}

	if(KEYS[input.W] || KEYS[input.Up]&&posFigura.Y>70 )
	{ if (posFigura.Y>5)
		posFigura.Y-=8; 
	MovRecto=true;
	}
	if(KEYS[input.D] || KEYS[input.Right]&&posFigura.X<700)
	{
		MovDer=true;
		posFigura.X+=15;
		
	}
	if(KEYS[input.S] || KEYS[input.Down])
	{	
		MovRecto=true;
		if  (posFigura.Y<500)
		posFigura.Y+=8;
	}
	if(KEYS[input.A] || KEYS[input.Left]&&posFigura.X>0)
	{   
		MovIzq=true;

		posFigura.X-=15;
	}

	
}

#pragma region LENS_CODE
/* Pinta el fondo de la ventana de acuerdo al color especificado.
	@param *ptrBuffer.	Puntero al area de memoria reservada para el proceso de dibujado.
	@param color.		Color expresado en formato hexadecimal.
	@param area.		Area de la ventana.
	*/
void LimpiarFondo(int *ptrBuffer, unsigned int color, int area) 
{
	__asm {
			mov edi, ptrBuffer
			mov ecx, area
			mov eax, color

			//draw: 	
			//	mov [edi], eax
			//	add edi, BPP
			//loop draw

			rep stosd
	}
}

/* Funcion que pinta una figura rectangular en pantalla.
	@param *ptrBuffer.	Puntero al area de memoria reservada para el dibujado.
	@param color.		Color de la figura expresado en formato hexadecimal.
	@param anchoWnd.	Ancho total de la ventana.
	@param altoWnd.		Alto total de la ventana.
	@param dmnFigura.	Especifica las dimensiones de la figura en relacion con la ventana.
	@param posFigura.	Posiciona la figura en la ventana.
	*/
void DibujaFigura(int *ptrBuffer, unsigned int color, int anchoWnd, int altoWnd, DIMENSION dmnFigura, POSITION posFigura)
{
	int x = posFigura.X;
	int y = posFigura.Y;
	int ancho = dmnFigura.ANCHO;
	int alto = dmnFigura.ALTO;
	__asm {
			mov edi, ptrBuffer

			mov eax, y
			mul BPP
			mul anchoWnd
			add edi, eax

			mov eax, x
			mul BPP
			add edi, eax

			mov eax, color

			mov ecx, alto
			lazo_alto:
				push ecx
				mov ecx, ancho
				lazo_ancho:
					mov [edi], eax
					add edi, BPP
				//stosd
				loop lazo_ancho
				push eax
				mov eax, anchoWnd
				sub eax, ancho
				mul BPP
				add edi, eax
				pop eax
				pop ecx
			loop lazo_alto
	}
}

	void imageblt(int* punteroDestino, int* punteroOrigen, int inicioXDestino, int inicioYDestino, int inicioXOrigen, int inicioYOrigen, int ancho, int alto, int anchodefondo, int anchodeorigen){
		//blt = block transfer, transferencia de bloque de imagen
		int bytesporlineafondo = anchodefondo*4;
		int bytesporlineaorigen = anchodeorigen*4;
		int bytesporlineaimagen = ancho*4;
		
		__asm{
			
			mov edi, punteroDestino //movemos la direccion del bitmap a edi, para poder escribir en el
			//Conseguimos el pixel inicial donde empezaremos a dibujar
			mov eax, inicioYDestino 
			mul bytesporlineafondo //inicioY * bytesporlineafondo, asi iniciamos en la linea donde queremos 
			mov ebx,eax //ebx contendra el resultado anterior
			mov eax, 4 
			mul inicioXDestino //inicioX*4, para asi encontrar la columnda donde queremos empezar a dibujar
			add eax, ebx //posicion de columna + posicion de linea
			add edi,eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada

			mov esi, punteroOrigen //movemos la direccion de la imagen a dibujar a esi, para poder escribir de ella
			//Conseguimos el pixel inicial DEL CUAL empezaremos a dibujar
			mov eax, inicioYOrigen 
			mul bytesporlineaorigen //inicioY * bytesporlineaorigen, asi iniciamos en la linea donde queremos 
			mov ebx,eax //ebx contendra el resultado anterior
			mov eax, 4 
			mul inicioXOrigen //inicioX*4, para asi encontrar la columnda de donde queremos empezar a leer
			add eax, ebx //posicion de columna + posicion de linea
			add esi,eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada

			mov ecx, alto //movemos a ecx la cantidad de lineas que dibujaremos
lazollenarY:
			push ecx //guardamos el valor anterior de ecx, porque lo reemplazaremos en un nuevo ciclo
			mov ecx, ancho //la cantidad de columnas que dibujaremos
	lazollenarX:
				movsd //mueve un pixel de la direccion apuntada por esi a la apuntada por edi, e incrementa esi y edi por 4

				loop lazollenarX

				add edi, bytesporlineafondo //le sumamos la cantidad de bytes de la linea del fondo para pasar a la siguiente linea
				sub edi, bytesporlineaimagen //y retrocedemos una cantidad igual al a su ancho para dibujar desde la posicion X inicial y que no quede escalonado

				//Lo mismo para esi
				add esi, bytesporlineaorigen
				sub esi, bytesporlineaimagen

				pop ecx //recuperamos el valor del contador del ciclo exterior
			loop lazollenarY
		}

	}

	void transparentblt(int* punteroDestino, int* punteroOrigen, int inicioXDestino, int inicioYDestino, int inicioXOrigen, int inicioYOrigen, int ancho, int alto, int anchodefondo, int anchodeorigen, int colortransparente){
		//blt = block transfer, transferencia de bloque de imagen
		int bytesporlineafondo = anchodefondo*4;
		int bytesporlineaorigen = anchodeorigen*4;
		int bytesporlineaimagen = ancho*4;
		
		__asm{
			
			mov edi, punteroDestino //movemos la direccion del bitmap a edi, para poder escribir en el
			//Conseguimos el pixel inicial donde empezaremos a dibujar
			mov eax, inicioYDestino 
			mul bytesporlineafondo //inicioY * bytesporlineafondo, asi iniciamos en la linea donde queremos 
			mov ebx,eax //ebx contendra el resultado anterior
			mov eax, 4 
			mul inicioXDestino //inicioX*4, para asi encontrar la columnda donde queremos empezar a dibujar
			add eax, ebx //posicion de columna + posicion de linea
			add edi,eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada

			mov esi, punteroOrigen //movemos la direccion de la imagen a dibujar a esi, para poder escribir de ella
			//Conseguimos el pixel inicial DEL CUAL empezaremos a dibujar
			mov eax, inicioYOrigen 
			mul bytesporlineaorigen //inicioY * bytesporlineaorigen, asi iniciamos en la linea donde queremos 
			mov ebx,eax //ebx contendra el resultado anterior
			mov eax, 4 
			mul inicioXOrigen //inicioX*4, para asi encontrar la columnda de donde queremos empezar a leer
			add eax, ebx //posicion de columna + posicion de linea
			add esi,eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada

			mov eax, colortransparente

			mov ecx, alto //movemos a ecx la cantidad de lineas que dibujaremos
lazollenarY:
			push ecx //guardamos el valor anterior de ecx, porque lo reemplazaremos en un nuevo ciclo
			mov ecx, ancho //la cantidad de columnas que dibujaremos

	lazollenarX:
				//movsd //mueve un pixel de la direccion apuntada por esi a la apuntada por edi, e incrementa esi y edi por 4

			cmp eax, [esi]
			je nodibujar

				mov edx, [esi]
				mov [edi],edx

nodibujar:
				add esi,4
				add edi,4

				loop lazollenarX

				add edi, bytesporlineafondo //le sumamos la cantidad de bytes de la linea del fondo para pasar a la siguiente linea
				sub edi, bytesporlineaimagen //y retrocedemos una cantidad igual al a su ancho para dibujar desde la posicion X inicial y que no quede escalonado

				//Lo mismo para esi
				add esi, bytesporlineaorigen
				sub esi, bytesporlineaimagen

				pop ecx //recuperamos el valor del contador del ciclo exterior
			loop lazollenarY
		}

	}

	void autotransparentblt(int* punteroDestino, int* punteroOrigen, int inicioXDestino, int inicioYDestino, int inicioXOrigen, int inicioYOrigen, int ancho, int alto, int anchodefondo, int anchodeorigen){
		//blt = block transfer, transferencia de bloque de imagen
		int bytesporlineafondo = anchodefondo*4;
		int bytesporlineaorigen = anchodeorigen*4;
		int bytesporlineaimagen = ancho*4;
		
		__asm{
			
			mov edi, punteroDestino //movemos la direccion del bitmap a edi, para poder escribir en el
			//Conseguimos el pixel inicial donde empezaremos a dibujar
			mov eax, inicioYDestino 
			mul bytesporlineafondo //inicioY * bytesporlineafondo, asi iniciamos en la linea donde queremos 
			mov ebx,eax //ebx contendra el resultado anterior
			mov eax, 4 
			mul inicioXDestino //inicioX*4, para asi encontrar la columnda donde queremos empezar a dibujar
			add eax, ebx //posicion de columna + posicion de linea
			add edi,eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada

			mov esi, punteroOrigen //movemos la direccion de la imagen a dibujar a esi, para poder escribir de ella
			//Conseguimos el pixel inicial DEL CUAL empezaremos a dibujar
			mov eax, inicioYOrigen 
			mul bytesporlineaorigen //inicioY * bytesporlineaorigen, asi iniciamos en la linea donde queremos 
			mov ebx,eax //ebx contendra el resultado anterior
			mov eax, 4 
			mul inicioXOrigen //inicioX*4, para asi encontrar la columnda de donde queremos empezar a leer
			add eax, ebx //posicion de columna + posicion de linea
			add esi,eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada

			
			mov eax, [esi]

			mov ecx, alto //movemos a ecx la cantidad de lineas que dibujaremos
lazollenarY:
			push ecx //guardamos el valor anterior de ecx, porque lo reemplazaremos en un nuevo ciclo
			mov ecx, ancho //la cantidad de columnas que dibujaremos

	lazollenarX:
				//movsd //mueve un pixel de la direccion apuntada por esi a la apuntada por edi, e incrementa esi y edi por 4

			cmp eax, [esi]
			je nodibujar

				mov edx, [esi]
				mov [edi],edx

nodibujar:
				add esi,4
				add edi,4

				loop lazollenarX

				add edi, bytesporlineafondo //le sumamos la cantidad de bytes de la linea del fondo para pasar a la siguiente linea
				sub edi, bytesporlineaimagen //y retrocedemos una cantidad igual al a su ancho para dibujar desde la posicion X inicial y que no quede escalonado

				//Lo mismo para esi
				add esi, bytesporlineaorigen
				sub esi, bytesporlineaimagen

				pop ecx //recuperamos el valor del contador del ciclo exterior
			loop lazollenarY
		}

	}


	void mirrorxblt(int* punteroDestino, int* punteroOrigen, int inicioXDestino, int inicioYDestino, int inicioXOrigen, int inicioYOrigen, int ancho, int alto, int anchodefondo, int anchodeorigen){
		//blt = block transfer, transferencia de bloque de imagen
		int bytesporlineafondo = anchodefondo*4;
		int bytesporlineaorigen = anchodeorigen*4;
		int bytesporlineaimagen = ancho*4;
		
		__asm{
			
			mov edi, punteroDestino //movemos la direccion del bitmap a edi, para poder escribir en el
			//Conseguimos el pixel inicial donde empezaremos a dibujar
			mov eax, inicioYDestino 
			mul bytesporlineafondo //inicioY * bytesporlineafondo, asi iniciamos en la linea donde queremos 
			mov ebx,eax //ebx contendra el resultado anterior
			mov eax, 4 
			mul inicioXDestino //inicioX*4, para asi encontrar la columnda donde queremos empezar a dibujar
			add eax, ebx //posicion de columna + posicion de linea
			add edi,eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada

			mov esi, punteroOrigen //movemos la direccion de la imagen a dibujar a esi, para poder escribir de ella
			//Conseguimos el pixel inicial DEL CUAL empezaremos a dibujar
			mov eax, inicioYOrigen 
			mul bytesporlineaorigen //inicioY * bytesporlineaorigen, asi iniciamos en la linea donde queremos 
			mov ebx,eax //ebx contendra el resultado anterior
			mov eax, 4 
			mul inicioXOrigen //inicioX*4, para asi encontrar la columnda de donde queremos empezar a leer
			add eax, ebx //posicion de columna + posicion de linea
			add esi,eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada

			
			mov eax, [esi]

			mov ecx, alto //movemos a ecx la cantidad de lineas que dibujaremos
lazollenarY:
			push ecx //guardamos el valor anterior de ecx, porque lo reemplazaremos en un nuevo ciclo
			mov ecx, ancho //la cantidad de columnas que dibujaremos

			add esi, bytesporlineaimagen
			sub esi, 4

	lazollenarX:
				//movsd //mueve un pixel de la direccion apuntada por esi a la apuntada por edi, e incrementa esi y edi por 4

			cmp eax, [esi]
			je nodibujar

				mov edx, [esi]
				mov [edi],edx

nodibujar:
				sub esi,4
				add edi,4

				loop lazollenarX

				add edi, bytesporlineafondo //le sumamos la cantidad de bytes de la linea del fondo para pasar a la siguiente linea
				sub edi, bytesporlineaimagen //y retrocedemos una cantidad igual al a su ancho para dibujar desde la posicion X inicial y que no quede escalonado

				//Lo mismo para esi
				add esi, bytesporlineaorigen
				add esi, 4
				//sub esi, bytesporlineaimagen

				pop ecx //recuperamos el valor del contador del ciclo exterior
			loop lazollenarY
		}

	}

	void mirroryblt(int* punteroDestino, int* punteroOrigen, int inicioXDestino, int inicioYDestino, int inicioXOrigen, int inicioYOrigen, int ancho, int alto, int anchodefondo, int anchodeorigen){
		//blt = block transfer, transferencia de bloque de imagen
		int bytesporlineafondo = anchodefondo*4;
		int bytesporlineaorigen = anchodeorigen*4;
		int bytesporlineaimagen = ancho*4;
		
		__asm{
			
			mov edi, punteroDestino //movemos la direccion del bitmap a edi, para poder escribir en el
			//Conseguimos el pixel inicial donde empezaremos a dibujar
			mov eax, inicioYDestino 
			mul bytesporlineafondo //inicioY * bytesporlineafondo, asi iniciamos en la linea donde queremos 
			mov ebx,eax //ebx contendra el resultado anterior
			mov eax, 4 
			mul inicioXDestino //inicioX*4, para asi encontrar la columnda donde queremos empezar a dibujar
			add eax, ebx //posicion de columna + posicion de linea
			add edi,eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada

			mov esi, punteroOrigen //movemos la direccion de la imagen a dibujar a esi, para poder escribir de ella
			//Conseguimos el pixel inicial DEL CUAL empezaremos a dibujar
			mov eax, inicioYOrigen
			add eax, alto
			dec eax
			mul bytesporlineaorigen //inicioY * bytesporlineaorigen, asi iniciamos en la linea donde queremos 
			mov ebx,eax //ebx contendra el resultado anterior
			mov eax, 4 
			mul inicioXOrigen //inicioX*4, para asi encontrar la columnda de donde queremos empezar a leer
			add eax, ebx //posicion de columna + posicion de linea
			add esi,eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada

			
			mov eax, [esi]

			mov ecx, alto //movemos a ecx la cantidad de lineas que dibujaremos
lazollenarY:
			push ecx //guardamos el valor anterior de ecx, porque lo reemplazaremos en un nuevo ciclo
			mov ecx, ancho //la cantidad de columnas que dibujaremos

	lazollenarX:
				//movsd //mueve un pixel de la direccion apuntada por esi a la apuntada por edi, e incrementa esi y edi por 4

			cmp eax, [esi]
			je nodibujar

				mov edx, [esi]
				mov [edi],edx

nodibujar:
				add esi,4
				add edi,4

				loop lazollenarX

				add edi, bytesporlineafondo //le sumamos la cantidad de bytes de la linea del fondo para pasar a la siguiente linea
				sub edi, bytesporlineaimagen //y retrocedemos una cantidad igual al a su ancho para dibujar desde la posicion X inicial y que no quede escalonado

				//Lo mismo para esi
				sub esi, bytesporlineaorigen
				sub esi, bytesporlineaimagen

				pop ecx //recuperamos el valor del contador del ciclo exterior
			loop lazollenarY
		}

	}

		void mirrorxyblt(int* punteroDestino, int* punteroOrigen, int inicioXDestino, int inicioYDestino, int inicioXOrigen, int inicioYOrigen, int ancho, int alto, int anchodefondo, int anchodeorigen){
		//blt = block transfer, transferencia de bloque de imagen
		int bytesporlineafondo = anchodefondo*4;
		int bytesporlineaorigen = anchodeorigen*4;
		int bytesporlineaimagen = ancho*4;
		
		__asm{
			
			mov edi, punteroDestino //movemos la direccion del bitmap a edi, para poder escribir en el
			//Conseguimos el pixel inicial donde empezaremos a dibujar
			mov eax, inicioYDestino 
			mul bytesporlineafondo //inicioY * bytesporlineafondo, asi iniciamos en la linea donde queremos 
			mov ebx,eax //ebx contendra el resultado anterior
			mov eax, 4 
			mul inicioXDestino //inicioX*4, para asi encontrar la columnda donde queremos empezar a dibujar
			add eax, ebx //posicion de columna + posicion de linea
			add edi,eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada

			mov esi, punteroOrigen //movemos la direccion de la imagen a dibujar a esi, para poder escribir de ella
			//Conseguimos el pixel inicial DEL CUAL empezaremos a dibujar
			mov eax, inicioYOrigen
			add eax, alto
			dec eax
			mul bytesporlineaorigen //inicioY * bytesporlineaorigen, asi iniciamos en la linea donde queremos 
			mov ebx,eax //ebx contendra el resultado anterior
			mov eax, 4 
			mul inicioXOrigen //inicioX*4, para asi encontrar la columnda de donde queremos empezar a leer
			add eax, ebx //posicion de columna + posicion de linea
			add esi,eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada

			
			mov eax, [esi]

			mov ecx, alto //movemos a ecx la cantidad de lineas que dibujaremos
lazollenarY:
			add esi, bytesporlineaimagen
			sub esi, 4

			push ecx //guardamos el valor anterior de ecx, porque lo reemplazaremos en un nuevo ciclo
			mov ecx, ancho //la cantidad de columnas que dibujaremos
	lazollenarX:
				//movsd //mueve un pixel de la direccion apuntada por esi a la apuntada por edi, e incrementa esi y edi por 4

			cmp eax, [esi]
			je nodibujar

				mov edx, [esi]
				mov [edi],edx

nodibujar:
				sub esi,4
				add edi,4

				loop lazollenarX

				add edi, bytesporlineafondo //le sumamos la cantidad de bytes de la linea del fondo para pasar a la siguiente linea
				sub edi, bytesporlineaimagen //y retrocedemos una cantidad igual al a su ancho para dibujar desde la posicion X inicial y que no quede escalonado

				//Lo mismo para esi
				sub esi, bytesporlineaorigen
				add esi,4
				//sub esi, bytesporlineaimagen

				pop ecx //recuperamos el valor del contador del ciclo exterior
			loop lazollenarY
		}

	}
		void cambiarTonalidadAzul(int* punteroImagen, int ancho, int alto, unsigned char color){

		int totalpixels = ancho * alto;
		//0xFFFFFF00
		__asm{

			mov edi,punteroImagen
			mov ecx, totalpixels
			tonoloop:
			mov eax,0xFFFFFF00
			and [edi],eax ;Limpia el componente azul de la imagen
			;Mascaras para los otros colores:
			;Rojo: 0xFF00FFFF
			;Verde: 0xFFFF00FF
			;Azul: 0xFFFFFF00
			xor eax,eax
			mov al,color
			;shl eax,16 ;Ponemos el valor del color en eax, luego lo movemos a la izquierda para que quede en el lugar del componente rojo
			;Cantidad para el rojo: 16
			;Cantidad para el color verde: 8
			;Cantidad para azul: 0
			add [edi],eax

			add edi, 4
			loop tonoloop

		}

	}

void cambiarTonalidadVerde(int* punteroImagen, int ancho, int alto, unsigned char color){

		int totalpixels = ancho * alto;
		//0xFFFFFF00
		__asm{

			mov edi,punteroImagen
			mov ecx, totalpixels
			tonoloop:
			mov eax,0xFFFF00FF
			and [edi],eax ;Limpia el componente azul de la imagen
			;Mascaras para los otros colores:
			;Rojo: 0xFF00FFFF
			;Verde: 0xFFFF00FF
			;Azul: 0xFFFFFF00
			xor eax,eax
			mov al,color
			;shl eax,16 ;Ponemos el valor del color en eax, luego lo movemos a la izquierda para que quede en el lugar del componente rojo
			;Cantidad para el rojo: 16
			;Cantidad para el color verde: 8
			;Cantidad para azul: 0
			add [edi],eax

			add edi, 4
			loop tonoloop

		}

	}

void cambiarTonalidadRojo(int* punteroImagen, int ancho, int alto, unsigned char color){

		int totalpixels = ancho * alto;
		//0xFFFFFF00
		__asm{

			mov edi,punteroImagen
			mov ecx, totalpixels
			tonoloop:
			mov eax,0xFF00FFFF
			and [edi],eax ;Limpia el componente azul de la imagen
			;Mascaras para los otros colores:
			;Rojo: 0xFF00FFFF
			;Verde: 0xFFFF00FF
			;Azul: 0xFFFFFF00
			xor eax,eax
			mov al,color
			;shl eax,16 ;Ponemos el valor del color en eax, luego lo movemos a la izquierda para que quede en el lugar del componente rojo
			;Cantidad para el rojo: 16
			;Cantidad para el color verde: 8
			;Cantidad para azul: 0
			add [edi],eax

			add edi, 4
			loop tonoloop

		}

	}


void scaleblt(int* punteroDestino, int* punteroOrigen, int inicioXDestino, int inicioYDestino, int inicioXOrigen, int inicioYOrigen, int ancho, int alto, int anchodefondo, int anchodeorigen, int escalaX, int escalaY){
		//blt = block transfer, transferencia de bloque de imagen
		int bytesporlineafondo = anchodefondo*4;
		int bytesporlineaorigen = anchodeorigen*4;
		int bytesporlineaimagen = ancho*4;
		
		__asm{
			
			mov edi, punteroDestino //movemos la direccion del bitmap a edi, para poder escribir en el
			//Conseguimos el pixel inicial donde empezaremos a dibujar
			mov eax, inicioYDestino 
			mul bytesporlineafondo //inicioY * bytesporlineafondo, asi iniciamos en la linea donde queremos 
			mov ebx,eax //ebx contendra el resultado anterior
			mov eax, 4 
			mul inicioXDestino //inicioX*4, para asi encontrar la columnda donde queremos empezar a dibujar
			add eax, ebx //posicion de columna + posicion de linea
			add edi,eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada

			mov esi, punteroOrigen //movemos la direccion de la imagen a dibujar a esi, para poder escribir de ella
			//Conseguimos el pixel inicial DEL CUAL empezaremos a dibujar
			mov eax, inicioYOrigen 
			mul bytesporlineaorigen //inicioY * bytesporlineaorigen, asi iniciamos en la linea donde queremos 
			mov ebx,eax //ebx contendra el resultado anterior
			mov eax, 4 
			mul inicioXOrigen //inicioX*4, para asi encontrar la columnda de donde queremos empezar a leer
			add eax, ebx //posicion de columna + posicion de linea
			add esi,eax //Sumamos el desplazamiento anterior al inicio de nuestra imagen para empezar a trabajar en la posicion deseada

			
			mov eax, [esi]

			mov ecx, alto //movemos a ecx la cantidad de lineas que dibujaremos
lazollenarY:

			push ecx
			mov ecx, escalaY
			escaladoY:

			push ecx //guardamos el valor anterior de ecx, porque lo reemplazaremos en un nuevo ciclo
			mov ecx, ancho //la cantidad de columnas que dibujaremos

	lazollenarX:
				//movsd //mueve un pixel de la direccion apuntada por esi a la apuntada por edi, e incrementa esi y edi por 4
		push ecx
		mov ecx, escalaX
escaladoX:

			cmp eax, [esi]
			je nodibujar

				mov edx, [esi]
				mov [edi],edx

nodibujar:
				add edi,4
				loop escaladoX
				
				add esi,4

				pop ecx
				loop lazollenarX

				add edi, bytesporlineafondo //le sumamos la cantidad de bytes de la linea del fondo para pasar a la siguiente linea
				
				push eax
				mov eax, bytesporlineaimagen
				mul escalaX
				sub edi, eax //y retrocedemos una cantidad igual al a su ancho para dibujar desde la posicion X inicial y que no quede escalonado
				pop eax

				sub esi, bytesporlineaimagen

				pop ecx
				loop escaladoY

				//Lo mismo para esi
				
				add esi, bytesporlineaorigen

				pop ecx //recuperamos el valor del contador del ciclo exterior
			loop lazollenarY
		}

	}
#pragma endregion