/*
 * Compile and run with:
 *
 * g++ -Wall -Wextra -std=c++11 ex1b.cpp && ./a.out
 *
 */

#include <iostream>
#include <map>

#include "TeuchosCore_ConfigDefs.hpp"
#include "Teuchos_UnitTestHarness.hpp"
#include "Teuchos_Ptr.hpp"
#include "Teuchos_UniquePtr.hpp"
#include "Teuchos_getConst.hpp"

using Teuchos::Ptr;
using Teuchos::UniquePtr;
using Teuchos::ptrFromRef;

#define DEBUG_MODE

struct D { // deleter
  D() {};
  D(const D&) { }
  D(D&) { }
  D(D&&) { }
  void operator()(std::map<int, int> *p) const { };
};


class A {
private:
    std::map<int, int> m;
public:
    A()
#ifdef DEBUG_MODE
      : mp(&m, D())
#endif
      { }
    void add(int a, int b) {
        m[a] = b;
    }
    Ptr<std::map<int, int>> get_access() {
#ifdef DEBUG_MODE
        return mp.ptr();
#else
        return ptrFromRef(m);
#endif
    }


private:
// Only used in Debug mode
#ifdef DEBUG_MODE
    UniquePtr<decltype(m), D> mp;
#endif
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
