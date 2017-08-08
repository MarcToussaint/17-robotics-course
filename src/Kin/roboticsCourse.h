/*  ------------------------------------------------------------------
    Copyright 2016 Marc Toussaint
    email: marc.toussaint@informatik.uni-stuttgart.de
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or (at
    your option) any later version. This program is distributed without
    any warranty. See the GNU General Public License for more details.
    You should have received a COPYING file of the full GNU General Public
    License along with this program. If not, see
    <http://www.gnu.org/licenses/>
    --------------------------------------------------------------  */

#ifndef MLR_simulator_h
#define MLR_simulator_h

#include <Kin/kin.h>
#include <Core/array.h>

struct Simulator {
  struct sSimulator *s; //hidden (private) space
  
  Simulator(const char* orsFile);
  ~Simulator();
  
  void watch(bool pause=true, const char *txt=NULL);                                    //pauses and lets you watch the OpenGL window
  
  //-- KINEMATICS
  //set the joint angles AND compute the frames of all bodies via
  //forward chaining of transformations AND update the robot display
  void setJointAngles(const arr& q, bool updateDisplay=true);
  void getJointAngles(arr& q);                     //get the joint angle vector for the current state
  uint getJointDimension();                        //get the dimensionality of q
  void kinematicsPos(arr& y, const char* shapeName, const arr* rel=0); //get the position of the body (names = "handR" or "handL", for instance)
  void jacobianPos(arr& J, const char* shapeName, const arr* rel=0);   //get the Jacobian of the body's position
  void kinematicsVec(arr& y, const char* shapeName, const arr* vec=0); //get the z-axis of the body
  void jacobianVec(arr& J, const char* shapeName, const arr* vec=0);   //get the Jacobian of the body's z-axis
  void kinematicsCOM(arr& y);
  void jacobianCOM(arr& J);
  void kinematicsContacts(arr& y);                 //get a scalar meassuring current collision costs
  void jacobianContacts(arr& J);                   //get gradient of the collision cost
  void setContactMargin(double margin);            //set the collision margin
  void reportProxies();                            //write info on collisions to console
  void anchorKinematicChainIn(const char* bodyName);

  //-- DYNAMICS
  //set the joint angles and velocities AND compute the frames and
  //linear & angular velocities of all bodies via
  //forward chaining of dynamic transformations AND update the robot display
  void setJointAnglesAndVels(const arr& q, const arr& qdot, bool updateDisplay=true);
  void getJointAnglesAndVels(arr& q, arr& qdot);
  void getDynamics(arr& M, arr& F); //get the mass matrix and force vector describing the system equation
  double getEnergy();

  //-- step dynamic simulation
  void stepDynamics(const arr& Bu, double tau);
  void setDynamicSimulationNoise(double noise);
  void setDynamicGravity(bool gravity);
  
  //-- PHYSICS
  void stepOde(const arr& qdot, double tau);
  void stepPhysx(const arr& qdot, double tau);

  //-- internal
  mlr::KinematicWorld& getOrsGraph();
};

struct VisionSimulator {
  struct sVisionSimulator *s;
  
  VisionSimulator();
  ~VisionSimulator();
  
  void watch();
  
  arr getCameraTranslation();
  void getRandomWorldPoints(arr& X, uint N);
  void projectWorldPointsToImagePoints(arr& x, const arr& X, double noiseInPixel=1.);
};


struct CarSimulator{
  struct Gaussian{  arr A;  arr a;  };

  double x,y,theta; //this is the true state -- accessing it means cheating...
  double tau,L;
  double dynamicsNoise, observationNoise;
  arr landmarks;
  arr particlesToDraw;
  mlr::Array<Gaussian> gaussiansToDraw;
  OpenGL *gl;
  
  CarSimulator();
  void step(const arr& u); 
  //get a (noisy) observation (meassuring landmarks) in the current state
  void getRealNoisyObservation(arr& Y);

  //-- access for tracking, where the true landmarks are known (Kalman/particle filter)
  //compute the ideal mean observation if you were in state (x,y,theta): use this to compute Y for a particle and compare with meassured observation
  void getMeanObservationAtState(arr& Y, const arr& X);
  void getObservationJacobianAtState(arr& dy_dx, const arr& X);

  //-- access for SLAM, where only estimated landmarks are accessible
  void getMeanObservationAtStateAndLandmarks(arr& Y, const arr& x, const arr& landmarks);
  void getObservationJacobianAtStateAndLandmarks(arr& dy_dx, arr& dy_dlandmarks, const arr& x, const arr& landmarks);

  void getLinearObservationModelAtState(arr& C, arr& c, const arr& X);

};


#endif


