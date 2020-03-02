//
//  main.cpp
//  ADS_set
//
//  Created by Макс Боги on 04/05/18.
//  Copyright (c) 2018 Maxim B. All rights reserved.
//

#include <iostream>
#include <functional>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <array>
//#include"ADS_set.h"
//#include"Header.h"
#include"itADS_set.h"

using namespace std;




class Person {
    std::string vn;
    std::string nn;
public:
    Person() = default;
    Person(const std::string& vn, const std::string& nn) : vn(vn), nn(nn) { }
    friend struct std::hash<Person>;
    friend struct std::equal_to<Person>;
    friend struct std::less<Person>;
    friend std::ostream& operator<<(std::ostream& o, const Person& p) { return o << '[' << p.nn << ", " << p.vn << ']'; }
    friend std::istream& operator>>(std::istream& i, Person& p) { return i >> p.vn >> p.nn; }
    
};


namespace std {
    template <> struct hash<Person> {
        size_t operator()(const Person& p) const {
            return std::hash<std::string>{}(p.vn) ^ std::hash<std::string>{}(p.nn) << 1;
        }
    };
    template <> struct equal_to<Person> {
        bool operator()(const Person& lhs, const Person& rhs) const {
            return lhs.vn == rhs.vn && lhs.nn == rhs.nn;
        }
    };
    template <> struct less<Person> {
        bool operator()(const Person& lhs, const Person& rhs) const {
            return lhs.nn < rhs.nn || (lhs.nn == rhs.nn && lhs.vn < rhs.vn);
        }
    };
}

int main() {
    Person p1{"1","b"};
    Person p2{"2","b"};
    Person p3{"3","b"};
    Person p4{"4","b"};
    Person p5{"5","b"};
    Person p6{"6","b"};
    Person p7{"7","b"};
    Person p8{"8","b"};
    Person p9{"9","b"};
    Person p10{"10","b"};
     Person p11{"11","b"};
     Person p12{"12","b"};
     Person p13{"13","b"};
    
    ADS_set<Person> a{};
    array<Person,9> vec{p1,p2,p3,p4,p5,p6,p7,p8,p9};
   
    a.insert(p10);
    a.insert(vec.begin(), vec.end());
    a.insert(p11);
    a.insert(p12);
    a.insert(p13);
    a.dump(cerr);
      cout << "Size: " << a.size() << '\n';
    // cout << *(a.begin()) << '\n';
    //ADS_set<Person, 3>::iterator l{a.begin()};
   // ++l;
     //cout << *l << '\n';
    
    // BBBBB
    ADS_set<string,4> b;
    
    cout << "\n\n\n\n" << "B out: "  << "\n";
    b.insert("128");
    b.insert("256");
    b.insert("512");
    b.insert("1");
    b.insert("0");
    b.insert("10");
    b.insert("11");
    b.insert("12");

    b.dump(cerr);
    cout << "Size: " << b.size() << '\n';
    cout << "Begin b: " << *(b.begin()) << '\n';
    cout << "Begin b: " << *(++b.begin()) << '\n';
   
    
    
    ADS_set<string,4> c;
    c.insert("ab"); 
    c.insert("cd");
    c.insert("ef");
    c.insert("gh");

    cout << "\n\n\n\n" << "C out: "  << "\n";
    c.dump(cerr);
    cout << "Size: " << c.size() << '\n';
    
    
    
    
    
    c=b;
    
    c.erase("0");
    cout << "\n\n\n\n" << "C out: "  << "\n";
    c.dump(cerr);
    cout << "Size: " << c.size() << '\n';
    
    cout << "\n" << "B out: "  << "\n";
    b.dump(cerr);
    cout << "Size: " << b.size() << '\n';
    
    
    c.insert("111");
    c.insert("3000");
    cout << "\n" << "C out: "  << "\n";
    c.dump(cerr);
    cout << "Size: " << c.size() << '\n';
    
    bool c_b{c==b};
    cout << "c==b: " << c_b << '\n';
    
    c.erase("3000");
    c.erase("111");
    c.insert("0");
    cout << "Size C: " << c.size() << '\n';
    cout << "Size B: " << b.size() << '\n';
    bool c_b2{c==b};
    cout << "c==b: " << c_b2 << '\n';
    
    ADS_set<Person> setInt;
    for (int ins{0}; ins <10; ++ins){
        Person in{to_string(ins),"Nachname"};
        setInt.insert(in);
    }
    cout << "Size setInt: " << setInt.size() << '\n';
    setInt.erase({"99","Nachname"});
    setInt.insert({"99","Nachname"});
    setInt.insert({"55012f","Nachname"});
    cout << "Size setInt: " << setInt.size() << '\n';
    for (auto ele: setInt)
        if (++(setInt.find(ele)) == setInt.end()) cout << ele <<'\n';
   // ADS_set<Person>::iterator last{setInt.find({"50843","Nachname"})};
    //cout << "Last " << *last <<'\n';
    //last++;
    //cout << "Last " << *last <<'\n';
    //if (last == setInt.end()) cout << " last is end" << '\n';
    //auto s{setInt.begin()-setInt.end()};
    
    ADS_set<int> pls;
    for (int i{120}; i < 300000; ++i)
        pls.insert(i);
    
    cout << "size pls " << pls.size() << '\n';
    for (auto ele:pls)
        if (++(pls.find(ele)) == pls.end()) cout << ele <<'\n';
    ADS_set<int>::iterator last{pls.find(43263)};
    ++last;
    cout << "Last " << *last <<'\n';
    
    
    
    
    ADS_set<int> it;
    
    cout << "\n\n\n\n" << "it out: "  << "\n";
   // it.insert(128);
    it.insert(25);
    it.insert(1);
    it.insert(16);
    it.insert(7);
    it.insert(43);
    it.insert(89);
    std::pair<typename ADS_set<int>::iterator,bool>  m{(it.insert(32))};
    cout << *m.first << " iterator 32 " << '\n';

    
    it.dump(cerr);
    //z(it);
    cout << "Size: " << it.size() << '\n';
    
    for( auto i :it)
        cout << i << "  ";
    cout << '\n';
    cout << *it.begin() << '\n';
    if ( m.first != ++it.begin()) cout << "true"  <<  '\n';
    cout << *it.find(7) << " found " << '\n';

    cout << *it.w(2) << '\n';
    for( ADS_set<int>::iterator  i{it.w(2)}; i != it.end(); i++)
        cout << *i << "  ";
    
    //cout << " pos between: " << it.x(7,43) << '\n';
    //cout << " elem next: " << it.y(43,7) << '\n';

     cout << '\n';
    
    
    
    int* arr{new int[5]};
    int* arr2{new int[5]};
    
    *arr2 = 12;
    for (int i{0}; i < 5; ++i)
        *(arr+i) = i + 5;
    for (int i{0}; i < 5; ++i)
    cout << *(arr+i) << " :" << i << '\n';
    int* lastelm{arr+4};
    lastelm++;
    cout << *lastelm << " :last"  << '\n';
    cout << arr[5] << " :[]"  << '\n';
    
    
    
    
    cout << " z(c,b) : " << z(c,b) << '\n';
    
    
    
    return 0;
}
