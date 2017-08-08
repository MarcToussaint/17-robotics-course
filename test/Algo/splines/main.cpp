#include <Algo/spline.h>
#include <Plot/plot.h>
#include <Kin/kin.h>

ScalarFunction cost = [](arr &g, arr &H, const arr &x) -> double{
  uint t;
  double C=0.;
  //obstacle
  /*  for(t=0;t<f.d0;t++){
    if(f(t,0)<1. && f(t,1)>0.){
      (*grad)(t,0) += .01*(f(t,0)-1.);
      (*grad)(t,1) += .01*f(t,1);
    }
  }*/

  //gravity
  for(t=0;t<x.d0;t++){
    C += x(t,1)*.001;
  }
  //tension
  for(t=1;t<x.d0;t++){
    C += sumOfSqr(x[t]-x[t-1]);
  }
  //goals
  for(t=0;t<x.d0/2;t++) C += .1*sumOfSqr(x[t]-0.);
  t=0;       C += sumOfSqr(x[t]-0.);
  t=x.d0-1;  C += sumOfSqr(x[t]-1.);

  if(&g){
    g.resizeAs(x);
    g.setZero();
    //gravity
    for(t=0;t<x.d0;t++){
      g(t,1) += .001;
    }
    //tension
    for(t=0;t<x.d0;t++){
      if(t>0)      g[t]() += 2.*(x[t]-x[t-1]);
      if(t+1<x.d0) g[t]() -= 2.*(x[t+1]-x[t]);
    }
    //goals
    for(t=0;t<x.d0/2;t++)      g[t]() += .2*(x[t]-0.);
    t=0;       g[t]() += 2.*(x[t]-0.);
    t=x.d0-1;  g[t]() += 2.*(x[t]-1.);
  }

  return C;
};


void TEST(BSpline){
  uint K=6,T=100; //6 spline point, discrete path with T=100

  arr X(K,2); //spline points
  rndUniform(X,-1,1,false);

  mlr::Spline S(T, X, 2);

  cout <<"times = " <<S.times <<endl;

  plotOpengl();
  plotModule.drawBox=true;
  S.plotBasis();
  
  arr path = S.eval();
  plotClear();
  plotFunction(path);
  plotFunction(S.points);
  plotPoints(S.points);
  plot(false);

  for(double lambda = 0.; lambda < .1; lambda += .001) {
    path = S.smooth(lambda);
    plotClear();
    plotFunction(path);
    plotFunction(S.points);
    plotPoints(S.points);
    plot(false);
  }

  ofstream fil("z.test");
  for(uint t=0;t<=1000;t++){
    fil <<(double)t/1000 <<' ' <<S.eval(t/10) <<' ' <<S.eval((double)t/1000) <<endl;
  }
  fil.close();
  gnuplot("plot 'z.test' us 1:2, '' us 1:4", true);

  //Cost cost;

  ScalarFunction splineCost = [&S](arr &g, arr &H, const arr &x) -> double{
    double c=cost(g, NoArr, S.basis*x);
    if(&g) g=S.basis_trans*g;
    return c;
  };


  arr grad_path, grad_X;
  for(uint i=0;i<100;i++){
    checkGradient(cost, path, 1e-5);
    checkGradient(splineCost, S.points, 1e-5);
    cost(grad_path, NoArr, path);
    //S.partial(dCdx,dCdt,dCdf,true);
    if(grad_path.d0==S.points.d0){
      S.points -= .3 * grad_path;
    }else{
      S.partial(grad_X, grad_path);
      S.points -= .3 * grad_X;
    }
    if(i>50){
      //S.times  -= .3 * dCdt;
      //S.setBasisAndTimeGradient();
    }
    path = S.eval();
    cout <<cost(NoArr, NoArr, path) <<endl;

    plotClear();
    plotFunction(path);
    plotFunction(S.points);
    plotPoints(S.points);
    plot(false);
  }

  plot(true);

  plotClose();
}


void TEST(Path){
  arr X(11,1);
  rndUniform(X,-1,1,false);

  mlr::Path P(X);
  cout <<"times = " <<P.times
      <<"\npoints= " <<P.points <<endl;

  //-- gradient check of velocity
  VectorFunction Test = [&P](arr& y, arr& J, const arr& x) -> void {
    CHECK_EQ(x.N,1,"");
    y = P.getPosition(x(0));
    if(&J) J = P.getVelocity(x(0));
  };
  for(uint k=0;k<10;k++){
    arr x(1);
    x(0) = rnd.uni();
    checkJacobian(Test, x, 1e-4);
  }

  //-- transform spline
//  P.transform_CurrentBecomes_EndFixed(ARR(1.), .25);
//  P.transform_CurrentFixed_EndBecomes(ARR(-1.), .25);

  //-- write spline
  mlr::arrayBrackets="  ";
  FILE("z.points") <<X;

  ofstream fil("z.test");
  for(uint t=0;t<=1000;t++){
    double time=(double)t/1000;
    fil <<time <<' ' <<P.getPosition(time) <<' ' <<P.getVelocity(time) <<endl;
  }
  fil.close();
  gnuplot("plot 'z.test' us 1:2 t 'pos', '' us 1:3 t 'vel', 'z.points' us ($0/10):1 w p", true);

}

int MAIN(int argc,char** argv){
  mlr::initCmdLine(argc, argv);

  testBSpline();
//  testPath();


  return 0;
}
