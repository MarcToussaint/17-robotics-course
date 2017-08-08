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

#include <iomanip>
#include "blackbox.h"

//===========================================================================

// void BayesOpt::query(const arr& x){
//   X.append(x);
//   y.append( f(NoArr, NoArr, x) );

//   double mu = sum(y)/double(y.N);
// //  double var = var(y);

//   if(model) delete model;
//   model = new KernelRidgeRegression(X, y, *kernel, -1., mu);
// }

//===========================================================================

extern "C"{
#include "CMA/cmaes_interface.h" //by Nikolaus Hansen
}

struct sSearchCMA{
  cmaes_t evo;
};

SearchCMA::SearchCMA(){
  s = new sSearchCMA;
}

SearchCMA::~SearchCMA(){
  delete s;
}

void SearchCMA::init(uint D, int mu, int lambda, const arr &startPoint, const arr &startDev){
  cmaes_init(&s->evo, NULL, D, startPoint.p, startDev.p, 1, lambda, mu, NULL);
}


void SearchCMA::init(uint D, int mu, int lambda, const arr &startPoint, double _startDev){
  CHECK_EQ(startPoint.N,D,"");
  arr startDev(D);
  startDev=_startDev;
  cmaes_init(&s->evo, NULL, D, startPoint.p, startDev.p, 1, lambda, mu, NULL);
}

void SearchCMA::init(uint D, int mu, int lambda, double lo, double hi){
  arr startPoint(D);
  rndUniform(startPoint, lo, hi, false);
  init(D, mu, lambda, startPoint, hi-lo);
}

void SearchCMA::step(arr& samples, arr& costs){
  if(costs.N){
    cmaes_ReestimateDistribution(&s->evo, costs.p);
  }else{ //first iteration: initialize arrays:
    samples.resize(s->evo.sp.lambda, s->evo.sp.N);
    costs.resize(s->evo.sp.lambda).setZero();
  }

  //generate samples
  double *const*rgx = cmaes_SampleDistribution(&s->evo, NULL);
  for(uint i=0;i<samples.d0;i++) samples[i].setCarray(rgx[i], samples.d1);
}

void SearchCMA::getBestSample(arr &sample) {
  sample.resize(s->evo.sp.N);
  sample.setCarray(s->evo.rgxbestever,sample.N);
}

void SearchCMA::getMean(arr &mean) {
  mean.resize(s->evo.sp.N);
  mean.setCarray(s->evo.rgxmean,mean.N);
}


