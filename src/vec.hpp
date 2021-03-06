/* An implementation of a number of vector classes.
 * 
 * Nvector is an n-dimensional vector, allowing addition, subtraction,
 * multiplication by a scalar. You can have Nvectors of Nvectors.
 * Numvector requires numbers as elements, and allows for calculation of
 * a dot product, magnitude, and normalizing.
 * Vector is a 3D vector, and adds a cross-product, as well as accessors
 * for x, y, and z components by name.
 */

#ifndef VEC_H
#define VEC_H

#include <ostream>
#include <cmath>
using namespace std;

typedef unsigned int uint;

template <class T, unsigned int N>
class Array {
    protected:
        T vals[N];
    public:
        Array();
        Array(const Array &rhs);
        template <class U> Array(const Array<U, N> &rhs);
        Array(const T locs[N]);
        const T& get(const unsigned int n) const {return vals[n];}
        void set(const unsigned int n, const T a){vals[n]=a;}
        unsigned int len() const {return N;};
        
        T& operator[](const unsigned int i){return vals[i];};
        const T& operator[](const unsigned int i) const {return vals[i];};
        //~ template <class U> bool operator==(const Array<U,N> a) const{
            //~ for(uint i=0; i<N; i++) if(a[i] != get[i]) return false;
            //~ return true;};
        T* begin(){return vals;};
        T* end(){return vals + N;};
        ~Array(){};
        
        //~ template <class U, unsigned int M>
        //~ friend ostream& operator<<(ostream& out, const Nvector<U, M> v);
};

template <class T, unsigned int N>
class Nvector {
    protected:
        T vals[N];
    public:
        typedef T* iterator;
        Nvector();
        Nvector(const Nvector &rhs);
        template <class U> Nvector(const Nvector<U, N> &rhs);
        Nvector(const T locs[N]);
        const T& get(const unsigned int n) const {return vals[n];}
        void set(const unsigned int n, const T a){vals[n]=a;}
        unsigned int len() const {return N;};
        
        Nvector& operator+=(const Nvector &rhs);
        Nvector& operator-=(const Nvector &rhs);
        template <class U> Nvector& operator*=(const U rhs);
        template <class U> Nvector& operator/=(const U rhs);
        Nvector operator-() const;
        Nvector operator+(const Nvector &rhs) const {return Nvector(*this) += rhs;}
        Nvector operator-(const Nvector &rhs) const {return Nvector(*this) -= rhs;}
        T& operator[](const unsigned int i){return vals[i];};
        const T& operator[](const unsigned int i) const{return vals[i];};
        template <class U> Nvector operator*(const U rhs) const {return Nvector(*this) *= rhs;}
        template <class U> Nvector operator/(const U rhs) const {return Nvector(*this) /= rhs;}
        T* begin(){return vals;};
        T* end(){return vals + N;};
        ~Nvector(){};
        
        template <class U, unsigned int M>
        friend ostream& operator<<(ostream& out, const Nvector<U, M> v);
};

//~ typedef typename std::iterator_traits<>::value_type cont;
//~ typedef typename cont::const_iterator const_iterator;

template <class T, unsigned int N>
class Numvector : public Nvector<T, N> {
    public:
        inline Numvector() {for(unsigned int i=0; i<N; i++) Nvector<T,N>::vals[i]=0;}
        inline Numvector(const Nvector<T, N> &rhs) {
                    for(unsigned int i=0; i<N; i++) Nvector<T,N>::vals[i]=rhs.get(i);}
        inline Numvector(const T rhs[N]) {
                    for(unsigned int i=0; i<N; i++) Nvector<T,N>::vals[i]=rhs[i];}
        T dot (const Numvector &other) const;
        inline T sq() const {return dot(*this);};
        inline T mag() const {return sqrt(sq());};
        inline T distance(const Numvector &rhs) const;
        Numvector perpto(const Numvector &other) const;
        //returns the component of this perpendicular to other
        
        void normalize();
        Numvector norm() const;
        ~Numvector(){};
        
        template <class U, unsigned int M>
        friend ostream& operator<<(ostream& out, const Numvector<U, M> v);
};

template <class T>
class Vector3 : public Numvector<T, 3> {
    public:
        Vector3() {setx(0); sety(0); setz(0);}
        Vector3(const T a, const T b, const T c) {setx(a); sety(b); setz(c);}
        Vector3(const Numvector<T, 3> rhs) {setx(rhs.get(0)); sety(rhs.get(1)); setz(rhs.get(2));}
        Vector3(const Nvector<T, 3> rhs) {setx(rhs.get(0)); sety(rhs.get(1)); setz(rhs.get(2));}
        inline const T getx() const {return Nvector<T,3>::get(0);}
        inline const T gety() const {return Nvector<T,3>::get(1);}
        inline const T getz() const {return Nvector<T,3>::get(2);}
        inline double getxd() const {return double(Nvector<T,3>::get(0));}
        inline double getyd() const {return double(Nvector<T,3>::get(1));}
        inline double getzd() const {return double(Nvector<T,3>::get(2));}
        inline void setx(const T a){Nvector<T,3>::vals[0]=a;}
        inline void sety(const T b){Nvector<T,3>::vals[1]=b;}
        inline void setz(const T c){Nvector<T,3>::vals[2]=c;}
        inline void setxd(const double a){Nvector<T,3>::vals[0]=a;}
        inline void setyd(const double b){Nvector<T,3>::vals[1]=b;}
        inline void setzd(const double c){Nvector<T,3>::vals[2]=c;}
        inline void set(const T a, const T b, const T c){
            Nvector<T,3>::vals[0]=a; Nvector<T,3>::vals[1]=b; Nvector<T,3>::vals[2]=c;}
        inline Vector3 operator-() const{
            return Vector3(-getx(),-gety(),-getz());}
        inline Vector3 operator+(const Vector3 &rhs) const {
            return Vector3(getx()+rhs.getx(),gety()+rhs.gety(),getz()+rhs.getz());}
        inline Vector3 operator-(const Vector3 &rhs) const {
            return Vector3(getx()-rhs.getx(),gety()-rhs.gety(),getz()-rhs.getz());}
        inline T operator*(const Vector3 &rhs) const {
            return (getx()*rhs.getx() + gety()*rhs.gety() + getz()*rhs.getz());}
        template <class U> inline Vector3 operator*(const U rhs) const {
            return Vector3(getx()*rhs,gety()*rhs,getz()*rhs);}
        template <class U> inline Vector3 operator/(const U rhs) const {
            return Vector3(getx()/rhs,gety()/rhs,getz()/rhs);}
        Vector3 cross (const Vector3 &rhs) const;
        inline Vector3 norm() const {return Vector3(Numvector<T,3>::norm());};
        inline Vector3& operator-=(const Vector3 &rhs){Nvector<T,3>::operator-=(rhs); return *this;};
        inline Vector3& operator+=(const Vector3 &rhs){Nvector<T,3>::operator+=(rhs); return *this;}; 
        template <class U> Vector3& operator*=(const U rhs);
        template <class U> Vector3& operator/=(const U rhs);
        
        static T angle(const Vector3 &dx1, const Vector3 &dx2){
            return acos(dx1.dot(dx2) / dx1.mag() / dx2.mag());
        }
        
        static T angle(const Vector3 &x1, const Vector3 &x2, const Vector3 &x3){
            Vector3 dx1 = x1 - x2, dx2 = x3 - x2;
            return acos(dx1.dot(dx2) / dx1.mag() / dx2.mag());
        }
        
        static T dihedral(const Vector3 &dx1, const Vector3 &dx2, const Vector3 &dx3){
            return atan2(dx1.dot(dx2.cross(dx3))*dx2.mag(), 
                                (dx1.cross(dx2).dot(dx2.cross(dx3))));
        }
        
        static T dihedral(const Vector3 &x1, const Vector3 &x2, 
                        const Vector3 &x3, const Vector3 &x4){
            Vector3 dx1 = x2 - x1, dx2 = x3 - x2, dx3 = x4 - x3;
            return atan2(dx1.dot(dx2.cross(dx3))*dx2.mag(), 
                                (dx1.cross(dx2).dot(dx2.cross(dx3))));
        }
        ~Vector3(){};
        
        template <class U>
        friend ostream& operator<<(ostream& out, const Vector3<U> v);

};

template <class T>
class Vector2 : public Numvector<T, 2> {
    public:
        Vector2() {setx(0); sety(0);}
        Vector2(const T a, const T b) {setx(a); sety(b);}
        Vector2(const Numvector<T, 2> rhs) {setx(rhs.get(0)); sety(rhs.get(1));}
        Vector2(const Nvector<T, 2> rhs) {setx(rhs.get(0)); sety(rhs.get(1));}
        inline const T getx() const {return Nvector<T,2>::get(0);}
        inline const T gety() const {return Nvector<T,2>::get(1);}
        inline double getxd() const {return (double) Nvector<T,2>::get(0);}
        inline double getyd() const {return (double) Nvector<T,2>::get(1);}
        inline void setx(const T a){Nvector<T,2>::vals[0]=a;}
        inline void sety(const T b){Nvector<T,2>::vals[1]=b;}
        inline void setxd(const double a){Nvector<T,2>::vals[0]= (T) a;}
        inline void setyd(const double b){Nvector<T,2>::vals[1]= (T) b;}
        inline void set(const T a, const T b){
            Nvector<T,2>::vals[0]=a; Nvector<T,2>::vals[1]=b;}
        inline Vector2 operator-() const{
            return Vector2(-getx(),-gety());}
        inline Vector2 operator+(const Vector2 &rhs) const {
            return Vector2(getx()+rhs.getx(),gety()+rhs.gety());}
        inline Vector2 operator-(const Vector2 &rhs) const {
            return Vector2(getx()-rhs.getx(),gety()-rhs.gety());}
        inline T operator*(const Vector2 &rhs) const {
            return (getx()*rhs.getx() + gety()*rhs.gety());}
        template <class U> inline Vector2 operator*(const U rhs) const {
            return Vector2(getx()*rhs,gety()*rhs);}
        template <class U> inline Vector2 operator/(const U rhs) const {
            return Vector2(getx()/rhs,gety()/rhs);}
        T cross (const Vector2 &rhs) const{return getx()*rhs.gety() - rhs.getx()*gety();};
        Vector2 cross (const T v) const{return Vector2(gety()*v, -getx()*v);};
        Vector2 perp() const{return Vector2(-gety(),getx());};
        inline Vector2 norm() const {return Vector2(Numvector<T,2>::norm());};
        inline Vector2& operator-=(const Vector2 &rhs){Nvector<T,2>::operator-=(rhs); return *this;};
        inline Vector2& operator+=(const Vector2 &rhs){Nvector<T,2>::operator+=(rhs); return *this;}; 
        template <class U> Vector2& operator*=(const U rhs);
        template <class U> Vector2& operator/=(const U rhs);
        
        Vector2 rotate(uint i);
        inline Vector2 flip(){return Vector2(gety(), getx());};
        inline Vector2 rotate_flip(uint i){
            if((i / 4) % 2 == 1) return flip().rotate(i%4);
            return rotate(i%4);
        };
        
        inline Vector2 rotate_flip_inv(uint i){
            Vector2 inv = rotate(4-(i%4));
            if((i / 4) % 2 == 0) return inv;
            return inv.flip();
        };
        
        static T angle(const Vector2 &dx1, const Vector2 &dx2){
            return acos(dx1.dot(dx2) / dx1.mag() / dx2.mag());
        }
        
        static T angle(const Vector2 &x1, const Vector2 &x2, const Vector2 &x3){
            Vector2 dx1 = x1 - x2, dx2 = x3 - x2;
            return acos(dx1.dot(dx2) / dx1.mag() / dx2.mag());
        }
        ~Vector2(){};
        
        template <class U>
        friend ostream& operator<<(ostream& out, const Vector2<U> v);

};

//~ typedef float C;
template<class C>
class Matrix : public Nvector<Vector3<C>,3> {
    public:
        Vector3<C> dot(Vector3<C> v) const;
        inline Vector3<C> operator *(Vector3<C> v) const{return dot(v);};
        Matrix<C> SymmetricInverse() const;
        C det() const;
};

template<class C>
C Matrix<C>::det() const{
    const Matrix<C> &M = *this;
    return 2*M[0][1]*M[0][2]*M[1][2] + M[0][0]*M[1][1]*M[2][2]
              - M[0][2]*M[0][2]*M[1][1] - M[0][0]*M[1][2]*M[1][2]
               - M[0][1]*M[0][1]*M[2][2];
};

template<class C>
Matrix<C> Matrix<C>::SymmetricInverse() const{
    const Matrix<C> &M = *this;
    C d = det();
    Matrix<C> I;
    I[0][0] = (M[1][1]*M[2][2]-M[1][2]*M[1][2]) / d;
    I[0][1] = (M[0][2]*M[1][2]-M[0][1]*M[2][2]) / d;
    I[1][0] = I[0][1];
    I[0][2] = (M[0][1]*M[1][2]-M[0][2]*M[1][1]) / d;
    I[2][0] = I[0][2];
    I[1][1] = (M[0][0]*M[2][2]-M[0][2]*M[0][2]) / d;
    I[1][2] = (M[0][1]*M[0][2]-M[0][0]*M[1][2]) / d;
    I[2][1] = I[1][2];
    I[2][2] = (M[0][0]*M[1][1]-M[0][1]*M[0][1]) / d;
    return I;
};

template<class C>
Vector3<C> Matrix<C>::dot(Vector3<C> vec) const{
    return Vector3<C>(this->get(0).dot(vec),this->get(1).dot(vec),this->get(2).dot(vec));
}

template <class T, unsigned int N>
Array<T, N>::Array(const Array<T, N> &rhs) {
    for(unsigned int i=0; i < N; i++){
        vals[i] = rhs.get(i);
    }
}

template <class T, unsigned int N>
Array<T, N>::Array() {}

template <class T, unsigned int N>
Array<T, N>::Array(const T locs[N]) {
    for(unsigned int i=0; i < N; i++) vals[i] = locs[i];
}

template <class T, unsigned int N> template<class U>
Array<T, N>::Array(const Array<U,N> &rhs) {
    for(unsigned int i=0; i < N; i++){
        vals[i] = T(rhs.get(i));
    }
}

template <class T, unsigned int N>
Nvector<T, N>::Nvector(const Nvector<T, N> &rhs) {
    for(unsigned int i=0; i < N; i++){
        vals[i] = rhs.get(i);
    }
}

template <class T, unsigned int N>
Nvector<T, N>::Nvector() {}

template <class T, unsigned int N>
Nvector<T, N>::Nvector(const T locs[N]) {
    for(unsigned int i=0; i < N; i++) vals[i] = locs[i];
}

template <class T, unsigned int N> template<class U>
Nvector<T, N>::Nvector(const Nvector<U,N> &rhs) {
    for(unsigned int i=0; i < N; i++){
        vals[i] = T(rhs.get(i));
    }
}

template <class T, unsigned int N>
Nvector<T, N> Nvector<T, N>::operator-() const {
    Nvector<T, N> newvec = Nvector(*this);
    newvec *= -1;
    return newvec;
}

template <class T, unsigned int N>
Nvector<T, N>& Nvector<T, N>::operator-=(const Nvector<T,N> &rhs) {
    for(unsigned int i=0; i < N; i++){
        vals[i] -= rhs.get(i);
    }
    return *this;
}

template <class T, unsigned int N>
Nvector<T, N>& Nvector<T, N>::operator+=(const Nvector<T,N> &rhs) {
    for(unsigned int i=0; i < N; i++){
        vals[i] += rhs.get(i);
    }
    return *this;
}

template <class T, unsigned int N> template <class U>
Nvector<T,N>& Nvector<T,N>::operator*=(const U rhs) {
    for(unsigned int i=0; i<N; i++){
        vals[i] = vals[i] * rhs;
        //set(i, T(this->get(i) * rhs));
    }
    return *this;
}

template <class T, unsigned int N> template <class U>
Nvector<T,N>& Nvector<T,N>::operator/=(const U rhs) {
    for(unsigned int i=0; i<N; i++){
        vals[i] = T(vals[i] / rhs);
    }
    return *this;
}

template <class U, unsigned int M>
ostream& operator<< (ostream& out, const Nvector<U,M> &v){
    out << "[" << v.get(0);
    for(unsigned int i = 1; i < M; i++)
        out << ',' << v.get(i);
    return out << ']';
}

template <class T, unsigned int N>
T Numvector<T,N>::dot(const Numvector<T, N> &other) const{
    T m = 0;
    for(unsigned int i=0; i<N; i++){
        m += Nvector<T,N>::get(i) * other.get(i);
    }
    return m;
}

template <class T, unsigned int N>
Numvector<T,N> Numvector<T,N>::perpto(const Numvector<T,N>& other) const {
    //~ Numvector<T,N> parallel = other * (this->dot(other)) / other.dot(other);
    return (*this) - other * ((dot(other)) / other.sq());
    //~ return other - parallel;
}

template <class T, unsigned int N>
void Numvector<T,N>::normalize(){
    T m = mag();
    *this /= m;
}

template <class T, unsigned int N>
Numvector<T,N> Numvector<T,N>::norm() const{
    T m = mag();
    return *this / m;
}


template <class T, unsigned int N>
inline T Numvector<T,N>::distance(const Numvector<T,N> &rhs) const{
    T sum = 0;
    for(unsigned int i=0; i<N; i++){
        sum += pow(Nvector<T,N>::get(i) - rhs.get(i), 2);
    }
    return sqrt(sum);
    // return Numvector<T,N>(*this - rhs).mag();
}

template<>
inline double Numvector<double,2>::distance(const Numvector<double,2> &rhs) const{
    double x=get(0) - rhs.get(0), y=get(1) - rhs.get(1);
    return hypot(x,y);
    // return Numvector<T,N>(*this - rhs).mag();
}

template<>
inline long double Numvector<long double,2>::distance(const Numvector<long double,2> &rhs) const{
    long double x=get(0) - rhs.get(0), y=get(1) - rhs.get(1);
    return hypotl(x,y);
    // return Numvector<T,N>(*this - rhs).mag();
}

template<>
inline double Numvector<double,3>::distance(const Numvector<double,3> &rhs) const{
    double x=get(0)-rhs.get(0), y=get(1)-rhs.get(1), z=get(2)-rhs.get(2);
    return sqrt(x*x + y*y + z*z);
    // return Numvector<T,N>(*this - rhs).mag();
}

template<>
inline long double Numvector<long double,3>::distance(const Numvector<long double,3> &rhs) const{
    long double x=get(0)-rhs.get(0), y=get(1)-rhs.get(1), z=get(2)-rhs.get(2);
    return sqrtl(x*x + y*y + z*z);
    // return Numvector<T,N>(*this - rhs).mag();
}

template<>
inline double Numvector<double,2>::mag() const{
    double x=get(0), y=get(1);
    return hypot(x,y);
    // return Numvector<T,N>(*this - rhs).mag();
}

template<>
inline long double Numvector<long double,2>::mag() const{
    long double x=get(0), y=get(1);
    return hypotl(x,y);
    // return Numvector<T,N>(*this - rhs).mag();
}

template<>
inline double Numvector<double,3>::mag() const{
    double x=get(0), y=get(1), z=get(2);
    return sqrt(x*x + y*y + z*z);
    // return Numvector<T,N>(*this - rhs).mag();
}

template<>
inline long double Numvector<long double,3>::mag() const{
    long double x=get(0), y=get(1), z=get(2);
    return sqrtl(x*x + y*y + z*z);
    // return Numvector<T,N>(*this - rhs).mag();
}

template <class U, unsigned int M>
ostream& operator<< (ostream& out, const Numvector<U,M> &v){
    out << "[" << v.get(0);
    for(int i = 1; i < M; i++)
        out << ',' << v.get(i);
    return out << ']';
}

template <class T>
Vector3<T> Vector3<T>::cross(const Vector3<T> &rhs) const {
    T newx = gety()*rhs.getz() - rhs.gety()*getz();
    T newy = getz()*rhs.getx() - rhs.getz()*getx();
    T newz = getx()*rhs.gety() - rhs.getx()*gety();
    return Vector3<T>(newx, newy, newz);
}

template <class T> template <class U>
Vector3<T>& Vector3<T>::operator*=(const U rhs){
    Nvector<T,3>::vals[0] *= rhs;
    Nvector<T,3>::vals[1] *= rhs;
    Nvector<T,3>::vals[2] *= rhs;
    return *this;
}

template <class T> template <class U>
Vector3<T>& Vector3<T>::operator/=(const U rhs){
    Nvector<T,3>::vals[0] /= rhs;
    Nvector<T,3>::vals[1] /= rhs;
    Nvector<T,3>::vals[2] /= rhs;
    return *this;
}

template <class U>
ostream& operator<< (ostream& out, const Vector3<U> v){
    out << "{" << v.get(0);
    for(int i = 1; i < 3; i++)
        out << ',' << v.get(i);
    return out << '}';
}

template <class T> template <class U>
Vector2<T>& Vector2<T>::operator*=(const U rhs){
    Nvector<T,2>::vals[0] *= rhs;
    Nvector<T,2>::vals[1] *= rhs;
    return *this;
}

template <class T> template <class U>
Vector2<T>& Vector2<T>::operator/=(const U rhs){
    Nvector<T,2>::vals[0] /= rhs;
    Nvector<T,2>::vals[1] /= rhs;
    return *this;
}

template <class T>
Vector2<T> Vector2<T>::rotate(uint i){
    if(i % 4 == 0) return (*this);
    else if(i % 4 == 3) return Vector2(gety(), -getx());
    else if(i % 4 == 2) return Vector2(-getx(), -gety());
    else return Vector2(-gety(), getx());
}

template <class U>
ostream& operator<< (ostream& out, const Vector2<U> v){
    out << "{" << v.get(0);
    for(int i = 1; i < 2; i++)
        out << ',' << v.get(i);
    return out << '}';
}

#endif
