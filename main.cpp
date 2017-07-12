/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: lukasz
 *
 * Created on 15 października 2016, 17:18
 */

//#include <cstdlib>
#include <GL/gl.h>
#include <GL/glut.h>
#include <noise/noise.h>
#include <iostream>
#include <math.h>

typedef float point3[3];

using namespace std;
using namespace noise;


/* 
 * Zmienne globalne
 */
static GLdouble     angle       = M_PI / 4;
static GLdouble     pixToAngle;
static GLint        status      = 0;                        // stan przycisków myszy
static int          xPosOld     = 0;
static int          deltaX      = 0;
static double       motion      = 0;                        // predkość poruszania po mapie          
static GLfloat      eye[3]      = {0.0f, 0.0f, 2.0f};
static GLfloat      aim[3]      = {40.0f, 40.0f, 2.0f};
static int          N           = 300;

class Terrain {
    
    private:
        module::Perlin  modul;
        point3          **tab;
        int             size;
    
    public:
        Terrain();
        Terrain(int m);
        ~Terrain();
        void draw();
};

Terrain::Terrain() {
    
    size    = N;
    // alokowanie pamięci
    tab     = new point3 *[size];
    for (int i = 0; i < size; i++) tab[i]   = new point3[size];

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            
            tab[i][j][0]    = (float)i;
            tab[i][j][1]    = (float)j;
            tab[i][j][2]    = modul.GetValue((float)i / (i + j), (float)j / (i + j), (float) i + j); 
        }
    }
}

Terrain::Terrain(int m) {
    
    size    = m;
    // alokowanie pamięci
    tab    = new point3 *[size];
    for (int i = 0; i < size; i++) tab[i]   = new point3[size];
    
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            
            tab[i][j][0]    = (float)i;
            tab[i][j][1]    = (float)j;
            tab[i][j][2]    = modul.GetValue((float)i / (i + j), (float)j / (i + j), (float) i + j); 
        }
    }
}

Terrain::~Terrain() {
    
    // zwalnianie pamięci
    for (int i = 0; i < size; i++) delete [] tab[i];
    delete [] tab;
    tab     = NULL;
}

void Terrain::draw() {
    
    for (int i = 0; i < size; i++) {
        
        for (int j = 0; j < size; j++) {
            
            glBegin(GL_POINTS);
		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex3fv(tab[i][j]);
            glEnd();
        }
    }
}
// Powieszchnia rysowana
Terrain plane;

/*************************************************************************************/
// Funkcja monitoruje stan przycisków myszy i położenia kursora

void Mouse(int btn, int state, int x, int y) {

    if ( btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN ) {
        
        xPosOld = x;            // przypisanie aktualnie odczytanej pozycji kursora 
                                // jako pozycji poprzedniej
        status  = 1;            // wcięnięty został lewy klawisz myszy
    } 
    else    status = 0;         // nie został wcięnięty żaden klawisz 
}

/*************************************************************************************/
// Funkcja "monitoruje" położenie kursora myszy i ustawia wartości odpowiednich 
// zmiennych globalnych

void Motion( GLsizei x, GLsizei y ) {
    
    deltaX  = (x - xPosOld);     // obliczenie różnicy położenia kursora myszy

    xPosOld = x;            // podstawienie bieżącego położenia jako poprzednie

    glutPostRedisplay();     // przerysowanie obrazu sceny
}

/*************************************************************************************/
// Funkcaja określająca, co ma być rysowane 
// (zawsze wywoływana, gdy trzeba przerysować scenę)
void RenderScene(void) {
        
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Czyszczenie okna aktualnym kolorem czyszczącym

    // wybór macierzy modelowania
    glMatrixMode( GL_MODELVIEW );
    
    glLoadIdentity();
    // Czyszczenie macierzy bieżącej
    
    if(status == 1) {                                       // jeśli lewy klawisz myszy wcięnięty

        angle   += (double)deltaX * pixToAngle * 0.01;      // modyfikacja kąta obrotu o kat proporcjonalny
    }
    
    aim[0]  = cos(angle);
    aim[1]  = sin(angle);
    eye[0]  += cos(angle) * motion;
    eye[1]  += sin(angle) * motion;
    motion    = 0;
    
    gluLookAt(eye[0], eye[1], eye[2], aim[0] + eye[0], aim[1] + eye[1], aim[2], 0.0, 0.0, 1.0);
    // Zdefiniowanie położenia obserwatora

    // Narysowanie tego co chcemy zobaczyć
    plane.draw();

    glFlush();
    // Przekazanie poleceń rysujących do wykonania
    
    glutSwapBuffers();
}
/*************************************************************************************/
// Funkcja ustalająca stan renderowania

void MyInit(void) {
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    // Kolor okna wnętrza okna - ustawiono na szary
   
    glEnable(GL_DEPTH_TEST); // włączenie mechanizmu z-bufora
}

/*************************************************************************************/
// Funkcja odpowiada za akcje związanie z naciśniętymi klawiszami
void keys(unsigned char key, int x, int y) {
    
    if(key == 'w') motion    = 1;
    if(key == 's') motion    = -1;
    
    glutPostRedisplay();
}
void ChangeSize(GLsizei horizontal, GLsizei vertical ) {

    pixToAngle = 360.0 / (float)horizontal;  // przeliczenie pikseli na stopnie
    
    glMatrixMode(GL_PROJECTION);
    // Przełączenie macierzy bieżącej na macierz projekcji
    
    glLoadIdentity();
    // Czyszcznie macierzy bieżącej

    gluPerspective(70, 1.0, 1.0, 40.0);
    // Ustawienie parametrów dla rzutu perspektywicznego

    if(horizontal <= vertical)
        glViewport(0, (vertical-horizontal)/2, horizontal, horizontal);

    else
        glViewport((horizontal-vertical)/2, 0, vertical, vertical);
    // Ustawienie wielkości okna okna widoku (viewport) w zależności
    // relacji pomiędzy wysokością i szerokością okna

}
/*************************************************************************************/

/*************************************************************************************/
// Główny punkt wejścia programu. Program działa w trybie konsoli

int main(int argc, char **argv) {
        
    glutInit(&argc, argv);
    // Funkcja inicjująca bibliotekę glut
   
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB |GLUT_DEPTH);
    // Ustawienie trybu wyświetlania

    glutInitWindowSize(600, 600);
   
    glutCreateWindow("terrain: libnoise demo");
    // Utworzenie okna i określenie treści napisu w nagłówku okna
    
    glutDisplayFunc(RenderScene);
    // Określenie, że funkcja RenderScene będzie funkcją zwrotną (callback)
    // Biblioteka GLUT będzie wywoływała tą funkcję za każdym razem, gdy
    // trzeba będzie przerysować okno
    
    glutReshapeFunc(ChangeSize);
    // Dla aktualnego okna ustala funkcję zwrotną odpowiedzialną za
    // zmiany rozmiaru okna
   
    MyInit(); 
    // Funkcja MyInit (zdefiniowana powyżej) wykonuje wszelkie 
    // inicjalizacje konieczne przed przystąpieniem do renderowania

    glutMouseFunc(Mouse);
    // Ustala funkcję zwrotną odpowiedzialną za badanie stanu myszy
    
    glutMotionFunc(Motion);
    // Ustala funkcję zwrotną odpowiedzialną za badanie ruchu myszy
    
    glutKeyboardFunc(keys);
   
    glEnable(GL_DEPTH_TEST);
    // Włączenie mechanizmu usuwania niewidocznych elementów sceny
    
    glutMainLoop();
    // Funkcja uruchamia szkielet biblioteki GLUT

   return 0;
}
/**************************************************************************************/

