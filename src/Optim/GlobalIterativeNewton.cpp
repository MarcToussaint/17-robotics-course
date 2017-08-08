#include "GlobalIterativeNewton.h"

bool useNewton=true;

GlobalIterativeNewton::GlobalIterativeNewton(const ScalarFunction& f, const arr& bounds_lo, const arr& bounds_hi, OptOptions opt)
  : newton(x, f, opt),
    grad(x, f, opt),
    bounds_lo(bounds_lo), bounds_hi(bounds_hi),
    best(NULL) {
  newton.bound_lo = bounds_lo;
  newton.bound_hi = bounds_hi;
  newton.o.verbose = 0;
}

GlobalIterativeNewton::~GlobalIterativeNewton(){
}

void addRun(GlobalIterativeNewton& gin, const arr& x, double fx, double tol){
  GlobalIterativeNewton::LocalMinimum *found=NULL;
  for(GlobalIterativeNewton::LocalMinimum& m:gin.localMinima){
    double d = euclideanDistance(x, m.x);
    if(euclideanDistance(x,m.x)<tol){
      if(!found) found = &m;
      else if(d<euclideanDistance(x,found->x)) found = &m;
    }
  }

  if(found){
    found->hits++;
    if(fx<found->fx){
      found->x = x;
      found->fx = fx;
    }
  }else{
    gin.localMinima.append( {x, fx, 1} );
    found = &gin.localMinima.last();
    gin.best = NULL;
  }

  if(!gin.best){
    gin.best = &gin.localMinima.first();
    for(GlobalIterativeNewton::LocalMinimum& m:gin.localMinima) if(m.fx < gin.best->fx) gin.best = &m;
  }
  if(found->fx<gin.best->fx) gin.best=found;
  gin.newton.x = gin.best->x;
  gin.newton.fx = gin.best->fx;
  if(gin.newton.o.verbose>1) cout <<"***** optGlobalIterativeNewton: local minimum: " <<found->hits <<' ' <<found->fx <<' ' <<found->x <<endl;
}

void addRunFrom(GlobalIterativeNewton& gin, const arr& x){
  if(useNewton){
    gin.newton.reinit(x);
    gin.newton.run();
    addRun(gin, gin.newton.x, gin.newton.fx, 3.*gin.newton.o.stopTolerance);
  }else{
    gin.grad.reinit(x);
    gin.grad.run();
    addRun(gin, gin.grad.x, gin.grad.fx, 3.*gin.grad.o.stopTolerance);
  }
}

void GlobalIterativeNewton::step(){
  arr x = bounds_lo + (bounds_hi-bounds_lo) % rand(bounds_lo.N);
  if(newton.o.verbose>1) cout <<"***** optGlobalIterativeNewton: new iteration from x=" <<x <<endl;
  addRunFrom(*this, x);
}

void GlobalIterativeNewton::run(uint maxIt){
  for(uint i=0;i<maxIt;i++){
    step();
  }
}

void GlobalIterativeNewton::report(){
  cout <<"# local minima = " <<localMinima.N <<endl;
  uint i=0;
  for(LocalMinimum& m:localMinima){
    cout <<i++ <<' ' <<m.hits <<' ' <<m.fx <<" \t" <<m.x <<endl;
  }
}

void GlobalIterativeNewton::reOptimizeAllPoints(){
  if(!localMinima.N) return;
  arr X;
  for(LocalMinimum& m:localMinima) X.append(m.x);
  X.reshape(localMinima.N, X.N/localMinima.N);
  localMinima.clear();
  for(uint i=0;i<X.d0;i++) addRunFrom(*this, X[i]);
}
