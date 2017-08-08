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


#include "dataNeighbored.h"

void DataNeighbored::setData(const arr& pts){
  X = pts;
  valid.resize(X.d0);
  for(uint i=0;i<X.d0;i++) if(pts(i,2)>=0) valid(i)=true; else valid(i)=false;
  if(isModelledWeights.N!=X.d0){
    isModelledWeights.resize(X.d0);
    isModelledWeights.setZero();
  }
  N.clear();
  idx2pixel.setStraightPerm(X.d0);
}

void DataNeighbored::setCosts(const arr& _costs){
  costs = _costs;
  CHECK_EQ(costs.N, X.d0, "");
}

uint DataNeighbored::n() const{ return X.d0; }

uint DataNeighbored::d() const{ return X.d1; }

void DataNeighbored::setGridNeighborhood(uint height, uint width, bool excludeNonValids){
  CHECK_EQ(width*height, X.d0, "");
  N.resize(X.d0);
  for(uint y=0;y<height;y++) for(uint x=0;x<width;x++){
    uint i=y*width + x, j;
    if(excludeNonValids && !valid(i)) continue;
    if(y){          j=(y-1)*width+(x  ); if(!excludeNonValids || valid(j)) N(i).append(j); }
    if(x){          j=(y  )*width+(x-1); if(!excludeNonValids || valid(j)) N(i).append(j); }
    if(y<height-1){ j=(y+1)*width+(x  ); if(!excludeNonValids || valid(j)) N(i).append(j); }
    if(x<width-1){  j=(y  )*width+(x+1); if(!excludeNonValids || valid(j)) N(i).append(j); }
  }
}

void DataNeighbored::removeNonValid(){
  uintA index(X.d0);
  index = X.d0;
  int s=0;
  for(uint i=0;i<X.d0;i++) if(valid(i)){ index(i)=s; s++; } //assign new indeces to each point
  idx2pixel.resize(s);
  for(uint i=0;i<X.d0;i++) if(valid(i)){
    uintA& Ni = N(i);
    for(uint& j:Ni) j=index(j); //use new indices in neighborhoods
    Ni.sort();                  //sort neighborhoods
    while(Ni.N && Ni.last()==X.d0) Ni.resizeCopy(Ni.N-1); //remove those, pointing to !ok (==X.d0 index)
  }
  for(uint i=0;i<X.d0;i++) if(valid(i)){
    if(index(i)!=i){
      X[index(i)] = X[i];
      N(index(i)) = N(i);
      isModelledWeights(index(i)) = isModelledWeights(i);
      costs(index(i)) = costs(i);
    }
    idx2pixel(index(i)) = i;
  }
  X.resizeCopy(s,X.d1);
  N.resizeCopy(s);
  isModelledWeights.resizeCopy(s);
  costs.resizeCopy(s);
}

void DataNeighbored::initFringe(uintA& fringe, uintA& pts, boolA& included, uint i){
  CHECK(valid(i),"");
  fringe.clear();
  fringe.append(i);
  pts = fringe;
  included.resize(X.d0);
  included.setZero();
  included(i) = true;
}

void DataNeighbored::expandFringe(uintA& fringe, uintA& pts, boolA& included){
  uintA newfringe;
  for(uint i:fringe) for(uint j:N(i)){
    if(valid(j) && !included(j)){
      newfringe.append(j);
      pts.append(j);
      included(j)=true;
    }
  }
  fringe = newfringe;
}

uintA DataNeighbored::getKneighborhood(uint i, uint k){
  CHECK(valid(i),"");
  uintA fringe, pts;
  boolA included;
  initFringe(fringe, pts, included, i);
  uintA Nk;

  for(;fringe.N;){
    if(Nk.N+fringe.N<=k){
      Nk.append(fringe);
      if(Nk.N==k) return Nk;
    }else for(uint j:fringe){
      Nk.append(j);
      if(Nk.N==k) return Nk;
    }

    expandFringe(fringe, pts, included);
  }
  return Nk;
}
