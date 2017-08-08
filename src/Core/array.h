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

/// @file
/// @ingroup group_array
/// @addtogroup group_array
/// @{

#ifndef MLR_array_h
#define MLR_array_h

#include <iostream>
#include <stdint.h>
#include <string.h>
#include <functional>
#include <memory>

//-- TODO: old, remove
#define FOR1D(x, i)   for(i=0;i<x.N;i++)
#define FOR1D_DOWN(x, i)   for(i=x.N;i--;)
#define FOR2D(x, i, j) for(i=0;i<x.d0;i++) for(j=0;j<x.d1;j++)
#define FOR3D(x, i, j, k) for(i=0;i<x.d0;i++) for(j=0;j<x.d1;j++) for(k=0;k<x.d2;k++)

//-- don't require previously defined iterators
#define for_list(Type, it, X)     Type *it=NULL; for(uint it##_COUNT=0;   it##_COUNT<X.N && ((it=X(it##_COUNT)) || true); it##_COUNT++)
#define for_list_rev(Type, it, X) Type *it=NULL; for(uint it##_COUNT=X.N; it##_COUNT--   && ((it=X(it##_COUNT)) || true); )

#define ARR ARRAY<double> ///< write ARR(1., 4., 5., 7.) to generate a double-Array
#define TUP ARRAY<uint> ///< write TUP(1, 2, 3) to generate a uint-Array

typedef unsigned char byte;
typedef unsigned int uint;
struct SpecialArray;

//-- global memory information and options TODO: hide -> array.cpp
namespace mlr {
extern bool useLapack;
extern const bool lapackSupported;
extern uint64_t globalMemoryTotal, globalMemoryBound;
extern bool globalMemoryStrict;
extern const char* arrayElemsep;
extern const char* arrayLinesep;
extern const char* arrayBrackets;
struct FileToken;
template<class T> bool lower(const T& a, const T& b){ return a<b; }
template<class T> bool lowerEqual(const T& a, const T& b){ return a<=b; }
template<class T> bool greater(const T& a, const T& b){ return a>b; }
template<class T> bool greaterEqual(const T& a, const T& b){ return a>=b; }
} //namespace

//===========================================================================
//
// Array class
//

namespace mlr {

/// @addtogroup group_array
/// @{

/** Simple array container to store arbitrary-dimensional arrays (tensors).
  Can buffer more memory than necessary for faster
  resize; enables non-const reference of subarrays; enables fast
  memove for elementary types; implements many standard
  array/matrix/tensor operations. Interfacing with ordinary C-buffers is simple.
  Please see also the reference for the \ref array.h
  header, which contains lots of functions that can be applied on
  Arrays. */
template<class T> struct Array {
  T *p;     ///< the pointer on the linear memory allocated
  uint N;   ///< number of elements
  uint nd;  ///< number of dimensions
  uint d0,d1,d2;  ///< 0th, 1st, 2nd dim
  uint *d;  ///< pointer to dimensions (for nd<=3 points to d0)
  uint M;   ///< size of actually allocated memory (may be greater than N)
  bool reference; ///< true if this refers to some external memory
  
  static int  sizeT;   ///< constant for each type T: stores the sizeof(T)
  static char memMove; ///< constant for each type T: decides whether memmove can be used instead of individual copies

  //-- special: arrays can be sparse/packed/etc and augmented with aux data to support this
  SpecialArray *special; ///< arbitrary auxiliary data, depends on special

  typedef bool (*ElemCompare)(const T& a, const T& b);

  /// @name constructors
  Array();
  Array(const Array<T>& a);                 //copy constructor
  explicit Array(uint D0);
  explicit Array(uint D0, uint D1);
  explicit Array(uint D0, uint D1, uint D2);
  explicit Array(const T* p, uint size, bool byReference=true);    //reference!
  Array(std::initializer_list<T> values);
  Array(uint D0, std::initializer_list<T> values);
  Array(uint D0, uint D1, std::initializer_list<T> values);
  Array(uint D0, uint D1, uint D2, std::initializer_list<T> values);
  Array(mlr::FileToken&); //read from a file
  ~Array();
  
  Array<T>& operator=(std::initializer_list<T> values);
  Array<T>& operator=(const T& v);
  Array<T>& operator=(const Array<T>& a);

  /// @name iterators
  typedef T* iterator;
  typedef const T* const_iterator;
  iterator begin() { return p; }
  iterator end() { return p+N; }
  const_iterator begin() const { return p; }
  const_iterator end() const { return p+N; }

  /// @name resizing
  Array<T>& resize(uint D0);
  Array<T>& resize(uint D0, uint D1);
  Array<T>& resize(uint D0, uint D1, uint D2);
  Array<T>& resize(uint ND, uint *dim);
  Array<T>& resize(const Array<uint> &dim);
  Array<T>& reshape(uint D0);
  Array<T>& reshape(uint D0, uint D1);
  Array<T>& reshape(uint D0, uint D1, uint D2);
  Array<T>& reshape(uint ND, uint *dim);
  Array<T>& reshape(const Array<uint> &dim);
  Array<T>& resizeCopy(uint D0);
  Array<T>& resizeCopy(uint D0, uint D1);
  Array<T>& resizeCopy(uint D0, uint D1, uint D2);
  Array<T>& resizeCopy(uint ND, uint *dim);
  Array<T>& resizeCopy(const Array<uint> &dim);
  Array<T>& resizeAs(const Array<T>& a);
  Array<T>& reshapeAs(const Array<T>& a);
  Array<T>& resizeCopyAs(const Array<T>& a);
  Array<T>& reshapeFlat();
  Array<T>& dereference();

  /// @name initializing/assigning entries
  void clear();
  void setZero(byte zero=0);
  void setUni(const T& scalar, int d=-1);
  void setId(int d=-1);
  void setDiag(const T& scalar, int d=-1);
  void setDiag(const Array<T>& vector);
  void setBlockMatrix(const Array<T>& A, const Array<T>& B, const Array<T>& C, const Array<T>& D);
  void setBlockMatrix(const Array<T>& A, const Array<T>& B);
  void setBlockVector(const Array<T>& a, const Array<T>& b);
  void setMatrixBlock(const Array<T>& B, uint lo0, uint lo1);
  void setVectorBlock(const Array<T>& B, uint lo);
  void setStraightPerm(int n=-1);
  void setReversePerm(int n=-1);
  void setRandomPerm(int n=-1);
  void setCarray(const T *buffer, uint D0);
  void setCarray(const T **buffer, uint D0, uint D1);
  void referTo(const T *buffer, uint n);
  void referTo(const Array<T>& a);
  void referToRange(const Array<T>& a, int i, int I); // -> referTo(a,{i,I})
  void referToRange(const Array<T>& a, uint i, int j, int J); // -> referTo(a,{i,I})
  void referToDim(const Array<T>& a, uint i); // -> referTo
  void referToDim(const Array<T>& a, uint i, uint j);
  void referToDim(const Array<T>& a, uint i, uint j, uint k);
  void takeOver(Array<T>& a);  //a becomes a reference to its previously owned memory!
  void swap(Array<T>& a);      //the two arrays swap their contents!
  void setGrid(uint dim, T lo, T hi, uint steps);
  
  /// @name access by reference (direct memory access)
  T& elem(int i) const;
  T& elem(const Array<uint> &I) const;
  T& scalar() const;
  T& first() const;
  T& last(int i=-1) const;
  T& rndElem() const;
  T& operator()(uint i) const;
  T& operator()(uint i, uint j) const;
  T& operator()(uint i, uint j, uint k) const;
  Array<T> operator()(std::pair<int, int> I) const;
  Array<T> operator()(uint i, std::pair<int, int> J) const;
//  Array<T> operator()(uint i, std::initializer_list<int> J ) const;
  Array<T> operator()(uint i, uint j, std::initializer_list<int> K) const;
  Array<T> operator[](uint i) const;     // calls referToDim(*this, i)
  Array<T> operator[](std::initializer_list<uint> list) const; //-> remove
  Array<T>& operator()(){ return *this; } //TODO: replace by scalar reference!
  T** getCarray(Array<T*>& Cpointers) const;

  
  /// @name access by copy
  mlr::Array<T> copy() const;
  Array<T> sub(int i, int I) const;
  Array<T> sub(int i, int I, int j, int J) const;
  Array<T> sub(int i, int I, int j, int J, int k, int K) const;
  Array<T> sub(int i, int I, Array<uint> cols) const;
  Array<T> row(uint row_index) const;
  Array<T> rows(uint start_row, uint end_row) const;
  Array<T> col(uint col_index) const;
  Array<T> cols(uint start_col, uint end_col) const;

  /// @name dimensionality access
  uint dim(uint k) const;
  Array<uint> dim() const;

  void getMatrixBlock(Array<T>& B, uint lo0, uint lo1) const; // -> return array
  void getVectorBlock(Array<T>& B, uint lo) const;
  void copyInto(T *buffer) const;
  void copyInto2D(T **buffer) const;
  T& min() const;
  T& max() const;
  void minmax(T& minVal, T& maxVal) const;
  uint minIndex() const; // -> argmin
  uint maxIndex() const; // -> argmax
  void maxIndeces(uint& m1, uint& m2) const; //best and 2nd best -> remove
  void maxIndex(uint& i, uint& j) const; //-> remove, or return uintA
  void maxIndex(uint& i, uint& j, uint& k) const; //-> remove
  int findValue(const T& x) const;
  void findValues(mlr::Array<uint>& indices, const T& x) const;
  bool contains(const T& x) const { return findValue(x)!=-1; }
  bool containsDoubles() const;
  uint getMemsize() const; // -> remove
  void getIndexTuple(Array<uint> &I, uint i) const; // -> remove?
  
  /// @name appending etc
  T& append();
  T& append(const T& x);
  void append(const T& x, uint multiple);
  void append(const Array<T>& x);
  void append(const T *p, uint n);
  void prepend(const T& x){ insert(0,x); }
  void prepend(const Array<T>& x){ insert(0,x); }
  void replicate(uint copies);
  void insert(uint i, const T& x);
  void insert(uint i, const Array<T>& x);
  void replace(uint i, uint n, const Array<T>& x);
  void remove(uint i, uint n=1);
  void removePerm(uint i);          //more efficient for sets, works also for non-memMove arrays
  bool removeValue(const T& x, bool errorIfMissing=true);
  void removeAllValues(const T& x);
  void delRows(uint i, uint k=1);
  void delColumns(uint i, uint k=1);
  void insRows(uint i, uint k=1);
  void insColumns(uint i, uint k=1);
  void resizeDim(uint k, uint dk);
  void setAppend(const T& x); //? same as if(findValue(x)==-1) append(x)
  void setAppend(const Array<T>& x);
  T popFirst();
  T popLast();
  
  /// @name sorting and permuting this array
  void sort(ElemCompare comp=lowerEqual);
  bool isSorted(ElemCompare comp=lowerEqual) const;
  uint rankInSorted(const T& x, ElemCompare comp=lowerEqual, bool rankAfterIfEqual=false) const;
  int findValueInSorted(const T& x, ElemCompare comp=lowerEqual) const;
  uint insertInSorted(const T& x, ElemCompare comp=lowerEqual, bool insertAfterIfEqual=false);
  uint setAppendInSorted(const T& x, ElemCompare comp=lowerEqual);
  void removeValueInSorted(const T& x, ElemCompare comp=lowerEqual);
  void reverse();
  void reverseRows();
  void permute(uint i, uint j);
  void permute(const Array<uint>& permutation);
  void permuteInv(const Array<uint>& permutation);
  void permuteRows(const Array<uint>& permutation);
  void permuteRowsInv(const Array<uint>& permutation);

  void permuteRandomly();
  void shift(int offset, bool wrapAround=true);
  
  /// @name special matrices [TODO: move outside, use 'special']
  double sparsity();
  void makeSparse();
  
  /// @name I/O
  void write(std::ostream& os=std::cout, const char *ELEMSEP=NULL, const char *LINESEP=NULL, const char *BRACKETS=NULL, bool dimTag=false, bool binary=false) const;
  void read(std::istream& is);
  void writeTagged(std::ostream& os, const char* tag, bool binary=false) const;
  bool readTagged(std::istream& is, const char *tag);
  void writeTagged(const char* filename, const char* tag, bool binary=false) const;
  bool readTagged(const char* filename, const char *tag);
  void writeDim(std::ostream& os=std::cout) const;
  void readDim(std::istream& is);
  void writeRaw(std::ostream& os) const;
  void readRaw(std::istream& is);
  void writeWithIndex(std::ostream& os=std::cout) const;
  const Array<T>& ioraw() const;
  const char* prt(); //gdb pretty print
  
  /// @name kind of private
  void resizeMEM(uint n, bool copy, int Mforce=-1);
  void anticipateMEM(uint Mforce){ resizeMEM(N, true, Mforce); if(!nd) nd=1; }
  void freeMEM();
  void resetD();
//  void init();
};

//===========================================================================
/// @name basic Array operators
/// @{

template<class T> Array<T> operator~(const Array<T>& y); //transpose
template<class T> Array<T> operator-(const Array<T>& y); //negative
template<class T> Array<T> operator^(const Array<T>& y, const Array<T>& z); //outer product
template<class T> Array<T> operator%(const Array<T>& y, const Array<T>& z); //index/element-wise product
template<class T> Array<T> operator*(const Array<T>& y, const Array<T>& z); //inner product
template<class T> Array<T> operator*(const Array<T>& y, T z);
template<class T> Array<T> operator*(T y, const Array<T>& z);
template<class T> Array<T> operator/(int mustBeOne, const Array<T>& z_tobeinverted);
template<class T> Array<T> operator/(const Array<T>& y, T z);
template<class T> Array<T> operator/(const Array<T>& y, const Array<T>& z); //element-wise devision
template<class T> Array<T> operator|(const Array<T>& A, const Array<T>& B); //A^-1 B
template<class T> Array<T> operator,(const Array<T>& y, const Array<T>& z); //concat

template<class T> Array<T>& operator<<(Array<T>& x, const Array<T>& y); //append

template<class T> bool operator==(const Array<T>& v, const Array<T>& w);
template<class T> bool operator==(const Array<T>& v, const T *w);
template<class T> bool operator!=(const Array<T>& v, const Array<T>& w);
template<class T> bool operator<(const Array<T>& v, const Array<T>& w);
template<class T> std::istream& operator>>(std::istream& is, Array<T>& x);
template<class T> std::ostream& operator<<(std::ostream& os, const Array<T>& x);

//element-wise update operators
#ifndef SWIG
#define UpdateOperator( op )        \
  template<class T> Array<T>& operator op (Array<T>& x, const Array<T>& y); \
  template<class T> Array<T>& operator op (Array<T>& x, T y ); \
  template<class T> void operator op (Array<T>&& x, const Array<T>& y); \
  template<class T> void operator op (Array<T>&& x, T y );
UpdateOperator(|=)
UpdateOperator(^=)
UpdateOperator(&=)
UpdateOperator(+=)
UpdateOperator(-=)
UpdateOperator(*=)
UpdateOperator(/=)
UpdateOperator(%=)
#undef UpdateOperator
#endif

//element-wise operators
#define BinaryOperator( op, updateOp)         \
  template<class T> Array<T> operator op(const Array<T>& y, const Array<T>& z); \
  template<class T> Array<T> operator op(T y, const Array<T>& z);  \
  template<class T> Array<T> operator op(const Array<T>& y, T z)
BinaryOperator(+ , +=);
BinaryOperator(- , -=);
//BinaryOperator(% , *=);
//BinaryOperator(/ , /=);
#undef BinaryOperator

/// @} //name
/// @} //group
} //namespace

//===========================================================================
/// @name basic Array functions
/// @{

#ifndef SWIG
#define UnaryFunction( func )           \
  template<class T> mlr::Array<T> func (const mlr::Array<T>& y)
UnaryFunction(acos);
UnaryFunction(asin);
UnaryFunction(atan);
UnaryFunction(cos);
UnaryFunction(sin);
UnaryFunction(tan);
UnaryFunction(cosh);
UnaryFunction(sinh);
UnaryFunction(tanh);
UnaryFunction(acosh);
UnaryFunction(asinh);
UnaryFunction(atanh);
UnaryFunction(exp);
UnaryFunction(log);
UnaryFunction(log10);
UnaryFunction(sqrt);
UnaryFunction(cbrt);
UnaryFunction(ceil);
UnaryFunction(fabs);
UnaryFunction(floor);
UnaryFunction(sigm);
UnaryFunction(sign);
#undef UnaryFunction

#define BinaryFunction( func )            \
  template<class T> mlr::Array<T> func(const mlr::Array<T>& y, const mlr::Array<T>& z); \
  template<class T> mlr::Array<T> func(const mlr::Array<T>& y, T z); \
  template<class T> mlr::Array<T> func(T y, const mlr::Array<T>& z)
BinaryFunction(atan2);
BinaryFunction(pow);
BinaryFunction(fmod);
#undef BinaryFunction

#endif //SWIG

//===========================================================================
/// @}
/// @name standard types
/// @{

typedef mlr::Array<double> arr;
typedef mlr::Array<float>  arrf;
typedef mlr::Array<double> doubleA;
typedef mlr::Array<float>  floatA;
typedef mlr::Array<uint>   uintA;
typedef mlr::Array<int>    intA;
typedef mlr::Array<char>   charA;
typedef mlr::Array<byte>   byteA;
typedef mlr::Array<bool>   boolA;
typedef mlr::Array<uint16_t>   uint16A;
typedef mlr::Array<uint32_t>   uint32A;
typedef mlr::Array<const char*>  CstrList;
typedef mlr::Array<arr*>   arrL;
typedef mlr::Array<arr>    arrA;
typedef mlr::Array<uintA>    uintAA;

namespace mlr { struct String; }
typedef mlr::Array<mlr::String> StringA;
typedef mlr::Array<StringA> StringAA;
typedef mlr::Array<mlr::String*> StringL;

//===========================================================================
/// @}
/// @name constant non-arrays
/// @{

extern arr& NoArr; //this is a pointer to NULL!!!! I use it for optional arguments
extern arrA& NoArrA; //this is a pointer to NULL!!!! I use it for optional arguments
extern uintA& NoUintA; //this is a pointer to NULL!!!! I use it for optional arguments
extern byteA& NoByteA; //this is a pointer to NULL!!!! I use it for optional arguments
extern uintAA& NoUintAA; //this is a pointer to NULL!!!! I use it for optional arguments

//===========================================================================
/// @}
/// @name basic function types
/// @{

/// a scalar function \f$f:~x\mapsto y\in\mathbb{R}\f$ with optional gradient and hessian
//struct ScalarFunction {
//  virtual double fs(arr& g, arr& H, const arr& x) = 0;
//  virtual ~ScalarFunction(){}
//};

typedef std::function<double(arr& g, arr& H, const arr& x)> ScalarFunction;

/// a vector function \f$f:~x\mapsto y\in\mathbb{R}^d\f$ with optional Jacobian
//struct VectorFunction {
//  virtual void fv(arr& y, arr& J, const arr& x) = 0; ///< returning a vector y and (optionally, if NoArr) Jacobian J for x
//  virtual ~VectorFunction(){}
//};
typedef std::function<void(arr& y, arr& J, const arr& x)> VectorFunction;

/// a kernel function
struct KernelFunction {
  virtual double k(const arr& x1, const arr& x2, arr& g1=NoArr, arr& Hx1=NoArr) = 0;
  virtual ~KernelFunction(){}
};

//===========================================================================

template<class T> mlr::Array<T> ARRAY() {                                    mlr::Array<T> z(0); return z; }
template<class T> mlr::Array<T> ARRAY(const T& i) {                                    mlr::Array<T> z(1); z(0)=i; return z; }
template<class T> mlr::Array<T> ARRAY(const T& i, const T& j) {                               mlr::Array<T> z(2); z(0)=i; z(1)=j; return z; }
template<class T> mlr::Array<T> ARRAY(const T& i, const T& j, const T& k) {                          mlr::Array<T> z(3); z(0)=i; z(1)=j; z(2)=k; return z; }
template<class T> mlr::Array<T> ARRAY(const T& i, const T& j, const T& k, const T& l) {                     mlr::Array<T> z(4); z(0)=i; z(1)=j; z(2)=k; z(3)=l; return z; }
template<class T> mlr::Array<T> ARRAY(const T& i, const T& j, const T& k, const T& l, const T& m) {                mlr::Array<T> z(5); z(0)=i; z(1)=j; z(2)=k; z(3)=l; z(4)=m; return z; }
template<class T> mlr::Array<T> ARRAY(const T& i, const T& j, const T& k, const T& l, const T& m, const T& n) {           mlr::Array<T> z(6); z(0)=i; z(1)=j; z(2)=k; z(3)=l; z(4)=m; z(5)=n; return z; }
template<class T> mlr::Array<T> ARRAY(const T& i, const T& j, const T& k, const T& l, const T& m, const T& n, const T& o) {      mlr::Array<T> z(7); z(0)=i; z(1)=j; z(2)=k; z(3)=l; z(4)=m; z(5)=n; z(6)=o; return z; }
template<class T> mlr::Array<T> ARRAY(const T& i, const T& j, const T& k, const T& l, const T& m, const T& n, const T& o, const T& p) { mlr::Array<T> z(8); z(0)=i; z(1)=j; z(2)=k; z(3)=l; z(4)=m; z(5)=n; z(6)=o; z(7)=p; return z; }
template<class T> mlr::Array<T> ARRAY(const T& i, const T& j, const T& k, const T& l, const T& m, const T& n, const T& o, const T& p, const T& q) { mlr::Array<T> z(9); z(0)=i; z(1)=j; z(2)=k; z(3)=l; z(4)=m; z(5)=n; z(6)=o; z(7)=p; z(8)=q; return z; }

template<class T> mlr::Array<T*> LIST() {                                    mlr::Array<T*> z(0); return z; }
template<class T> mlr::Array<T*> LIST(const T& i) {                                    mlr::Array<T*> z(1); z(0)=(T*)&i; return z; }
template<class T> mlr::Array<T*> LIST(const T& i, const T& j) {                               mlr::Array<T*> z(2); z(0)=(T*)&i; z(1)=(T*)&j; return z; }
template<class T> mlr::Array<T*> LIST(const T& i, const T& j, const T& k) {                          mlr::Array<T*> z(3); z(0)=(T*)&i; z(1)=(T*)&j; z(2)=(T*)&k; return z; }
template<class T> mlr::Array<T*> LIST(const T& i, const T& j, const T& k, const T& l) {                     mlr::Array<T*> z(4); z(0)=(T*)&i; z(1)=(T*)&j; z(2)=(T*)&k; z(3)=(T*)&l; return z; }
template<class T> mlr::Array<T*> LIST(const T& i, const T& j, const T& k, const T& l, const T& m) {                mlr::Array<T*> z(5); z(0)=(T*)&i; z(1)=(T*)&j; z(2)=(T*)&k; z(3)=(T*)&l; z(4)=(T*)&m; return z; }
template<class T> mlr::Array<T*> LIST(const T& i, const T& j, const T& k, const T& l, const T& m, const T& n) {           mlr::Array<T*> z(6); z(0)=(T*)&i; z(1)=(T*)&j; z(2)=(T*)&k; z(3)=(T*)&l; z(4)=(T*)&m; z(5)=(T*)&n; return z; }
template<class T> mlr::Array<T*> LIST(const T& i, const T& j, const T& k, const T& l, const T& m, const T& n, const T& o) {      mlr::Array<T*> z(7); z(0)=(T*)&i; z(1)=(T*)&j; z(2)=(T*)&k; z(3)=(T*)&l; z(4)=(T*)&m; z(5)=(T*)&n; z(6)=(T*)&o; return z; }
template<class T> mlr::Array<T*> LIST(const T& i, const T& j, const T& k, const T& l, const T& m, const T& n, const T& o, const T& p) { mlr::Array<T*> z(8); z(0)=(T*)&i; z(1)=(T*)&j; z(2)=(T*)&k; z(3)=(T*)&l; z(4)=(T*)&m; z(5)=(T*)&n; z(6)=(T*)&o; z(7)=(T*)&p; return z; }


//===========================================================================
/// @}
/// @name Octave/Matlab functions to generate special arrays
/// @{

/// return identity matrix
inline arr eye(uint d0, uint d1) { arr z(d0, d1);  z.setId();  return z; }
/// return identity matrix
inline arr eye(uint n) { return eye(n, n); }
/// return the ith standard basis vector (ith column of the Id matrix)
inline arr eyeVec(uint n, uint i) { arr z(n); z.setZero(); z(i)=1.; return z; }

/// return array of ones
inline arr ones(const uintA& d) {  arr z;  z.resize(d);  z=1.;  return z;  }
/// return VECTOR of ones
inline arr ones(uint n) { return ones(TUP(n)); }
/// return matrix of ones
inline arr ones(uint d0, uint d1) { return ones(TUP(d0, d1)); }

/// return array of zeros
inline arr zeros(const uintA& d) {  arr z;  z.resize(d);  z.setZero();  return z; }
/// return VECTOR of zeros
inline arr zeros(uint n) { return zeros(TUP(n)); }
/// return matrix of zeros
inline arr zeros(uint d0, uint d1) { return zeros(TUP(d0, d1)); }
/// return tensor of zeros
inline arr zeros(uint d0, uint d1, uint d2) { return zeros(TUP(d0, d1, d2)); }

/// return array of c's
template<class T> mlr::Array<T> consts(const T& c, const uintA& d)  {  mlr::Array<T> z;  z.resize(d);  z.setUni(c);  return z; }
/// return VECTOR of c's
template<class T> mlr::Array<T> consts(const T& c, uint n) { return consts(c, TUP(n)); }
/// return matrix of c's
template<class T> mlr::Array<T> consts(const T& c, uint d0, uint d1) { return consts(c, TUP(d0, d1)); }
/// return tensor of c's
template<class T> mlr::Array<T> consts(const T& c, uint d0, uint d1, uint d2) { return consts(c, TUP(d0, d1, d2)); }

/// return array with random numbers in [0, 1]
arr rand(const uintA& d);
/// return array with random numbers in [0, 1]
inline arr rand(uint n) { return rand(TUP(n)); }
/// return array with random numbers in [0, 1]
inline arr rand(uint d0, uint d1) { return rand(TUP(d0, d1)); }

/// return array with normal (Gaussian) random numbers
arr randn(const uintA& d);
/// return array with normal (Gaussian) random numbers
inline arr randn(uint n) { return randn(TUP(n)); }
/// return array with normal (Gaussian) random numbers
inline arr randn(uint d0, uint d1) { return randn(TUP(d0, d1)); }

/// return a grid with different lo/hi/steps in each dimension
arr grid(const arr& lo, const arr& hi, const uintA& steps);
/// return a grid (1D: range) split in 'steps' steps
inline arr grid(uint dim, double lo, double hi, uint steps) { arr g;  g.setGrid(dim, lo, hi, steps);  return g; }

arr repmat(const arr& A, uint m, uint n);

//inline double max(const arr& x) { return x.max(); }
//inline double min(const arr& x) { return x.min(); }
inline uint argmax(const arr& x) { return x.maxIndex(); }
inline uint argmin(const arr& x) { return x.minIndex(); }

inline uintA randperm(uint n) {  uintA z;  z.setRandomPerm(n);  return z; }
inline arr linspace(double base, double limit, uint n) {  arr z;  z.setGrid(1, base, limit, n);  return z;  }
arr logspace(double base, double limit, uint n);


//===========================================================================
/// @}
/// @name non-template functions //? needs most cleaning
/// @{

arr diag(double d, uint n);
void makeSymmetric(arr& A);
void transpose(arr& A);
uintA sampleMultinomial_SUS(const arr& p, uint n);
uint sampleMultinomial(const arr& p);
arr bootstrap(const arr& x);
void addDiag(arr& A, double d);

namespace mlr {
/// use this to turn on Lapack routines [default true if MLR_LAPACK is defined]
extern bool useLapack;
}

uint svd(arr& U, arr& d, arr& V, const arr& A, bool sort=true);
void svd(arr& U, arr& V, const arr& A);
void pca(arr &Y, arr &v, arr &W, const arr &X, uint npc = 0);

arr  oneover(const arr& A); //element-wise reciprocal (devision, 1./A)

void mldivide(arr& X, const arr& A, const arr& b);

uint inverse(arr& Ainv, const arr& A);
arr  inverse(const arr& A);
uint inverse_SVD(arr& inverse, const arr& A);
void inverse_LU(arr& Xinv, const arr& X);
void inverse_SymPosDef(arr& Ainv, const arr& A);
inline arr inverse_SymPosDef(const arr& A){ arr Ainv; inverse_SymPosDef(Ainv, A); return Ainv; }
arr pseudoInverse(const arr& A, const arr& Winv=NoArr, double robustnessEps=1e-10);
void gaussFromData(arr& a, arr& A, const arr& X);
void rotationFromAtoB(arr& R, const arr& a, const arr& v);

double determinant(const arr& A);
double cofactor(const arr& A, uint i, uint j);

uintA getIndexTuple(uint i, const uintA &d);  //? that also exists inside of array!
void lognormScale(arr& P, double& logP, bool force=true);



void gnuplot(const arr& X, bool pauseMouse=false, bool persist=false, const char* PDFfile=NULL);
//these are obsolete, use catCol instead
void write(const arrL& X, const char *filename, const char *ELEMSEP=" ", const char *LINESEP="\n ", const char *BRACKETS="  ", bool dimTag=false, bool binary=false);


void write_ppm(const byteA &img, const char *file_name, bool swap_rows=true);
void read_ppm(byteA &img, const char *file_name, bool swap_rows=true);
void add_alpha_channel(byteA &img, byte alpha);
void make_grey(byteA &img);
void make_RGB(byteA &img);
void make_RGB2BGRA(byteA &img);
void swap_RGB_BGR(byteA &img);
void flip_image(byteA &img);
void flip_image(floatA &img);

void scanArrFile(const char* name);

arr finiteDifferenceGradient(const ScalarFunction& f, const arr& x, arr& Janalytic=NoArr);
bool checkGradient(const ScalarFunction& f, const arr& x, double tolerance, bool verbose=false);
bool checkHessian(const ScalarFunction& f, const arr& x, double tolerance, bool verbose=false);
bool checkJacobian(const VectorFunction& f, const arr& x, double tolerance, bool verbose=false);

double NNinv(const arr& a, const arr& b, const arr& Cinv);
double logNNprec(const arr& a, const arr& b, double prec);
double logNNinv(const arr& a, const arr& b, const arr& Cinv);
double NN(const arr& a, const arr& b, const arr& C);
double logNN(const arr& a, const arr& b, const arr& C);

/// non-normalized!! Gaussian function (f(0)=1)
double NNNNinv(const arr& a, const arr& b, const arr& Cinv);
double NNNN(const arr& a, const arr& b, const arr& C);
double NNzeroinv(const arr& x, const arr& Cinv);
/// gradient of a Gaussian
double dNNinv(const arr& x, const arr& a, const arr& Ainv, arr& grad);
/// gradient of a non-normalized Gaussian
double dNNNNinv(const arr& x, const arr& a, const arr& Ainv, arr& grad);
double NNsdv(const arr& a, const arr& b, double sdv);
double NNzerosdv(const arr& x, double sdv);

mlr::String singleString(const StringA& strs);

//===========================================================================
/// @}
/// @name template functions
/// @{

//NOTES:
// -- past-tense names do not modify the array, but return variants
// -- more methods should return an array instead of have a returned parameter...

template<class T> mlr::Array<T> vectorShaped(const mlr::Array<T>& x) {  mlr::Array<T> y;  y.referTo(x);  y.reshape(y.N);  return y;  }
template<class T> void transpose(mlr::Array<T>& x, const mlr::Array<T>& y);
template<class T> void negative(mlr::Array<T>& x, const mlr::Array<T>& y);
template<class T> mlr::Array<T> getDiag(const mlr::Array<T>& y);
template<class T> mlr::Array<T> diag(const mlr::Array<T>& x) {  mlr::Array<T> y;  y.setDiag(x);  return y;  }
template<class T> mlr::Array<T> skew(const mlr::Array<T>& x);
template<class T> void inverse2d(mlr::Array<T>& Ainv, const mlr::Array<T>& A);
template<class T> mlr::Array<T> replicate(const mlr::Array<T>& A, uint d0);
template<class T> mlr::Array<T> integral(const mlr::Array<T>& x);

template<class T> uintA size(const mlr::Array<T>& x) { return x.dim(); }
template<class T> void checkNan(const mlr::Array<T>& x);
template<class T> void sort(mlr::Array<T>& x);

template<class T> T entropy(const mlr::Array<T>& v);
template<class T> T normalizeDist(mlr::Array<T>& v);
template<class T> void makeConditional(mlr::Array<T>& P);
template<class T> void checkNormalization(mlr::Array<T>& v, double tol);
template<class T> void checkNormalization(mlr::Array<T>& v) { checkNormalization(v, 1e-10); }
template<class T> void eliminate(mlr::Array<T>& x, const mlr::Array<T>& y, uint d);
template<class T> void eliminate(mlr::Array<T>& x, const mlr::Array<T>& y, uint d, uint e);
template<class T> void eliminatePartial(mlr::Array<T>& x, const mlr::Array<T>& y, uint d);

#ifndef SWIG
template<class T> T sqrDistance(const mlr::Array<T>& v, const mlr::Array<T>& w);
template<class T> T maxDiff(const mlr::Array<T>& v, const mlr::Array<T>& w, uint *maxi=0);
template<class T> T maxRelDiff(const mlr::Array<T>& v, const mlr::Array<T>& w, T tol);
//template<class T> T sqrDistance(const mlr::Array<T>& v, const mlr::Array<T>& w, const mlr::Array<bool>& mask);
template<class T> T sqrDistance(const mlr::Array<T>& g, const mlr::Array<T>& v, const mlr::Array<T>& w);
template<class T> T euclideanDistance(const mlr::Array<T>& v, const mlr::Array<T>& w);
template<class T> T metricDistance(const mlr::Array<T>& g, const mlr::Array<T>& v, const mlr::Array<T>& w);

template<class T> T sum(const mlr::Array<T>& v);
template<class T> T scalar(const mlr::Array<T>& v);
template<class T> mlr::Array<T> sum(const mlr::Array<T>& v, uint d);
template<class T> T sumOfAbs(const mlr::Array<T>& v);
template<class T> T sumOfSqr(const mlr::Array<T>& v);
template<class T> T length(const mlr::Array<T>& v);
template<class T> T product(const mlr::Array<T>& v);
template<class T> T max(const mlr::Array<T>& v);
template<class T> mlr::Array<T> max(const mlr::Array<T>& v, uint d);

template<class T> T trace(const mlr::Array<T>& v);
template<class T> T var(const mlr::Array<T>& v);
template<class T> mlr::Array<T> mean(const mlr::Array<T>& v);
template<class T> mlr::Array<T> stdDev(const mlr::Array<T>& v);
template<class T> T minDiag(const mlr::Array<T>& v);
template<class T> T absMax(const mlr::Array<T>& x);
template<class T> T absMin(const mlr::Array<T>& x);
template<class T> void clip(const mlr::Array<T>& x, T lo, T hi);

template<class T> void innerProduct(mlr::Array<T>& x, const mlr::Array<T>& y, const mlr::Array<T>& z);
template<class T> void outerProduct(mlr::Array<T>& x, const mlr::Array<T>& y, const mlr::Array<T>& z);
template<class T> void indexWiseProduct(mlr::Array<T>& x, const mlr::Array<T>& y, const mlr::Array<T>& z);
template<class T> mlr::Array<T> crossProduct(const mlr::Array<T>& y, const mlr::Array<T>& z); //only for 3 x 3 or (3,n) x 3
template<class T> T scalarProduct(const mlr::Array<T>& v, const mlr::Array<T>& w);
template<class T> T scalarProduct(const mlr::Array<T>& g, const mlr::Array<T>& v, const mlr::Array<T>& w);
template<class T> mlr::Array<T> diagProduct(const mlr::Array<T>& v, const mlr::Array<T>& w);

template<class T> mlr::Array<T> elemWiseMin(const mlr::Array<T>& v, const mlr::Array<T>& w);
template<class T> mlr::Array<T> elemWiseMax(const mlr::Array<T>& v, const mlr::Array<T>& w);
template<class T> mlr::Array<T> elemWisemax(const mlr::Array<T>& x,const T& y);
template<class T> mlr::Array<T> elemWisemax(const T& x,const mlr::Array<T>& y);


//===========================================================================
/// @}
/// @name concatenating arrays together
/// @{

template<class T> mlr::Array<T> cat(const mlr::Array<T>& y, const mlr::Array<T>& z) { mlr::Array<T> x(y); x.append(z); return x; }
template<class T> mlr::Array<T> cat(const mlr::Array<T>& y, const mlr::Array<T>& z, const mlr::Array<T>& w) { mlr::Array<T> x; x.append(y); x.append(z); x.append(w); return x; }
template<class T> mlr::Array<T> cat(const mlr::Array<T>& a, const mlr::Array<T>& b, const mlr::Array<T>& c, const mlr::Array<T>& d) { mlr::Array<T> x; x.append(a); x.append(b); x.append(c); x.append(d); return x; }
template<class T> mlr::Array<T> cat(const mlr::Array<T>& a, const mlr::Array<T>& b, const mlr::Array<T>& c, const mlr::Array<T>& d, const mlr::Array<T>& e) { mlr::Array<T> x; x.append(a); x.append(b); x.append(c); x.append(d); x.append(e); return x; }
template<class T> mlr::Array<T> catCol(const mlr::Array<mlr::Array<T>*>& X);
template<class T> mlr::Array<T> catCol(const mlr::Array<mlr::Array<T> >& X);
template<class T> mlr::Array<T> catCol(const mlr::Array<T>& a, const mlr::Array<T>& b){ return catCol(LIST<mlr::Array<T> >(a,b)); }
template<class T> mlr::Array<T> catCol(const mlr::Array<T>& a, const mlr::Array<T>& b, const mlr::Array<T>& c){ return catCol(LIST<mlr::Array<T> >(a,b,c)); }
template<class T> mlr::Array<T> catCol(const mlr::Array<T>& a, const mlr::Array<T>& b, const mlr::Array<T>& c, const mlr::Array<T>& d){ return catCol(LIST<mlr::Array<T> >(a,b,c,d)); }
template<class T> mlr::Array<T> catCol(const mlr::Array<T>& a, const mlr::Array<T>& b, const mlr::Array<T>& c, const mlr::Array<T>& d, const mlr::Array<T>& e){ return catCol(LIST<mlr::Array<T> >(a,b,c,d,e)); }


//===========================================================================
/// @}
/// @name arrays interpreted as a set
/// @{

template<class T> void setUnion(mlr::Array<T>& x, const mlr::Array<T>& y, const mlr::Array<T>& z);
template<class T> void setSection(mlr::Array<T>& x, const mlr::Array<T>& y, const mlr::Array<T>& z);
template<class T> mlr::Array<T> setUnion(const mlr::Array<T>& y, const mlr::Array<T>& z) { mlr::Array<T> x; setUnion(x, y, z); return x; }
template<class T> mlr::Array<T> setSection(const mlr::Array<T>& y, const mlr::Array<T>& z) { mlr::Array<T> x; setSection(x, y, z); return x; }
template<class T> mlr::Array<T> setSectionSorted(const mlr::Array<T>& x, const mlr::Array<T>& y,
                                                bool (*comp)(const T& a, const T& b) );
template<class T> void setMinus(mlr::Array<T>& x, const mlr::Array<T>& y);
template<class T> void setMinusSorted(mlr::Array<T>& x, const mlr::Array<T>& y,
                                      bool (*comp)(const T& a, const T& b) );
template<class T> uint numberSharedElements(const mlr::Array<T>& x, const mlr::Array<T>& y);
template<class T> void rndInteger(mlr::Array<T>& a, int low=0, int high=1, bool add=false);
template<class T> void rndUniform(mlr::Array<T>& a, double low=0., double high=1., bool add=false);
template<class T> void rndNegLogUniform(mlr::Array<T>& a, double low=0., double high=1., bool add=false);
template<class T> void rndGauss(mlr::Array<T>& a, double stdDev=1., bool add=false);
//template<class T> void rndGauss(mlr::Array<T>& a, bool add=false);
//template<class T> mlr::Array<T>& rndGauss(double stdDev, uint dim);
template<class T> uint softMax(const mlr::Array<T>& a, arr& soft, double beta);
template<class T> mlr::Array<T> sqr(const mlr::Array<T>& y) { mlr::Array<T> x; x.resizeAs(y); for(uint i=0; i<x.N; i++) x.elem(i)=y.elem(i)*y.elem(i); return x; }


//===========================================================================
/// @}
/// @name tensor functions
/// @{

template<class T> void tensorCondNormalize(mlr::Array<T> &X, int left);
template<class T> void tensorCondMax(mlr::Array<T> &X, uint left);
template<class T> void tensorCondSoftMax(mlr::Array<T> &X, uint left, double beta);
template<class T> void tensorCond11Rule(mlr::Array<T>& X, uint left, double rate);
template<class T> void tensorCheckCondNormalization(const mlr::Array<T> &X, uint left, double tol=1e-10);
template<class T> void tensorCheckCondNormalization_with_logP(const mlr::Array<T> &X, uint left, double logP, double tol=1e-10);

template<class T> void tensorEquation(mlr::Array<T> &X, const mlr::Array<T> &A, const uintA &pickA, const mlr::Array<T> &B, const uintA &pickB, uint sum=0);
template<class T> void tensorPermutation(mlr::Array<T> &Y, const mlr::Array<T> &X, const uintA &Yid);
template<class T> void tensorMarginal(mlr::Array<T> &Y, const mlr::Array<T> &X, const uintA &Yid);
template<class T> void tensorMaxMarginal(mlr::Array<T> &Y, const mlr::Array<T> &X, const uintA &Yid);
template<class T> void tensorMarginal_old(mlr::Array<T> &y, const mlr::Array<T> &x, const uintA &xd, const uintA &ids);
template<class T> void tensorMultiply(mlr::Array<T> &X, const mlr::Array<T> &Y, const uintA &Yid);
template<class T> void tensorAdd(mlr::Array<T> &X, const mlr::Array<T> &Y, const uintA &Yid);
template<class T> void tensorMultiply_old(mlr::Array<T> &x, const mlr::Array<T> &y, const uintA &d, const uintA &ids);
template<class T> void tensorDivide(mlr::Array<T> &X, const mlr::Array<T> &Y, const uintA &Yid);


//===========================================================================
/// @}
/// @name twice template functions
/// @{

#ifndef SWIG
template<class T, class S> void resizeAs(mlr::Array<T>& x, const mlr::Array<S>& a) {
  x.nd=a.nd; x.d0=a.d0; x.d1=a.d1; x.d2=a.d2;
  x.resetD();
  if(x.nd>3) { x.d=new uint[x.nd];  memmove(x.d, a.d, x.nd*sizeof(uint)); }
  x.resizeMEM(a.N, false);
}
template<class T, class S> void resizeCopyAs(mlr::Array<T>& x, const mlr::Array<S>& a);
template<class T, class S> void reshapeAs(mlr::Array<T>& x, const mlr::Array<S>& a);
template<class T, class S> void copy(mlr::Array<T>& x, const mlr::Array<S>& a) {
  resizeAs(x, a);
  for(uint i=0; i<x.N; i++) x.elem(i)=(T)a.elem(i);
}
template<class T, class S> mlr::Array<T> convert(const mlr::Array<S>& a) {
  mlr::Array<T> x;
  copy<T,S>(x,a);
  return x;
}
/// check whether this and \c a have same dimensions
template<class T, class S>
bool samedim(const mlr::Array<T>& a, const mlr::Array<S>& b) {
  return (b.nd==a.nd && b.d0==a.d0 && b.d1==a.d1 && b.d2==a.d2);
}
#endif //SWIG


//===========================================================================
/// @}
/// @name low-level lapack interfaces
/// @{

void blas_Mv(arr& y, const arr& A, const arr& x);
void blas_MM(arr& X, const arr& A, const arr& B);
void blas_MsymMsym(arr& X, const arr& A, const arr& B);
void blas_A_At(arr& X, const arr& A);
void blas_At_A(arr& X, const arr& A);

void lapack_cholesky(arr& C, const arr& A);
uint lapack_SVD(arr& U, arr& d, arr& Vt, const arr& A);
void lapack_mldivide(arr& X, const arr& A, const arr& B);
void lapack_LU(arr& LU, const arr& A);
void lapack_RQ(arr& R, arr& Q, const arr& A);
void lapack_EigenDecomp(const arr& symmA, arr& Evals, arr& Evecs);
arr  lapack_kSmallestEigenValues_sym(const arr& A, uint k);
bool lapack_isPositiveSemiDefinite(const arr& symmA);
void lapack_inverseSymPosDef(arr& Ainv, const arr& A);
void lapack_choleskySymPosDef(arr& Achol, const arr& A);
double lapack_determinantSymPosDef(const arr& A);
inline arr lapack_inverseSymPosDef(const arr& A){ arr Ainv; lapack_inverseSymPosDef(Ainv, A); return Ainv; }
arr lapack_Ainv_b_sym(const arr& A, const arr& b);
void lapack_min_Ax_b(arr& x,const arr& A, const arr& b);
arr lapack_Ainv_b_symPosDef_givenCholesky(const arr& U, const arr&b);
arr lapack_Ainv_b_triangular(const arr& L, const arr& b);


//===========================================================================
/// @}
/// @name special matrices & packings
/// @{

arr unpack(const arr& X);
arr comp_At_A(const arr& A);
arr comp_A_At(const arr& A);
arr comp_At_x(const arr& A, const arr& x);
arr comp_At(const arr& A);
arr comp_A_x(const arr& A, const arr& x);

struct SpecialArray{
  enum Type { ST_none, hasCarrayST, sparseVectorST, sparseMatrixST, diagST, RowShiftedST, CpointerST };
  Type type;
  virtual ~SpecialArray(){}
};

template<class T> bool isNotSpecial(const mlr::Array<T>& X){ return !X.special || X.special->type==SpecialArray::ST_none; }
template<class T> bool isRowShifted(const mlr::Array<T>& X){ return X.special && X.special->type==SpecialArray::RowShiftedST; }
template<class T> bool isSparseMatrix(const mlr::Array<T>& X){ return X.special && X.special->type==SpecialArray::sparseMatrixST; }
template<class T> bool isSparseVector(const mlr::Array<T>& X){ return X.special && X.special->type==SpecialArray::sparseVectorST; }

struct RowShifted : SpecialArray {
  arr& Z;           ///< references the array itself
  uint real_d1;     ///< the real width (the packed width is Z.d1; the height is Z.d0)
  uintA rowShift;   ///< amount of shift of each row (rowShift.N==Z.d0)
  uintA rowLen;     ///< number of non-zeros in the row
  uintA colPatches; ///< column-patch: (nd=2,d0=real_d1,d1=2) range of non-zeros in a COLUMN; starts with 'a', ends with 'b'-1
  bool symmetric;   ///< flag: if true, this stores a symmetric (banded) matrix: only the upper triangle
  
  RowShifted(arr& X);
  RowShifted(arr& X, RowShifted &aux);
  ~RowShifted();
  double elem(uint i, uint j); //TODO rename to 'elem'
  void reshift(); //shift all cols to start with non-zeros
  void computeColPatches(bool assumeMonotonic);
  arr At_A();
  arr A_At();
  arr At_x(const arr& x);
  arr A_x(const arr& x);
  arr At();
};

inline RowShifted* castRowShifted(arr& X) {
  ///CHECK_EQ(X.special,X.RowShiftedST,"can't cast like this!");
  if(!X.special || X.special->type!=SpecialArray::RowShiftedST) throw("can't cast like this!");
  return dynamic_cast<RowShifted*>(X.special); //((RowShifted*)X.aux);
}

namespace mlr {

struct SparseVector: SpecialArray{
  uint N; ///< original size
  uintA elems; ///< for every non-zero (in memory order), the index
  template<class T> SparseVector(mlr::Array<T>& X);
};

struct SparseMatrix : SpecialArray{
  uintA elems; ///< for every non-zero (in memory order), the (row,col) index tuple [or only (row) for vectors]
  uintAA cols; ///< for every column, for every non-zero the (row,memory) index tuple [also for a vector column]
  uintAA rows; ///< for every row   , for every non-zero the (column,memory) index tuple [not for vectors]
  template<class T> SparseMatrix(mlr::Array<T>& X);
  template<class T> SparseMatrix(mlr::Array<T>& X, uint d0);
};

}//namespace mlr

//struct RowSparseMatrix : SpecialArray {
//  RowSparseMatrix()
//};

arr unpack(const arr& Z); //returns an unpacked matrix in case this is packed
arr packRowShifted(const arr& X);
RowShifted *makeRowShifted(arr& Z, uint d0, uint pack_d1, uint real_d1);
arr makeRowSparse(const arr& X);


//===========================================================================
/// @}
/// @name lists -- TODO: make lists 'special'
/// @{

/*  TODO: realize list simpler: let the Array class have a 'listMode' flag. When this flag is true, the read, write, resize, find etc routines
will simply be behave differently */

template<class T> char listWrite(const mlr::Array<std::shared_ptr<T> >& L, std::ostream& os=std::cout, const char *ELEMSEP=" ", const char *delim=NULL);
template<class T> char listWrite(const mlr::Array<T*>& L, std::ostream& os=std::cout, const char *ELEMSEP=" ", const char *delim=NULL);
template<class T> void listWriteNames(const mlr::Array<T*>& L, std::ostream& os);
template<class T> void listRead(mlr::Array<T*>& L, std::istream& is, const char *delim=NULL);
template<class T> void listCopy(mlr::Array<T*>& L, const mlr::Array<T*>& M);  //copy a list by calling the copy constructor for each element
template<class T> void listClone(mlr::Array<T*>& L, const mlr::Array<T*>& M); //copy a list by calling the 'newClone' method of each element (works for virtual types)
template<class T> void listDelete(mlr::Array<T*>& L);
template<class T> void listReindex(mlr::Array<T*>& L);
template<class T> T* listFindValue(const mlr::Array<T*>& L, const T& x);
template<class T> T* listFindByName(const mlr::Array<T*>& L, const char* name); //each element needs a 'name' (usually mlr::String)
template<class T> T* listFindByType(const mlr::Array<T*>& L, const char* type); //each element needs a 'type' (usually mlr::String)
template<class T, class LowerOperator> void listSort(mlr::Array<T*>& L, LowerOperator lowerop);

//TODO obsolete?
template<class T> mlr::Array<T*> getList(const mlr::Array<T>& A) {
  mlr::Array<T*> L;
  resizeAs(L, A);
  for(uint i=0; i<A.N; i++) L.elem(i) = &A.elem(i);
  return L;
}
template<class T> T* new_elem(mlr::Array<T*>& L) { T *e=new T; e->index=L.N; L.append(e); return e; }


//===========================================================================
/// @}
/// @name graphs -- TODO: transfer to graph data structure
/// @{

void graphRandomUndirected(uintA& E, uint n, double connectivity);
void graphRandomFixedDegree(uintA& E, uint N, uint degree);
void graphRandomTree(uintA& E, uint N, uint roots=1);

template<class vert, class edge> edge* graphGetEdge(vert *from, vert *to);
template<class vert, class edge> void graphMakeLists(mlr::Array<vert*>& V, mlr::Array<edge*>& E);
template<class vert, class edge> void graphRandomUndirected(mlr::Array<vert*>& V, mlr::Array<edge*>& E, uint N, double connectivity);
template<class vert, class edge> void graphRandomFixedDegree(mlr::Array<vert*>& V, mlr::Array<edge*>& E, uint N, uint degree);
template<class vert, class edge> void graphRandomLinear(mlr::Array<vert*>& V, mlr::Array<edge*>& E, uint N);
template<class vert, class edge> void graphConnectUndirected(mlr::Array<vert*>& V, mlr::Array<edge*>& E);
template<class vert, class edge> void graphLayered(mlr::Array<vert*>& V, mlr::Array<edge*>& E, const uintA& layers, bool interConnections);
template<class vert, class edge> edge *newEdge(vert *a, vert *b, mlr::Array<edge*>& E);
template<class edge> edge *newEdge(uint a , uint b, mlr::Array<edge*>& E);
template<class vert, class edge> edge *del_edge(edge *e, mlr::Array<vert*>& V, mlr::Array<edge*>& E, bool remakeLists);
template<class vert, class edge> void graphWriteDirected(std::ostream& os, const mlr::Array<vert*>& V, const mlr::Array<edge*>& E);
template<class vert, class edge> void graphWriteUndirected(std::ostream& os, const mlr::Array<vert*>& V, const mlr::Array<edge*>& E);
template<class vert, class edge> bool graphTopsort(mlr::Array<vert*>& V, mlr::Array<edge*>& E);
template<class vert, class edge> void graphDelete(mlr::Array<vert*>& V, mlr::Array<edge*>& E);

/// @}


//===========================================================================
//
// conv with std::vector
//

#include <vector>

template<class T> mlr::Array<T> conv_stdvec2arr(const std::vector<T>& v){
  return mlr::Array<T>(&v.front(), v.size());
}

template<class T> std::vector<T> conv_arr2stdvec(const mlr::Array<T>& x){
  return std::vector<T>(x.begin(), x.end());
}

//===========================================================================
//
// conv with Eigen
//

#ifdef MT_EIGEN

#include <Eigen/Dense>

arr conv_eigen2arr(const Eigen::MatrixXd& in);
Eigen::MatrixXd conv_arr2eigen(const arr& in);

#endif

//===========================================================================
// implementations
//

void linkArray();

//#if defined MLR_IMPLEMENT_TEMPLATES | defined MLR_IMPLEMENTATION
#  include "array.tpp"
//#endif

#endif
#endif

// (note: http://www.informit.com/articles/article.aspx?p=31783&seqNum=2)

/// @} //end group
