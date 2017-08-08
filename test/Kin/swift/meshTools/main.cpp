
#include <stdlib.h>

#include <Geo/mesh.h>
#include <Gui/opengl.h>

#include "swift_decomposer.inc"
#include <Gui/color.h>

const char *USAGE=
"\n\
Usage:  ors_meshTools file.[tri|obj|off|ply|stl] <tags...>\n\
\n\
Tags can be -view, -box, -fuse, -clean, -center, -scale, -swift, -save, -qhull, -flip, -decomp\n";


void drawInit(void*){
  glStandardLight(NULL);
  glDrawAxes(1.);
  glColor(1.,.5,0.);
}

void TEST(MeshTools) {
  cout <<USAGE <<endl;

  mlr::String file;
  if(mlr::argc>=2 && mlr::argv[1][0]!='-') file=mlr::argv[1];
  else file="../../Gui/opengl/base-male-nude.obj"; //m494.off

  cout <<"FILE=" <<file <<endl;
  OpenGL *gl=NULL;

  mlr::Mesh mesh;
  mesh.readFile(file);

  cout <<"#vertices = " <<mesh.V.d0 <<" #triangles=" <<mesh.T.d0 <<endl;

  file(file.N-4)=0; //replace . by 0

  //modify
  if(mlr::checkCmdLineTag("view")){
    cout <<"viewing..." <<endl;
    if(!gl) gl=new OpenGL;
    gl->clear();
    gl->add(drawInit);
    gl->add(mesh);
    gl->watch();
  }
  if(mlr::checkCmdLineTag("box")){
    cout <<"box" <<endl;
    mesh.box();
  }
  if(mlr::checkCmdLineTag("scale")){
    double s;
    mlr::getParameter(s,"scale");
    cout <<"scale " <<s <<endl;
    mesh.scale(s);
  }
  if(mlr::checkCmdLineTag("qhull")){
    cout <<"qhull..." <<endl;
    mesh.deleteUnusedVertices();
#ifdef MLR_QHULL
    getTriangulatedHull(mesh.T,mesh.V);
#else
    MLR_MSG("can'd use qhull - compiled without MLR_QHULL flag");
#endif
  }
  if(mlr::checkCmdLineTag("fuse")){
    double f;
    mlr::getParameter(f,"fuse");
    cout <<"fuse " <<f <<endl;
    mesh.fuseNearVertices(f);
  }
  if(mlr::checkCmdLineTag("clean")){
    cout <<"clean" <<endl;
    mesh.clean();
  }
  if(mlr::checkCmdLineTag("flip")){
    cout <<"clean" <<endl;
    //mesh.fuseNearVertices(1e-2);
    mesh.flipFaces();
  }
  if(mlr::checkCmdLineTag("center")){
    cout <<"center" <<endl;
    mesh.center();
  }
  if(mlr::checkCmdLineTag("swift")){
    mesh.writeTriFile(STRING(file<<"_x.tri"));
    mlr::String cmd;
    cmd <<"decomposer_c-vs6d.exe -df " <<file <<"_x.dcp -hf " <<file <<"_x.chr " <<file <<"_x.tri";
    cout <<"swift: " <<cmd <<endl;
    if(system(cmd)) MLR_MSG("system call failed");
  }
  if(mlr::checkCmdLineTag("decomp")){
    cout <<"decomposing..." <<endl;
    intA triangleAssignments;
    mlr::Array<mlr::Array<uint> > shapes;
    decompose(mesh, STRING(file<<"_x.dcp"), triangleAssignments, shapes);
    mesh.C.resize(mesh.T.d0,3);
    for(uint t=0;t<mesh.T.d0;t++){
      mlr::Color col;
      col.setIndex(triangleAssignments(t));
      mesh.C(t,0) = col.r;  mesh.C(t,1) = col.g;  mesh.C(t,2) = col.b;
    }
  }
  if(mlr::checkCmdLineTag("view")){
    cout <<"viewing..." <<endl;
    if(!gl) gl=new OpenGL;
    gl->clear();
    gl->add(drawInit);
    gl->add(mesh);
    gl->watch();
  }
  if(mlr::checkCmdLineTag("save")){
    cout <<"saving..." <<endl;
    cout << "\tto " << file <<"_x.tri" << endl;
    mesh.writeTriFile(STRING(file<<"_x.tri"));
    cout << "\tto " << file <<"_x.off" << endl;
    mesh.writeOffFile(STRING(file<<"_x.off"));
    cout << "\tto " << file <<"_x.ply" << endl;
    mesh.writePLY(STRING(file<<"_x.ply"), true);
  }

  cout <<"#vertices = " <<mesh.V.d0 <<" #triangles=" <<mesh.T.d0 <<endl;
}

int MAIN(int argc, char** argv){
  mlr::initCmdLine(argc, argv);

  testMeshTools();

  return 1;
}
