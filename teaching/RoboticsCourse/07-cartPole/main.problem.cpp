#include <stdlib.h>
#include <Kin/roboticsCourse.h>
#include <Gui/opengl.h>

void drawEnv(void*);
void glDrawCartPole(void *classP);

struct CartPoleState{
  double x,x1,th,th1;
  double tau,x2,the2;
  double c1,c2,Mp,Mc,l,g, dynamicsNoise;
  OpenGL gl;
  CartPoleState(){
    x=0.;
    x1=0.;
    th=.2; //slighly non-upright //MLR_PI; //haning down
    th1=0.;

    //init constants
    tau = 1/60.0; //with 1/1000 is better
    Mp = 1;
    Mc = 1;
    l = 1;
    c1 = 1/(Mp+Mc);
    c2 = l*Mp/(Mp+Mc);
    g = 9.8;

    dynamicsNoise = 0;

    gl.add(drawEnv, this);
    gl.add(glDrawCartPole, this);
    gl.camera.setPosition(10., -50., 10.);
    gl.camera.focus(0, 0, 1.);
    gl.camera.upright();
    gl.update();
  }

  void step(double u){
    the2 = g*sin(th) + cos(th)*(-c1*u-c2*th1*th1*sin(th));
    the2 /= l*4/3 - c2*cos(th)*cos(th);
    x2 = c1*u + c2*(th1*th1*sin(th) - the2*cos(th));

    x   += tau*x1;
    x1  += tau*x2;
    th  += tau*th1;
    th1 += tau*the2;

    if(dynamicsNoise){
      x1 += dynamicsNoise*rnd.gauss();
      th1 += dynamicsNoise*rnd.gauss();
    }
  }

};

void glDrawCartPole(void *classP){
#ifdef MLR_GL //FIXME: this should really use our GL abstraction, should it not?
  CartPoleState *s=(CartPoleState*)classP;
  double GLmatrix[16];
  mlr::Transformation f;
  f.setZero();
  //cart
  f.addRelativeTranslation(s->x,0.,1.);
  f.getAffineMatrixGL(GLmatrix);
  glLoadMatrixd(GLmatrix);
  glColor(.8,.2,.2);
  glDrawBox(1., .2, .2);
  //pole
  f.addRelativeRotationRad(s->th,0., 1., 0.);
  f.addRelativeTranslation(0., 0., .5);
  f.getAffineMatrixGL(GLmatrix);
  glLoadMatrixd(GLmatrix);
  glColor(.2,.2,.2);
  glDrawBox(.1, .1, 1.);
  glLoadIdentity();
#endif
}

void testDraw(){
  CartPoleState s;
  s.gl.watch();
}

void testMove(){
  CartPoleState s;
  for (uint t=0; t<400000; t++){
    s.step(0.0);
    s.gl.text.clear() <<t <<" ; " <<s.x1 << " ; " <<s.th1;
    s.gl.update();
  }
}



int main(int argc,char **argv){
  //testDraw();
  testMove();
  
  return 0;
}
