/*
 * Compile and run with:
 *
 * g++ -Wall -Wextra -std=c++11 ex1b.cpp && ./a.out
 *
 */

#include <iostream>
#include <map>
#include <list>

#include "TeuchosCore_ConfigDefs.hpp"
#include "Teuchos_UnitTestHarness.hpp"
#include "Teuchos_Ptr.hpp"
#include "Teuchos_UniquePtr.hpp"
#include "Teuchos_getConst.hpp"

using Teuchos::Ptr;
using Teuchos::UniquePtr;
using Teuchos::ptrFromRef;

#define DEBUG_MODE

template<class T>
struct D { // deleter
  D() {};
  D(const D&) { }
  D(D&) { }
  D(D&&) { }
  void operator()(T *p) const { };
};

#ifdef DEBUG_MODE
#    define DECLARE_P(x) UniquePtr<decltype(x), D<decltype(x)>> x ## aux_pointer;
#    define PTRFROMREF(x) x ## aux_pointer.ptr()
#    define INIT_P(x) x ## aux_pointer(&x, D<decltype(x)>())
#else
#    define DECLARE_P(x)
#    define PTRFROMREF(x) ptrFromRef(x)
#    define INIT_P(x)
#endif


class A {
private:
    std::map<int, int> m;
    DECLARE_P(m)
    std::list<int> l;
    DECLARE_P(l)
public:
    A()
#ifdef DEBUG_MODE
      : INIT_P(m), INIT_P(l)
#endif
      { }
    void add(int a, int b) {
        m[a] = b;
    }
    void add2(int a) {
        l.push_back(a);
    }
    Ptr<std::map<int, int>> get_access() {
        return PTRFROMREF(m);
    }
    Ptr<std::list<int>> get_access2() {
        return PTRFROMREF(l);
    }
};

template<class T>
inline std::ostream& print_map(std::ostream& out, T& d)
{
    out << "{";
    for (auto p = d.begin(); p != d.end(); p++) {
        if (p != d.begin()) out << ", ";
        out << (p->first) << ": " << (p->second);
    }
    out << "}";
    return out;
}

std::ostream& operator<<(std::ostream& out, const std::map<int, int> &d)
{
    return print_map(out, d);
}

int main() {
    Ptr<std::map<int, int>> ap;
    {
        A a;
        a.add(5, 6);
        a.add(6, 7);
        ap = a.get_access();
        std::cout << "a = " << *ap << std::endl; // OK
    }
    std::cout << "a = " << *ap << std::endl; // Dangling
    return 0;
}
