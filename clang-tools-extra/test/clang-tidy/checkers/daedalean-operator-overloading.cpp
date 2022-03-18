// RUN: %check_clang_tidy %s daedalean-operator-overloading %t

class C {
public:
    bool operator &&(const C&) const;
    //  CHECK-MESSAGES: :[[@LINE-1]]:5: warning: overloading 'operator&&' is disallowed [daedalean-operator-overloading]
    bool operator ||(const C&) const;
    //  CHECK-MESSAGES: :[[@LINE-1]]:5: warning: overloading 'operator||' is disallowed [daedalean-operator-overloading]
    C* operator&();
    //  CHECK-MESSAGES: :[[@LINE-1]]:5: warning: overloading 'operator&' is disallowed [daedalean-operator-overloading]
    C operator &(const C&);
    int operator[](int);
    //  CHECK-MESSAGES: :[[@LINE-1]]:5: warning: If operator[] overloaded with non-const version, const version MUST be present. [daedalean-operator-overloading]

    bool operator==(const C&);
    //  CHECK-MESSAGES: :[[@LINE-1]]:5: warning: If == overloaded, != MUST be overloaded as well [daedalean-operator-overloading]
    bool operator!=(const int);
    //  CHECK-MESSAGES: :[[@LINE-1]]:5: warning: If != overloaded, == MUST be overloaded as well [daedalean-operator-overloading]

    bool operator==(float);
    bool operator!=(float);

    C & operator=(int a) {
      if (a == 1) {
        return *(new C);
        // CHECK-MESSAGES: :[[@LINE-1]]:9: warning: Assignment operator must return reference to this  [daedalean-operator-overloading]
      } else if (a == 2) {
        C * v = this;
        return *v;
        // CHECK-MESSAGES: :[[@LINE-1]]:9: warning: Assignment operator must return reference to this  [daedalean-operator-overloading]
      } else if (a == 3) {
        C &v = *this;
        return v;
        // CHECK-MESSAGES: :[[@LINE-1]]:9: warning: Assignment operator must return reference to this  [daedalean-operator-overloading]
      }
      return *this;
    }
};

class C2 {
    int operator[](int);
    int operator[](int) const;
    int operator[](int&);
    //  CHECK-MESSAGES: :[[@LINE-1]]:5: warning: If operator[] overloaded with non-const version, const version MUST be present. [daedalean-operator-overloading]
    bool operator==(char);
};

bool operator==(const C2&, const C&);
//  CHECK-MESSAGES: :[[@LINE-1]]:1: warning: If == overloaded, != MUST be overloaded as well [daedalean-operator-overloading]
bool operator!=(const C2&, const int);
//  CHECK-MESSAGES: :[[@LINE-1]]:1: warning: If != overloaded, == MUST be overloaded as well [daedalean-operator-overloading]

bool operator<(const C2&, const C&);
//  CHECK-MESSAGES: :[[@LINE-1]]:1: warning: If < overloaded, > MUST be overloaded as well [daedalean-operator-overloading]
bool operator>(const C2&, const int);
//  CHECK-MESSAGES: :[[@LINE-1]]:1: warning: If > overloaded, < MUST be overloaded as well [daedalean-operator-overloading]

bool operator<=(const C2&, const C&);
//  CHECK-MESSAGES: :[[@LINE-1]]:1: warning: If <= overloaded, >= MUST be overloaded as well [daedalean-operator-overloading]
bool operator>=(const C2&, const int);
//  CHECK-MESSAGES: :[[@LINE-1]]:1: warning: If >= overloaded, <= MUST be overloaded as well [daedalean-operator-overloading]
//  CHECK-MESSAGES: :[[@LINE-2]]:1: warning: If >= overloaded, == MUST be overloaded as well [daedalean-operator-overloading]

bool operator<=(const C2&, float);
//  CHECK-MESSAGES: :[[@LINE-1]]:1: warning: If <= overloaded, == MUST be overloaded as well [daedalean-operator-overloading]
bool operator>=(const C2&, float);
//  CHECK-MESSAGES: :[[@LINE-1]]:1: warning: If >= overloaded, == MUST be overloaded as well [daedalean-operator-overloading]

bool operator<(const C2&, double);
bool operator>(const C2&, double);
bool operator!=(const C2&, double);
bool operator==(const C2&, double);
//  CHECK-MESSAGES: :[[@LINE-1]]:1: warning: If == and < overloaded, <= MUST be overloaded as well [daedalean-operator-overloading]

bool operator!=(const C2&, char);

bool operator==(const C2&, const C2&);
bool operator!=(const C2&, const C2&);
bool operator<(const C2&, const C2&);
bool operator>(const C2&, const C2&);
bool operator<=(const C2&, const C2&);
bool operator>=(const C2&, const C2&);

bool operator &&(const C&, const C&);
//  CHECK-MESSAGES: :[[@LINE-1]]:1: warning: overloading 'operator&&' is disallowed [daedalean-operator-overloading]
bool operator ||(const C&, const C&);
//  CHECK-MESSAGES: :[[@LINE-1]]:1: warning: overloading 'operator||' is disallowed [daedalean-operator-overloading]
