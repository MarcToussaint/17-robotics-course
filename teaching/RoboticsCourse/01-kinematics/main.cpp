#include <Kin/roboticsCourse.h>


void simpleArrayOperations(){
  arr x;
  x = ARR(.1, .2, .3);         //directly setting the array
  cout <<"x = " <<x <<endl;

  x += ARR(2., 2., 2.);        //adding to an array
  cout <<"x = " <<x <<endl;

  x *= 1.;
  cout <<"x = " <<x <<endl;

  arr y = ARR(-.3,-.2,-.1);
  y.append(x);                 //appending a vector to a vector
  cout <<"y = " <<y <<endl;

  arr M(4,3);                  //some 3 x 4 matrix
  M[0] = ARR(1, 0, 0);         //setting the first row
  M[1] = ARR(0, 1, 0);
  M[2] = ARR(0, 0, 1);
  M[3] = ARR(1, 0, 0);

  cout <<"M =\n" <<M <<endl;

  cout <<"transpose M =\n" <<~M <<endl;  //matrix transpose

  cout <<"M*x = " <<M*x <<endl;          //matrix-vector product

  cout <<"M^-1 =\n" <<inverse(M) <<endl; //matrix inverse

  M.append(M);                 //appending a matrix to a matrix
  cout <<"M =\n" <<M <<endl;
}


void openingSimulator(){
  Simulator S("man.ors");
  cout <<"joint dimensions = " <<S.getJointDimension() <<endl;

  cout <<"initial posture (hit ENTER in the OpenGL window to continue!!)" <<endl;
  S.watch();        //pause and watch initial posture

  arr q;
  S.getJointAngles(q);

  q(0) += 0.1;                 //change the first entry of q-vector
  S.setJointAngles(q);
  S.watch();
  
  q = 0.;                      //set q-vector equal zero
  S.setJointAngles(q);
  S.watch();
}


void reach(){
  Simulator S("man.ors");
  arr q,W;
  uint n = S.getJointDimension();
  S.getJointAngles(q);
  double w = mlr::getParameter("w",1e-4);
  W.setDiag(w,n);  //W is equal the Id_n matrix times scalar w

  cout <<"initial posture (hit ENTER in the OpenGL window to continue!!)" <<endl;
  S.watch();        //pause and watch initial posture

  arr y_target,y,J;
  for(uint i=0;i<10;i++){
    //1st task:
    y_target = {-0.2, -0.4, 1.1}; 
    S.kinematicsPos(y,"handR");  //"handR" is the name of the right hand ("handL" for the left hand)
    S.jacobianPos  (J,"handR");

    //compute joint updates
    q += inverse(~J*J + W)*~J*(y_target - y); 
    //NOTATION: ~J is the transpose of J
    
    //sets joint angles AND computes all frames AND updates display
    S.setJointAngles(q);

    //optional: pause and watch OpenGL
    S.watch();
  }
}


int main(int argc,char **argv){
  mlr::initCmdLine(argc,argv);

  switch(mlr::getParameter<int>("mode",2)){
  case 0:  simpleArrayOperations();  break;
  case 1:  openingSimulator();  break;
  case 2:  reach();  break;
//  case 3:  circle();  break;
//  case 4:  multiTask();  break;
  }

  return 0;
}
