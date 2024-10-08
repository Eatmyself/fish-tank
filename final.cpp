#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace std;
/*** freeglut***/
#include ".\GL\freeglut.h"
#define PI 3.14159265

class vec3 {
public:
    float x, y, z;
    vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}
};
class ivec3 {
public:
    int x, y, z;
    ivec3(int x = 0, int y = 0, int z = 0) : x(x), y(y), z(z) {}
};

void ResizeWindow(int, int);
void Display(void);
void keyboard(unsigned char, int, int);
void specialKeyboard(int, int, int);
void SetupRC();
void My_Mouse(int, int, int, int);
void readObj(string, vector<vec3>&, vector<vec3>&, vector<vec3>&);
void drawObj(vector<vec3>, vector<vec3>, vector<vec3>, GLuint, int w = GL_POINTS, float r = 255, float b = 255, float g = 255, bool twoD = true);
void translateMatrix(float, float, float);
void rotateMatrix(float, float, float, float);
void scaleMatrix(float);
void createMenu(void);
void mainMenu(int index);
void renderMenu(int index);

int mouseDown = 0;
float xdiff = 100.0f;
float ydiff = 100.0f;
float xrot = 0;
float sight_x = 0, sight_y = 0;
float light=0;
vector<vec3> eel1points;
vector<vec3> eel1texcoords;
vector<vec3> eel1normals;
vector<vec3> eel2points;
vector<vec3> eel2texcoords;
vector<vec3> eel2normals;
vector<vec3> jellyfishpoints;
vector<vec3> jellyfishtexcoords;
vector<vec3> jellyfishnormals;
vector<vec3> bSWpoints;
vector<vec3> bSWtexcoords;
vector<vec3> bSWnormals;
vector<vec3> mSWpoints;
vector<vec3> mSWtexcoords;
vector<vec3> mSWnormals;
vector<vec3> bubbles;
bool stop = false;
GLuint textures[4];
float eel1status[6] = { 0.2,1.5,0.2,1.5,0.2,0.7 };
float eel1move[6] = { 0.02,-0.02,0.02,-0.02,0.02,-0.02 };
float eel2move = 0.2;
float jellyfish = 0;
float circle = 0;

void Timer(int value) {
    if (xrot >= 1)eel2move = -0.2;
    else if (xrot <= -7)eel2move = 0.2;
    xrot += eel2move;
    for (int i = 0; i < 6; i++) {
        if (eel1status[i] >= 1.5)eel1move[i] = -0.02;
        else if (eel1status[i] <= 0.2)eel1move[i] = 0.02;
        eel1status[i] += eel1move[i];
        if (eel1status[i] > 0.7 && eel1status[i] < 0.8) {
            int tmp = rand() % 2;
            if (tmp == 0)eel1move[i] = 0.05;
            else eel1move[i] = -0.05;
        }
    }
    jellyfish += 1;
    if(jellyfish>360)jellyfish=0;
    circle += 10;
    if(circle>360)circle=0;
    glutPostRedisplay();
    if(stop == false)glutTimerFunc(100, Timer, 0);
}

int main(int argc, char** argv)
{
    srand(time(NULL));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1600, 800);
    glutInitWindowPosition(600, 80);
    glutCreateWindow("Garden Eels");

    SetupRC();
    glutReshapeFunc(ResizeWindow);
    glutDisplayFunc(Display);
    glutMouseFunc(My_Mouse);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);

    glutTimerFunc(0, Timer, 0);

    createMenu();

    glutMainLoop();
    return 0;
}
void SetupRC()
{
    GLfloat whiteLight[] = { 0.45f, 0.45f, 0.45f, 1.0f };
    GLfloat sourceLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
    GLfloat lightPos[] = { 0.f, 25.0f, 20.0f, 0.0f };

    glEnable(GL_LIGHTING);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, whiteLight);
    glLightfv(GL_LIGHT0, GL_AMBIENT, sourceLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sourceLight);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glEnable(GL_LIGHT0);

    GLfloat lightPosition[] = { 10.0f, 10.0f, 0.0f, 1.0f }; // Position (homogeneous coordinates)
    GLfloat lightColor[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Color (RGBA)
    GLfloat attenuation[] = { 1.0f, 0.05f, 0.0f }; // Attenuation (constant, linear, quadratic)

     glLightfv(GL_LIGHT1, GL_POSITION, lightPosition); // Set light position
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor); // Set diffuse color
    glLightfv(GL_LIGHT1, GL_CONSTANT_ATTENUATION, &attenuation[0]); // Set constant attenuation
    glLightfv(GL_LIGHT1, GL_LINEAR_ATTENUATION, &attenuation[1]); // Set linear attenuation
    glLightfv(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, &attenuation[2]); // Set quadratic attenuation
    glEnable(GL_LIGHT1);

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);

    readObj("obj/eel1.obj", eel1points, eel1texcoords, eel1normals);
    readObj("obj/eel2.obj", eel2points, eel2texcoords, eel2normals);
    readObj("obj/jelly-fish.obj", jellyfishpoints, jellyfishtexcoords, jellyfishnormals);
    readObj("obj/big-seaweed.obj", bSWpoints, bSWtexcoords, bSWnormals);
    readObj("obj/mini-seaweed.obj", mSWpoints, mSWtexcoords, mSWnormals);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // 設定openGL材質紋理的參數和材質的組合模式
    glGenTextures(4, textures);
    cv::Mat eel1Image = cv::imread("obj/eel1.jpg"); // 利用openCV讀取圖片檔案
    if (eel1Image.empty()) {
        std::cout << "eel1 empty\n";
    }
    else {
        // 將讀取進來的圖片檔案當作材質存進textures中
        cv::flip(eel1Image, eel1Image, 0);
        glGenTextures(1, &textures[0]);
        glBindTexture(GL_TEXTURE_2D, textures[0]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, eel1Image.cols, eel1Image.rows, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, eel1Image.ptr());
    }
    cv::Mat eel2Image = cv::imread("obj/eel2.jpg"); // 利用openCV讀取圖片檔案
    if (eel2Image.empty()) {
        std::cout << "eel2 empty\n";
    }
    else {
        // 將讀取進來的圖片檔案當作材質存進textures中
        cv::flip(eel2Image, eel2Image, 0);
        glGenTextures(1, &textures[1]);
        glBindTexture(GL_TEXTURE_2D, textures[1]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, eel2Image.cols, eel2Image.rows, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, eel2Image.ptr());
    }
    cv::Mat sandImage = cv::imread("obj/sand.jpg"); // 利用openCV讀取圖片檔案
    if (sandImage.empty()) {
        std::cout << "sand empty\n";
    }
    else {
        // 將讀取進來的圖片檔案當作材質存進textures中
        cv::flip(sandImage, sandImage, 0);
        glGenTextures(1, &textures[2]);
        glBindTexture(GL_TEXTURE_2D, textures[2]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sandImage.cols, sandImage.rows, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, sandImage.ptr());
    }
    cv::Mat starfishImage = cv::imread("obj/starfish.jpg"); // 利用openCV讀取圖片檔案
    if (starfishImage.empty()) {
        std::cout << "starfish empty\n";
    }
    else {
        // 將讀取進來的圖片檔案當作材質存進textures中
        cv::flip(starfishImage, starfishImage, 0);
        glGenTextures(1, &textures[3]);
        glBindTexture(GL_TEXTURE_2D, textures[3]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, starfishImage.cols, starfishImage.rows, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, starfishImage.ptr());
    }
}

void ResizeWindow(int w, int h)
{
    //printf("Window Size= %d X %d\n", w, h);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-40, 40, -20, 20, -20, 40);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Display(void)
{


    glClearColor(0.0+light, 0.3+light, 0.4+light, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 1, 10, sight_x, sight_y, 0, 0, 1, 0);

    //bubles
    glColor3ub(175, 202, 218);
    int tmp = bubbles.size();
    for (int i = tmp - 1; i >= 0; i--) {
        translateMatrix(bubbles[i].x, bubbles[i].y, 0);
        glutSolidSphere(0.5, 20.0, 20.0);
        translateMatrix(-bubbles[i].x, -bubbles[i].y, 0);
        bubbles[i].y += 0.5;
        if (bubbles[i].y > 20) {
            bubbles.erase(bubbles.begin() + i);
            cout << "pop" << endl;
        }
    }

    //sand
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    glBegin(GL_QUADS);
    glColor3ub(255, 255, 255);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-40, -20, -100);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(40, -20, -100);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(40, -20, 0);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-40, -20, 0);
    glEnd();
    glBegin(GL_QUADS);
    glColor3ub(255, 255, 255);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-40, -20, 0);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(40, -20, 0);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(40, -40, 0);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-40, -40, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    //eels
    scaleMatrix(3);
    glPointSize(5);

    translateMatrix(-13, -10 - eel1status[0], -3);
    drawObj(eel2points, eel2texcoords, eel2normals, textures[0]);
    translateMatrix(13, 10 + eel1status[0], 3);

    translateMatrix(-10, -10 - eel1status[1], -4);
    rotateMatrix(30, 0, 1, 0);
    drawObj(eel1points, eel1texcoords, eel1normals, textures[0]);
    rotateMatrix(-30, 0, 1, 0);
    translateMatrix(10, 10 + eel1status[1], 4);

    translateMatrix(-9, -11, -7);
    rotateMatrix(-xrot, 0, 0, 1);
    drawObj(eel2points, eel2texcoords, eel2normals, textures[1]);
    rotateMatrix(xrot, 0, 0, 1);
    translateMatrix(9, 11, 7);

    translateMatrix(-8, -10 - eel1status[2], -3);
    drawObj(eel1points, eel1texcoords, eel1normals, textures[0]);
    translateMatrix(8, 10 + eel1status[2], 3);

    translateMatrix(-7, -10, -5);
    rotateMatrix(xrot, 0, 0, 1);
    drawObj(eel2points, eel2texcoords, eel2normals, textures[1]);
    rotateMatrix(-xrot, 0, 0, 1);
    translateMatrix(7, 10, 5);

    translateMatrix(-4, -11.5, -1);
    rotateMatrix(60, 0, 1, 0);
    rotateMatrix(-xrot, 0, 0, 1);
    drawObj(eel2points, eel2texcoords, eel2normals, textures[1]);
    rotateMatrix(xrot, 0, 0, 1);
    rotateMatrix(-60, 0, 1, 0);
    translateMatrix(4, 11.5, 1);

    translateMatrix(-3, -10 - eel1status[3], -7);
    drawObj(eel1points, eel1texcoords, eel1normals, textures[0]);
    translateMatrix(3, 10 + eel1status[3], 7);

    translateMatrix(0, -10 - eel1status[4], -4);
    drawObj(eel2points, eel2texcoords, eel2normals, textures[0]);
    translateMatrix(0, 10 + eel1status[4], 4);

    translateMatrix(3, -10 - eel1status[5], -7);
    drawObj(eel1points, eel1texcoords, eel1normals, textures[0]);
    translateMatrix(-3, 10 + eel1status[5], 7);

    translateMatrix(5, -10, -2);
    rotateMatrix(30, 0, 1, 0);
    rotateMatrix(xrot, 0, 0, 1);
    drawObj(eel1points, eel1texcoords, eel1normals, textures[1]);
    rotateMatrix(-xrot, 0, 0, 1);
    rotateMatrix(-30, 0, 1, 0);
    translateMatrix(-5, 10, 2);

    translateMatrix(7, -10.5, -7);
    rotateMatrix(-xrot, 0, 0, 1);
    drawObj(eel2points, eel2texcoords, eel2normals, textures[1]);
    rotateMatrix(xrot, 0, 0, 1);
    translateMatrix(-7, 10.5, 7);

    translateMatrix(8.5, -11, -5);
    rotateMatrix(-xrot, 0, 0, 1);
    drawObj(eel2points, eel2texcoords, eel2normals, textures[1]);
    rotateMatrix(xrot, 0, 0, 1);
    translateMatrix(-8.5, 11, 5);

    //seaweeds
    scaleMatrix(0.2);
    translateMatrix(-60, -35, -10);
    drawObj(bSWpoints, bSWtexcoords, bSWnormals, textures[0], GL_TRIANGLES, 41, 102, 31, false);
    translateMatrix(60, 35, 10);

    translateMatrix(-30, -34, -20);
    drawObj(mSWpoints, mSWtexcoords, mSWnormals, textures[0], GL_TRIANGLES, 41, 102, 31, false);
    translateMatrix(30, 34, 20);

    translateMatrix(-10, -34, -35);
    drawObj(mSWpoints, mSWtexcoords, mSWnormals, textures[0], GL_TRIANGLES, 41, 102, 31, false);
    translateMatrix(10, 34, 35);

    translateMatrix(0, -34, -30);
    drawObj(mSWpoints, mSWtexcoords, mSWnormals, textures[0], GL_TRIANGLES, 41, 102, 31, false);
    translateMatrix(0, 34, 30);

    translateMatrix(5, -34, -40);
    drawObj(mSWpoints, mSWtexcoords, mSWnormals, textures[0], GL_TRIANGLES, 41, 102, 31, false);
    translateMatrix(-5, 34, 40);

    translateMatrix(15, -34, -10);
    drawObj(mSWpoints, mSWtexcoords, mSWnormals, textures[0], GL_TRIANGLES, 41, 102, 31, false);
    translateMatrix(-15, 34, 10);

    translateMatrix(20, -34, -30);
    drawObj(mSWpoints, mSWtexcoords, mSWnormals, textures[0], GL_TRIANGLES, 41, 102, 31, false);
    translateMatrix(-20, 34, 30);

    translateMatrix(-10, -35, -40);
    drawObj(bSWpoints, bSWtexcoords, bSWnormals, textures[0], GL_TRIANGLES, 41, 102, 31, false);
    translateMatrix(10, 35, 40);

    translateMatrix(9, -35, -20);
    drawObj(bSWpoints, bSWtexcoords, bSWnormals, textures[0], GL_TRIANGLES, 41, 102, 31, false);
    translateMatrix(-9, 35, 20);

    translateMatrix(25, -35, -40);
    drawObj(bSWpoints, bSWtexcoords, bSWnormals, textures[0], GL_TRIANGLES, 41, 102, 31, false);
    translateMatrix(-25, 35, 40);

    scaleMatrix(0.5);
    translateMatrix(80, -70, -40);
    drawObj(bSWpoints, bSWtexcoords, bSWnormals, textures[0], GL_TRIANGLES, 41, 102, 31, false);
    translateMatrix(-80, 70, 40);

    scaleMatrix(4);
    translateMatrix(30, -20, -10);
    drawObj(bSWpoints, bSWtexcoords, bSWnormals, textures[0], GL_TRIANGLES, 41, 102, 31, false);
    translateMatrix(-30, 20, 10);

    translateMatrix(-20, -20, -20);
    drawObj(bSWpoints, bSWtexcoords, bSWnormals, textures[0], GL_TRIANGLES, 41, 102, 31, false);
    translateMatrix(20, 20, 20);

    translateMatrix(-25, -20, -20);
    drawObj(bSWpoints, bSWtexcoords, bSWnormals, textures[0], GL_TRIANGLES, 41, 102, 31, false);
    translateMatrix(25, 20, 20);

    //jellyfush
    scaleMatrix(5);
    translateMatrix(0, -1, 0);
    rotateMatrix(jellyfish, 0, 1, 0);
    drawObj(jellyfishpoints, jellyfishtexcoords, jellyfishnormals, textures[3], GL_TRIANGLES, 89, 139, 167, false);
    drawObj(jellyfishpoints, jellyfishtexcoords, jellyfishnormals, textures[3], GL_LINES, 240, 240, 240, false);
    rotateMatrix(-jellyfish, 0, 1, 0);
    translateMatrix(0, 1, 0);

    //shining jelly
    GLfloat sita = 2 * circle * PI / 180.0;
    rotateMatrix(circle, 0, 1, 0);
    translateMatrix(1, 0.5 + cos(sita) * 0.5, 0);
    scaleMatrix(0.2);
    drawObj(jellyfishpoints, jellyfishtexcoords, jellyfishnormals, textures[3], GL_TRIANGLES, rand() % 255, rand() % 255, rand() % 255, false);
    drawObj(jellyfishpoints, jellyfishtexcoords, jellyfishnormals, textures[3], GL_LINES, 240, 240, 240, false);

    glutSwapBuffers();
}


void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'r':
        sight_x = 0;
        sight_y = 0;
        light=0;
        break;
    case 'w':
        light+=0.05;
        break;
    case 's':
        light-=0.05;
        break;
    }

    glutPostRedisplay();
}

void specialKeyboard(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP:
        sight_y += 1;
        break;
    case GLUT_KEY_DOWN:
        sight_y -= 1;
        break;
    case GLUT_KEY_LEFT:
        sight_x -= 1;
        break;
    case GLUT_KEY_RIGHT:
        sight_x += 1;
        break;
    default:
        break;
    }

    glutPostRedisplay();
}

void My_Mouse(int button, int state, int rx, int ry)
{
    switch (button)
    {
    case GLUT_RIGHT_BUTTON:
        break;
    case GLUT_LEFT_BUTTON:
        float hx, hy;
        hx = ((rx - 720) / 18); hy = -((ry - 400) / 20);
        if (state == GLUT_DOWN) {
            bubbles.push_back(vec3(hx, hy));
        }
        break;
    case GLUT_MIDDLE_BUTTON:
        break;
    default:
        break;
    }

    glutPostRedisplay();
}

void readObj(string filepath, vector<vec3>& points, vector<vec3>& texcoords, vector<vec3>& normals)
{
    points.clear();
    // 頂點屬性
    vector<vec3> vectexPosition;
    vector<vec3> vertexTexcoord;
    vector<vec3> vectexNormal;


    // 面片索引資訊
    vector<ivec3> positionIndex;
    vector<ivec3> texcoordIndex;
    vector<ivec3> normalIndex;

    // 開啟檔案流
    ifstream fin(filepath);
    string line;
    if (!fin.is_open())
    {
        cout << "檔案 " << filepath << " 開啟失敗" << endl;
        exit(-1);
    }

    // 按行讀取
    while (getline(fin, line))
    {
        std::istringstream sin(line);   // 以一行的資料作為 string stream 解析並且讀取
        std::string type;
        GLfloat objx, objy, objz;
        int v0, vt0, vn0;
        int v1, vt1, vn1;   // 2
        int v2, vt2, vn2;   // 3
        char slash;

        // 讀取obj檔案
        sin >> type;
        if (type == "v") {
            sin >> objx >> objy >> objz;
            vectexPosition.push_back(vec3(objx, objy, objz));
        }
        if (type == "vt") {
            sin >> objx >> objy;
            vertexTexcoord.push_back(vec3(objx, objy));
        }
        if (type == "vn") {
            sin >> objx >> objy >> objz;
            vectexNormal.push_back(vec3(objx, objy, objz));
        }

        if (type == "f") {
            sin >> v0 >> slash >> vt0 >> slash >> vn0;
            sin >> v1 >> slash >> vt1 >> slash >> vn1;
            sin >> v2 >> slash >> vt2 >> slash >> vn2;
            positionIndex.push_back(ivec3(v0 - 1, v1 - 1, v2 - 1));
            texcoordIndex.push_back(ivec3(vt0 - 1, vt1 - 1, vt2 - 1));
            normalIndex.push_back(ivec3(vn0 - 1, vn1 - 1, vn2 - 1));
        }
    }

    // 根據面片資訊生成最終傳入頂點著色器的頂點資料
    for (int i = 0; i < positionIndex.size(); i++)
    {
        // 頂點位置
        points.push_back(vectexPosition[positionIndex[i].x]);
        points.push_back(vectexPosition[positionIndex[i].y]);
        points.push_back(vectexPosition[positionIndex[i].z]);

        // 頂點紋理座標
        texcoords.push_back(vertexTexcoord[texcoordIndex[i].x]);
        texcoords.push_back(vertexTexcoord[texcoordIndex[i].y]);
        texcoords.push_back(vertexTexcoord[texcoordIndex[i].z]);

        // 頂點法線
        normals.push_back(vectexNormal[normalIndex[i].x]);
        normals.push_back(vectexNormal[normalIndex[i].y]);
        normals.push_back(vectexNormal[normalIndex[i].z]);
    }
}

void drawObj(vector<vec3> points, vector<vec3> texcoords, vector<vec3> normals, GLuint tex, int w, float r, float b, float g, bool twoD) {
    if (twoD) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex);
    }
    glBegin(w);
    for (int i = 2; i < eel2points.size(); i++) {
        glColor3ub(r, b, g);
        glTexCoord2f(texcoords[i].x, texcoords[i].y);
        glNormal3f(normals[i].x, normals[i].y, normals[i].z);
        glVertex3f(points[i].x, points[i].y, points[i].z);
    }
    glEnd();
    if (twoD)glDisable(GL_TEXTURE_2D);
}

void translateMatrix(float tra_x, float tra_y, float tra_z) {
    GLfloat Matrix[] = {
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    tra_x, tra_y, tra_z, 1.0 };
    glMultMatrixf(Matrix);
}

void rotateMatrix(float degree, float x, float y, float z) {
    GLfloat sita = degree * PI / 180.0;
    GLfloat rotX[] = {
    (cos(sita) + (1 - cos(sita)) * x * x), ((1 - cos(sita)) * x * y + sin(sita) * z), ((1 - cos(sita)) * x * z - sin(sita) * y), 0.0,
    ((1 - cos(sita)) * x * y - sin(sita) * z), (cos(sita) + (1 - cos(sita)) * y * y) , ((1 - cos(sita)) * y * z + sin(sita) * x), 0.0,
    ((1 - cos(sita)) * x * z + sin(sita) * y), ((1 - cos(sita)) * y * z - sin(sita) * x), (cos(sita) + (1 - cos(sita)) * z * z), 0.0,
    0.0, 0.0, 0.0, 1.0 };
    glMultMatrixf(rotX);
}

void scaleMatrix(float n) {
    GLfloat smallMatrix[] = {
    n, 0.0, 0.0, 0.0,
    0.0, n, 0.0, 0.0,
    0.0, 0.0, n, 0.0,
    0.0, 0.0, 0.0, 1.0 };
    glMultMatrixf(smallMatrix);
}

void mainMenu(int index)
{
    switch (index)
    {
    case 1:
        stop = false;
        glutTimerFunc(0, Timer, 0);
        break;
    case 2:
        stop = true;
        break;
    }
    glutPostRedisplay();
}


void createMenu()
{

    int menu = glutCreateMenu(mainMenu);
    glutAddMenuEntry("play", 1);
    glutAddMenuEntry("stop", 2);

    glutAttachMenu(GLUT_RIGHT_BUTTON);

}