#include <Core/util.h>
#include <Kin/kin.h>
#include <Gui/opengl.h>

double
pqp_RectDist(double Rab[9], double Tab[3],
double a[2], double b[2], double Pa[3], double Pb[3]);

using mlr::Shape;

mlr::Vector Pa, Pb;

void draw(void*){
  glLoadIdentity();
  glColor(1., 0., 0., .9);
  glDrawDiamond(Pa.x, Pa.y, Pa.z, .1, .1, .1);
  glDrawDiamond(Pb.x, Pb.y, Pb.z, .1, .1, .1);
  glBegin(GL_LINES);
  glVertex3f(Pa.x, Pa.y, Pa.z);
  glVertex3f(Pb.x, Pb.y, Pb.z);
  glEnd();
  glLoadIdentity();
}

inline void clip(double& x, double r){
  if(x<0.) x=0.; else if(x>r) x=r;
}


double distance_SSPoints(mlr::Shape& A, mlr::Shape& B,mlr::Vector& Pa, mlr::Vector& Pb){
  CHECK(A.type==mlr::ST_retired_SSBox && B.type==mlr::ST_retired_SSBox,"");
  CHECK(!A.size(0) && !B.size(0) && !A.size(1) && !B.size(1) && !A.size(2) && !B.size(2), "can only handle SSpoints");
  Pa = A.X.pos;
  Pb = B.X.pos;
  mlr::Vector c = Pa-Pb;
  double d = c.length();
  //account for radii
  Pa -= A.size(3)*c/d;
  Pb += B.size(3)*c/d;
  return d-A.size(3)-B.size(3);
}

double distance_SSLinePoint(mlr::Shape& A, mlr::Shape& B,mlr::Vector& Pa, mlr::Vector& Pb){
  CHECK(A.type==mlr::ST_retired_SSBox && B.type==mlr::ST_retired_SSBox,"");
  CHECK(!B.size(0) && !A.size(1) && !B.size(1) && !A.size(2) && !B.size(2), "can only handle SSLinePoint");
  if(!A.size(0)){ //SSLinePoint
    return distance_SSPoints(A, B, Pa, Pb);
  }
  mlr::Vector a=A.X.rot.getX();
  mlr::Vector c=B.X.pos - A.X.pos;
  //get the 'coordinate' along the line segment
  double t = c*a;
  clip(t, A.size(0));
  //compute closest points
  Pa = A.X.pos + t*a;
  Pb = B.X.pos;
  //distance
  c = Pa-Pb;
  double d = c.length();
  //account for radii
  Pa -= A.size(3)*c/d;
  Pb += B.size(3)*c/d;
  return d-A.size(3)-B.size(3);
}

double distance_SSLines(mlr::Shape& A, mlr::Shape& B,mlr::Vector& Pa, mlr::Vector& Pb){
  CHECK(A.type==mlr::ST_retired_SSBox && B.type==mlr::ST_retired_SSBox,"");
  CHECK(!A.size(1) && !B.size(1) && !A.size(2) && !B.size(2), "can only handle SS line segments (cylinders)");
  if(!B.size(0)){ //SSLinePoint
    return distance_SSLinePoint(A, B, Pa, Pb);
  }
  mlr::Vector a=A.X.rot.getX();
  mlr::Vector b=B.X.rot.getX();
  mlr::Vector c=B.X.pos - A.X.pos;
  //get the 'coordinates' along the line segments
  double A_dot_B = a*b;
  double A_dot_C = a*c;
  double B_dot_C = b*c;
  double denom = 1. - A_dot_B*A_dot_B;
  double t, u;
  if(denom==0.) t=0.; else t = (A_dot_C - B_dot_C*A_dot_B)/denom;
  clip(t, A.size(0));
  u = t*A_dot_B - B_dot_C;
  clip(u, B.size(0));
  t = u*A_dot_B + A_dot_C;
  clip(t, A.size(0));
  //compute closest points
  Pa = A.X.pos + t*a;
  Pb = B.X.pos + u*b;
  //distance
  c = Pa-Pb;
  double d = c.length();
  //account for radii
  Pa -= A.size(3)*c/d;
  Pb += B.size(3)*c/d;
  return d-A.size(3)-B.size(3);
}

double distance_SSRects(mlr::Shape& A, mlr::Shape& B, mlr::Vector& Pa, mlr::Vector& Pb){
  CHECK(A.type==mlr::ST_ssBox && B.type==mlr::ST_ssBox,"");
  CHECK(!A.size(2) && !B.size(2), "can only handle spheres, cylinders & rectangles yet - no boxes");
  if(!A.size(1) && !B.size(1)){ //SSLines
    return distance_SSLines(A, B, Pa, Pb);
  }
  mlr::Transformation f;
  f.setDifference(A.X, B.X);
  mlr::Matrix R = ((f.rot)).getMatrix();
  mlr::Vector Asize={A.size(0), A.size(1), 0.};
  mlr::Vector Bsize={B.size(0), B.size(1), 0.};
  mlr::Vector trans = f.pos; //Asize + f.pos - R*Bsize;
  double dist = pqp_RectDist(R.p(), trans.p(), (Asize).p(), (Bsize).p(), Pa.p(), Pb.p());
  Pa = A.X * Pa;
  Pb = A.X * Pb;
   //distance
  mlr::Vector c = Pa-Pb;
  double d = c.length();
  if(dist>0.) CHECK_ZERO(dist-d, 1e-4, "NOT EQUAL!");
  if(dist==0.) d *= -1.; //if the rects penetrate already, measure the penetration as negative!
  //account for radii
  Pa -= A.size(3)*c/d;
  Pb += B.size(3)*c/d;
  return d-A.size(3)-B.size(3);
}


/* NOTE: All functions above: Internally they assume the shape's not centered, but extended from (0,0,0) to the positive coordinates
 * That is different to the 'Shape' convention, where shapes are centered and extend (with half length) to negative and positive coordinates
 * In the code this is transformed back and forth... */
double distance_(mlr::Shape& A, mlr::Shape& B, mlr::Vector& Pa, mlr::Vector& Pb){
  A.size(0)-=2.*A.size(3);  A.size(1)-=2.*A.size(3);  A.size(2)-=2.*A.size(3);
  B.size(0)-=2.*B.size(3);  B.size(1)-=2.*B.size(3);  B.size(2)-=2.*B.size(3);
  A.X.pos -= 0.5*(A.X.rot*mlr::Vector(A.size(0), A.size(1), A.size(2)));
  B.X.pos -= 0.5*(B.X.rot*mlr::Vector(B.size(0), B.size(1), B.size(2)));
  double d=distance_SSRects(A, B, Pa, Pb);
  A.X.pos += 0.5*(A.X.rot*mlr::Vector(A.size(0), A.size(1), A.size(2)));
  B.X.pos += 0.5*(B.X.rot*mlr::Vector(B.size(0), B.size(1), B.size(2)));
  A.size(0)+=2.*A.size(3);  A.size(1)+=2.*A.size(3);  A.size(2)+=2.*A.size(3);
  B.size(0)+=2.*B.size(3);  B.size(1)+=2.*B.size(3);  B.size(2)+=2.*B.size(3);
  return d;
}

void TEST(Distance){
  mlr::KinematicWorld W;
  mlr::Shape A(W, NoBody), B(W, NoBody);
  A.type = B.type = mlr::ST_ssBox;
  A.size = ARR(.6, .6, .1, .05);
  B.size = ARR(.6, .6, .1, .05);
  for(uint k=0;k<20;k++){
    A.X.setRandom(); A.X.pos(2) += 2.;
    B.X.setRandom(); B.X.pos(2) += 2.;
    double d=distance_(A, B, Pa, Pb);
    double d2=(Pa-Pb).length();
    cout <<"d=" <<d <<' ' <<d2 <<' ' <<Pa <<Pb <<endl;
    if(d>0.) CHECK_ZERO(d-d2, 1e-4, "NOT EQUAL!");
    mlr::Proxy p; p.posA=Pa; p.posB=Pb; p.colorCode=1;
    W.proxies.append( &p );
    W.gl().timedupdate(.1);
//    W.gl().watch();
    W.proxies.clear();
  }
}

int MAIN(int argc, char** argv){
  mlr::initCmdLine(argc, argv);

  testDistance();

  return 0;
}
