#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>


#include "All_Textures.ppm"
#include "Sky.ppm"


typedef struct {
    int w, a, d, s;
}ButtonKeys; ButtonKeys Keys;

#define     PI  3.1415926535
#define     P2  PI/2
#define     P3  3*PI/2
#define     DR  0.0174533

/* Subroutine which converts degrees to radians*/
float degToRad(float a) { return a*M_PI/180.0;}
/* Subroutine which standardizes angles to be within a range of [0-360]*/
float FixAng(float a) {if(a>359){a-=360;} if (a<0){ a+=360;} return a;}


/* Player coordinates and slope / angle*/
float px,py,pdx,pdy,pa;
/* variables for frame1, frame2, and the resulting fps*/
float frame1,frame2,fps;

/* Method for drawing player (Only applicable for top down view)*/
void drawPlayer() {
    glColor3f(1,1,0);
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex2i(px,py);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2i(px,py);
    glVertex2i(px+pdx*20,py+pdy*20);
    glEnd();
}

void drawSky() {
    int x,y;
    for (y=0;y<=40;y++) {
        for (x=0;x<120;x++) {
            int xo=(int)pa*2-x; if (xo<0) {xo+=120;} xo=xo % 120;
            int pixel=(y*120+xo)*3;
            int red = sky[pixel+0];
            int green = sky[pixel+1];
            int blue = sky[pixel+2];

            glPointSize(8); glColor3ub(red,green,blue); glBegin(GL_POINTS);
            glVertex2i(x*8,y*8); glEnd();
        }
    }
}

/* Tilemap which contains level data*/
int mapX=8,mapY=8,mapS=64;
int mapWalls[] = 
{
    1,1,1,1,1,5,1,1,
    1,0,0,0,0,0,0,1,
    1,0,1,1,0,0,0,1,
    1,0,1,1,0,1,1,1,
    1,0,1,0,0,1,0,1,
    1,0,1,0,0,0,0,1,
    1,0,1,0,0,1,0,1,
    1,1,1,1,1,1,1,1,
};

int mapFloor[] = 
{
    0,0,0,0,0,0,0,0,
    0,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,0,
    0,0,0,0,0,0,0,0,
};

int mapCeiling[] = 
{
    0,0,0,0,0,0,0,0,
    0,3,3,3,3,3,3,0,
    0,3,3,3,3,3,3,0,
    0,3,3,3,3,3,3,0,
    0,3,3,3,3,3,3,0,
    0,3,3,3,3,3,3,0,
    0,3,3,3,3,3,3,0,
    0,0,0,0,0,0,0,0,
};

/* Draws 2d visualization of world */
void drawMap2D() {
    int x,y,xo,yo;
    for (y=0;y<mapY;y++) {
        for (x=0;x<mapX;x++) {
            if (mapWalls[y*mapX+x]>0) { glColor3d(1,1,1); } 
            else {glColor3f(0,0,0);}
            xo=x*mapS;yo=y*mapS;
            glBegin(GL_QUADS);
            /* Utilize offset of 1 pixel */
            glVertex2i(xo     +1,yo     +1);
            glVertex2i(xo     +1,yo+mapS-1);
            glVertex2i(xo+mapS-1,yo+mapS-1);
            glVertex2i(xo+mapS-1,yo     +1);
            glEnd();
        }
    }
}
 

/* Distance formula (here we take radians into account)*/
float dist(float ax,float ay,float bx,float by,float ra) {
    return cos(degToRad(ra))*(bx-ax)-sin(degToRad(ra))*(by-ay);
}

/* Main Rendering engine, to the unitiated it looks like black magic but I assure you it
isn't (I think idk) */
void drawRays2D() {
    
    int r,mx,my,mp,dof,side; float vx,vy,rx,ry,ra,xo,yo,disV,disH; 

    /* take angle of player, add 30 (offset) and then fix it to be within range of 360 */
    ra=FixAng(pa+30);

    /* Iterate through the FOV*/
    for (r=0;r<120;r++) {
        int vmt=0,hmt=0;
        dof=0;side=0;disV=100000;
        /* Calculate Tangent */
        float Tan=tan(degToRad(ra));

        if(cos(degToRad(ra))> 0.001){ 
            rx=(((int)px>>6)<<6)+64;ry=(px-rx)*Tan+py;xo= 64;yo=-xo*Tan;}//looking left
        else if(cos(degToRad(ra))<-0.001){ 
            rx=(((int)px>>6)<<6) -0.0001;ry=(px-rx)*Tan+py; xo=-64;yo=-xo*Tan;
        }//looking right
        else { rx=px; ry=py; dof=8;}/* Looking Straight */ 

        while (dof<8) {
            mx=(int)(rx)>>6; my=(int)(ry)>>6;mp=my*mapX+mx;
            if (mp>0 && mp<mapX*mapY && mapWalls[mp]>0) {
                vmt=mapWalls[mp]-1;dof=8; disV=cos(degToRad(ra))*(rx-px)-sin(degToRad(ra))*(ry-py);//if hit
            }else{ rx+=xo; ry+=yo; dof+=1;} //check next horizontal    
            vx=rx;vy=ry;
        }

        dof=0; disH=100000;
        Tan=1.0/Tan; 
        if(sin(degToRad(ra))> 0.001){ ry=(((int)py>>6)<<6) -0.0001; rx=(py-ry)*Tan+px; yo=-64; xo=-yo*Tan;}//looking up 
        else if(sin(degToRad(ra))<-0.001){ ry=(((int)py>>6)<<6)+64;      rx=(py-ry)*Tan+px; yo= 64; xo=-yo*Tan;}//looking down
        else{ rx=px; ry=py; dof=8;}                                                   //looking straight left or right
        
        while(dof<8) 
        { 
            mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;                          
            if(mp>0 && mp<mapX*mapY && mapWalls[mp]>0){ hmt=mapWalls[mp]-1;dof=8; disH=cos(degToRad(ra))*(rx-px)-sin(degToRad(ra))*(ry-py);}//hit         
            else{ rx+=xo; ry+=yo; dof+=1;}                                               //check next horizontal
        } 
        
        float shade=1;
        glColor3f(0,0.8,0);
        if(disV<disH){hmt=vmt;shade=0.5;rx=vx; ry=vy; disH=disV; glColor3f(0,0.6,0);}                  //horizontal hit first

            
        int ca=FixAng(pa-ra); disH=disH*cos(degToRad(ca));                            //fix fisheye 
        int lineH = (mapS*640)/(disH);
        float ty_step=32.0/(float)lineH;
        float ty_off=0; 
        if(lineH>640){ ty_off=(lineH-640)/2.0;lineH=640;}                     //line height and limit
        int lineOff = 320 - (lineH>>1);                                               //line offset


        //draw walls
        int y;
        float ty=ty_off*ty_step;//hmt*32;
        float tx;
        if (shade==1) { tx=(int)(rx/2.0)%32; if (ra>180) {tx=31-tx;}}
        else          { tx=(int)(ry/2.0)%32; if (ra>90 && ra<270) {tx=31-tx;}}
        for (y=0;y<lineH;y++) {
            int pixel=((int)ty*32+(int)tx)*3+(hmt*32*32*3);
            int red = All_Textures[pixel+0]*shade;
            int green = All_Textures[pixel+1]*shade;
            int blue = All_Textures[pixel+2]*shade;

            glPointSize(8); glColor3ub(red,green,blue); glBegin(GL_POINTS);
            glVertex2i(r*8,y+lineOff); glEnd();
            ty+=ty_step;
        }


        //draw floors
        for (y=lineOff+lineH;y<640;y++) {
            float dy=y-(640/2.0),deg=degToRad(ra),raFix=cos(degToRad(FixAng(pa-ra)));
            tx=px/2 + cos(deg)*158*2*32/dy/raFix;
            ty=py/2 - sin(deg)*158*2*32/dy/raFix;
            int mp=mapFloor[(int)(ty/32.0)*mapX+(int)(tx/32.0)]*32*32;

            int pixel=(((int)(ty)&31)*32 + ((int)(tx)&31))*3+mp*3;
            int red = All_Textures[pixel+0]*0.7;
            int green = All_Textures[pixel+1]*0.7;
            int blue = All_Textures[pixel+2]*0.7;

            glPointSize(8); glColor3ub(red,green,blue); glBegin(GL_POINTS);
            glVertex2i(r*8,y); glEnd();

            //ceiling

            mp=mapCeiling[(int)(ty/32.0)*mapX+(int)(tx/32.0)]*32*32;

            pixel=(((int)(ty)&31)*32 + ((int)(tx)&31))*3+mp*3;
            red = All_Textures[pixel+0];
            green = All_Textures[pixel+1];
            blue = All_Textures[pixel+2];

            if (mp>0) {glPointSize(8); glColor3ub(red,green,blue); glBegin(GL_POINTS);
            glVertex2i(r*8,640-y); glEnd();}
        }

        ra=FixAng(ra-0.5);                                                              //go to next ray
    }
}



void display() {

    //fps
    frame2=glutGet(GLUT_ELAPSED_TIME); //use system call to get elapsed time
    fps=(frame2-frame1); //resulting FPS is basically the difference between f1 and f2
    frame1=glutGet(GLUT_ELAPSED_TIME);

    /* Turn Left/Right */
    if (Keys.a==1) { pa +=0.2*fps; pa=FixAng(pa); pdx=cos(degToRad(pa));pdy=-sin(degToRad(pa));}
    if (Keys.d==1) { pa -=0.2*fps; pa=FixAng(pa); pdx=cos(degToRad(pa));pdy=-sin(degToRad(pa));}
    
    /* Code for collision Detection, determine walkable tiles based off of dist*/
    int xo=0; if (pdx<0) {xo=-20;}else{xo=20;}
    int yo=0; if (pdy<0) {yo=-20;}else{yo=20;}
    int ipx=px/64.0,ipx_add_xo=(px+xo)/64.0,ipx_sub_xo=(px-xo)/64.0;
    int ipy=py/64.0,ipy_add_yo=(py+yo)/64.0,ipy_sub_yo=(py-yo)/64.0;
    
    /* Apply collision to forward/backward keys*/
    if (Keys.w==1) { 
        if (mapWalls[ipy*mapX+ipx_add_xo] == 0) { px+=pdx*0.2*fps;}  
        if (mapWalls[ipy_add_yo*mapX+ipx]==0)   { py+=pdy*0.2*fps;}
    }
    if (Keys.s==1) { 
        if (mapWalls[ipy*mapX+ipx_sub_xo] == 0) { px-=pdx*0.2*fps;}  
        if (mapWalls[ipy_sub_yo*mapX+ipx]==0)   { py-=pdy*0.2*fps;}
    }
    
    glutPostRedisplay();


    /* Draw Everything */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //drawMap2D();
    //drawPlayer();
    drawSky();
    drawRays2D();
    glutSwapBuffers();
}

void resize(int w, int h) {
    glutReshapeWindow(960,640);
}

/* Methods for quicker button response */
void ButtonDown(unsigned char key, int x, int y) {
    if (key=='a') { Keys.a=1;}
    if (key=='d') { Keys.d=1;}
    if (key=='w') { Keys.w=1;}
    if (key=='s') { Keys.s=1;}
    if (key=='e') {
        int xo=0; if (pdx<0) {xo=-25;}else{xo=25;}
        int yo=0; if (pdy<0) {yo=-25;}else{yo=25;}
        int ipx=px/64.0,ipx_add_xo=(px+xo)/64.0;
        int ipy=py/64.0,ipy_add_yo=(py+yo)/64.0;
        if (mapWalls[ipy_add_yo*mapX+ipx_add_xo]==4) { mapWalls[ipy_add_yo*mapX+ipx_add_xo]=0;}
    }
    glutPostRedisplay();
}

void ButtonUp(unsigned char key, int x, int y) {
    if (key=='a') { Keys.a=0;}
    if (key=='d') { Keys.d=0;}
    if (key=='w') { Keys.w=0;}
    if (key=='s') { Keys.s=0;}
    glutPostRedisplay();
}

void init() {
    glClearColor(0.3,0.3,0.3,0);
    gluOrtho2D(0,960,640,0);
    
    /* Player spawn point */
    px=90;
    py=90;

    /* Player variables for angle position */
    pdx=cos(pa)*5;
    pdy=sin(pa)*5;
}

/* Main Method where we call the main init loop*/
int main(int argc, char** argv) { 
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(960,640);
    glutInitWindowPosition( glutGet(GLUT_SCREEN_WIDTH)/2-960/2 ,glutGet(GLUT_SCREEN_HEIGHT)/2-640/2 );
    glutCreateWindow("Raycaster OpenGL");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(ButtonDown);
    glutKeyboardUpFunc(ButtonUp);
    glutMainLoop();
}