#include <stdlib.h>
#include <Kin/roboticsCourse.h>
#include <Plot/plot.h>

void holdSteady(){
  Simulator S("pegArm.ors");
  S.setDynamicSimulationNoise(1.);
  S.setDynamicGravity(true);
  uint n=S.getJointDimension();

  double tau=.01;
  uint T=500;
  
  arr q,qdot;
  arr M,F,u(n);
  
  S.getJointAnglesAndVels(q, qdot);
  cout <<"initial posture (hit ENTER in the OpenGL window to continue!!)" <<endl;
  S.watch();
  
  for(uint t=0;t<=T;t++){
    S.getDynamics(M,F);

    //no controller torques
    u = 0.; //use M, F, and some desired qddot to compute u

    //dynamic simulation (simple Euler integration of the system dynamics, look into the code)
    S.stepDynamics(u, tau);
    S.watch(false);
    S.getJointAnglesAndVels(q, qdot);

    cout  <<" t=" <<tau*t  <<"sec E=" <<S.getEnergy()  <<"  q = " <<q <<endl;
  }
  S.watch();
}

int main(int argc,char **argv){

  holdSteady();
  
  return 0;
}
