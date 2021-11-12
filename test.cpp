#include "bits/stdc++.h"

using namespace std;

class Triangle{
    private:
        double a,b,c;
    public:
        Triangle() = delete; // default, 0 arg constructor
        double obwod(){
            return a + b + c;
        }
        constexpr Triangle(const double a, const double b, const double c) : a(a), b(b), c(c){};
        constexpr bool operator< (const Triangle t) const {
                if (this->a < t.a) return true;
                if (this->a > t.a) return false;
                if (this->b < t.b) return true;
                if (this->b > t.b) return false;
                if (this->c < t.c) return true;
                return false;
        }
};

class TriangleMultiSet : public multiset<Triangle>{
    public:
        size_type remove(const Triangle val){
            return this->erase(val);
        }
};

int main() {
    multiset<Triangle> m = multiset<Triangle>();
    Triangle t(1,2,3);
    Triangle ttt(0,0,0);
    m.insert(t); m.insert(t); m.insert(t); //3
    m.insert(ttt); m.insert(ttt); m.insert(ttt); m.insert(ttt); m.insert(ttt); //5
    cout << m.erase(t) << " " << m.erase(ttt) << endl;

    TriangleMultiSet set = TriangleMultiSet();
    set.insert(t);
    set.insert(ttt);
    set.insert(Triangle(6,9,0));
    cout << set.remove(t) << " " << set.remove(ttt) << endl;

    //TriangleMultiSet set2({Triangle(6,9,0), Triangle(6,9,0), Triangle(6,9,0)});
}

