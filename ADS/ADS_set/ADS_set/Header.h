#ifndef ADS_SET_H
#define ADS_SET_H

#include <functional>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <array>
#include <cmath>



template <typename Key, size_t N=3>
class ADS_set {
    size_t sizeTable{2};
    size_t numbElem{0};
    
    
public:
    
    
    //                      USING
    class Iterator;
    using value_type = Key;
    using key_type = Key;
    using reference = key_type&;
    using const_reference = const key_type&;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using iterator = Iterator;
    using const_iterator = Iterator;
    using key_compare = std::less<key_type>;
    using key_equal = std::equal_to<key_type>;
    using hasher = std::hash<key_type>;
    
    
    
    
    
private:
    //                      BUCKET BUCKET BUCKET
    struct Bucket{
        size_type localDepth{1};
        size_type sizeBucket{0};
        size_type bucketTopIndex{0};
        
        value_type* bucket{new value_type[N]};
        
        void inBucket(const key_type& elem) { bucket[sizeBucket++]= elem;}
        void outBucket(const key_type& elem) {
            bool fromHere{false};
            for (size_t i=0; i< sizeBucket; ++i){
                if (key_equal{}(bucket[i],elem)) fromHere = true;
                if (fromHere && i!=N-1)
                    bucket[i]=bucket[i+1];
            };
            sizeBucket--;
        }
        bool findBucket(const key_type& elem) {
            for (size_t i=0; i< sizeBucket; ++i)
                if(key_equal{}(bucket[i],elem)) return true;
            return false;
        }
        void dumpBucket(std::ostream& o) {
            if (sizeBucket ==0) o << "-";
            else for (size_t i=0; i< sizeBucket; ++i)
                o << bucket[i] << ' ';
            // test
            //o << "top index: "<< bucketTopIndex << '\n';
            o << '\n';
        }
        size_t posBucket(const key_type& elem) {
            for (size_t i=0; i< sizeBucket; ++i)
                if(key_equal{}(bucket[i],elem)) return i;
            return N+1;
        }
        ~Bucket(){
            delete[] bucket;}
    };
    //                      BUCKET END
    
    
    
    Bucket* *table;   // Table
    
    
    //                      EXTRA EXTRA EXTRA
    void split(const size_t& idx);
    void grow();
    size_t topIndex(const size_t& idx) const;
    size_t globalDepth() const { return log2(sizeTable);};
    Bucket& useBucket(const size_t& idx) const { return *table[idx];};
    size_t findIndex(const Key& key) const { return std::hash<key_type>{}(key) % sizeTable;};
    void insertElemUn(const key_type& key);
    void insertElem(const key_type& key);
    bool findBool(const key_type& key) const;
    
public:
    //                      MAIN MAIN MAIN
    ADS_set();
    ADS_set(std::initializer_list<key_type> ilist);
    template<typename InputIt> ADS_set(InputIt first, InputIt last);
    ADS_set(const ADS_set& other);
    ~ADS_set();
    
    ADS_set& operator=(const ADS_set& other);
    ADS_set& operator=(std::initializer_list<key_type> ilist);
    
    size_type size() const  { return numbElem;}
    bool empty() const  {if (size() == 0) return true; return false;}
    
    size_type count(const key_type& key) const  {if (findBool(key)) return 1; return 0;}
    iterator find(const key_type& key ) const;
    
    
    void clear()  { ADS_set tmp; swap(tmp);}
    void swap(ADS_set&  other)  {
        std::swap(sizeTable, other.sizeTable);
        std::swap(table, other.table);
        std::swap(numbElem, other.numbElem);
    }
    
    void insert(std::initializer_list<key_type> ilist);
    std::pair<iterator,bool> insert(const key_type& key);
    template<typename InputIt> void insert(InputIt first, InputIt last);
    
    size_type erase(const key_type& );
    
    
    // begin() end()
    const_iterator begin() const  {
        for (size_t i{0}; i < sizeTable; ++i)
            if (i == useBucket(i).bucketTopIndex && useBucket(i).sizeBucket !=0)
                return const_iterator{table+i, 0, i, sizeTable};
        return const_iterator{table+sizeTable-1, N, sizeTable, sizeTable};
    }
    const_iterator end() const  {
        return const_iterator{table+sizeTable-1, N, sizeTable, sizeTable};
    }
    
    // 2 termin
    const_iterator w(const key_type &limit) const{
        for (size_t i{0}; i < sizeTable; ++i)
            if (i == table[i]->bucketTopIndex && table[i]->sizeBucket !=0)
                for (size_t l{0}; l < table[i]->sizeBucket; l++)
                    if (std::less<key_type>{}(limit,table[i]->bucket[l]))
                         return const_iterator{table+i, l, i, sizeTable, limit};
        return const_iterator{table+sizeTable-1, N, sizeTable, sizeTable};
    }
    
    
    void dump(std::ostream& o = std::cerr) const;
    
    friend bool operator==(const ADS_set& lhs, const ADS_set& rhs)  {
        if (lhs.numbElem != rhs.numbElem ) return false;
        for(auto i:rhs)
            if (!lhs.findBool(i)) return false;
        return true;
    }
    friend bool operator!=(const ADS_set& lhs, const ADS_set& rhs)  { return !(lhs == rhs);}
    
    
    
    friend bool z(const ADS_set& a, const ADS_set& b){
        if (a.numbElem==0 && b.numbElem==0) return true;
        if (b.numbElem < a.numbElem*0.9) return false;
        if (a.numbElem < b.numbElem*0.9) return false;
        size_t repeatElm{0};
        for (size_t i{0}; i < a.sizeTable; ++i)
            if (i == a.table[i]->bucketTopIndex && a.table[i]->sizeBucket !=0)
                for (size_t l{0}; l< a.table[i]->sizeBucket; l++)
                    if (b.count(a.table[i]->bucket[l])) repeatElm++;
        if (repeatElm > b.numbElem*0.9 && repeatElm > a.numbElem*0.9) return true;
        return false;
        
    }
    
    
    
    
    
};





template <typename Key, size_t N> void swap(ADS_set<Key,N>& lhs, ADS_set<Key,N>& rhs) { lhs.swap(rhs); }





//                      DECLARATION DECLARATION DECLARATION


//                      ITERATOR  ITERATOR ITERATOR
template <typename Key, size_t N>
class ADS_set<Key,N>::Iterator{
    Bucket* *buc;
    size_t posInBuc;
    size_t index;
    size_t size;
    // 2 termin
    key_type limit;
    bool normal{true};
    
    
public:
    using value_type =  Key;
    using difference_type = std::ptrdiff_t;
    using reference = const value_type&;
    using pointer = const value_type*;
    using iterator_category = std::forward_iterator_tag;
    using key_equal = std::equal_to<value_type>;
    using hasher = std::hash<value_type>;
    
    explicit Iterator(Bucket* *buc, size_t posInBuc, size_t index, size_t size ): buc{buc}, posInBuc{posInBuc}, index{index}, size{size} {}
    explicit Iterator(Bucket* *buc=nullptr): buc{buc} {}
    
    // 2 termin
    explicit Iterator(Bucket* *buc, size_t posInBuc, size_t index, size_t size, key_type limit): buc{buc}, posInBuc{posInBuc}, index{index}, size{size}, limit{limit} { normal=false;}
    
    
    reference operator*() const {return (*(*buc)).bucket[posInBuc];}
    pointer operator->() const  {return &((*(*buc)).bucket[posInBuc]);}
    
    Iterator& operator++() {
    if (normal){
        // if next key in the same bucket
        if ( posInBuc+1 < (*(*buc)).sizeBucket) {
            ++posInBuc;
            return *this;
        }
        // else search next bucket
        buc++;
        index++;
        if (index != size)
            while( (*(*buc)).bucketTopIndex != index ||  (*(*buc)).sizeBucket ==0){
                buc++;
                index++;
                if (index == size)
                    break;
            }
        posInBuc=0;
        if(index==size) {posInBuc=N, --buc;};
        return *this;
    }
     // 2 termin
        bool nextFind{false};
        while (!nextFind) {
            
        // if next key in the same bucket
        if ( posInBuc+1 < (*(*buc)).sizeBucket) {
            ++posInBuc;
            if (std::less<key_type>{}(limit,(*(*buc)).bucket[posInBuc])) nextFind=true;
        }
        else{
        // else search next bucket
        buc++;
        index++;
        if (index != size)
            while( (*(*buc)).bucketTopIndex != index ||  (*(*buc)).sizeBucket ==0){
                buc++;
                index++;
                if (index == size)
                    break;
            }
        posInBuc=0;
            
            if(index==size) {posInBuc=N, --buc, nextFind=true;}
            else {
                if (std::less<key_type>{}(limit,(*(*buc)).bucket[posInBuc])) nextFind=true;
            };
        }//else
        }//while
        return *this;
    };
    
    Iterator operator++(int) { Iterator old{*this}; ++*this; return old;}
    
    friend bool operator==(const Iterator& lhs, const Iterator& rhs)  {
        if (*(lhs.buc) == *(rhs.buc)) return lhs.posInBuc == rhs.posInBuc;
        return false;}
    friend bool operator!=(const Iterator& lhs, const Iterator& rhs) {
        if (*(lhs.buc) == *(rhs.buc)) return lhs.posInBuc != rhs.posInBuc;
        return true;}
};
//                      ITERATOR END


//                      constructors, destr

template <typename Key, size_t N>
ADS_set<Key,N>::ADS_set() {
    table = new Bucket*[sizeTable];
    table[0] = new Bucket;
    table[1] = new Bucket;
    useBucket(1).bucketTopIndex=1;
}

template <typename Key, size_t N>
ADS_set<Key,N>::ADS_set(std::initializer_list<key_type> ilist): ADS_set() {
    insert(ilist);
}

template <typename Key, size_t N>
template<typename InputIt> ADS_set<Key,N>::ADS_set(InputIt first, InputIt last): ADS_set() {
    insert(first,last);
}

template <typename Key, size_t N>
ADS_set<Key,N>::ADS_set(const ADS_set& other): ADS_set{}  {
    for (const auto i:other)
        insertElem(i);
        }

template <typename Key, size_t N>
ADS_set<Key,N>::~ADS_set()  {
    for (size_t i{sizeTable-1 }; i > 0; i--)
        if (i == useBucket(i).bucketTopIndex)
            delete table[i];
    delete table[0];
    delete[]  table;
}


//                      operator=

template <typename Key, size_t N>
ADS_set<Key,N>& ADS_set<Key,N>::operator=(const ADS_set& other)  {
    ADS_set tmp{other};
    swap(tmp);
    return *this;
}

template <typename Key, size_t N>
ADS_set<Key,N>& ADS_set<Key,N>::operator=(std::initializer_list<key_type> ilist)  {
    ADS_set tmp{ilist};
    swap(tmp);
    return *this;
}


//                      find

template <typename Key, size_t N>
typename ADS_set<Key,N>::iterator ADS_set<Key,N>::find(const key_type& key ) const  {
    size_t indexInTab{useBucket(findIndex(key)).bucketTopIndex};
    size_t posInBuc{useBucket(indexInTab).posBucket(key)};
    if (posInBuc < N) {
        return const_iterator{table+indexInTab, posInBuc, indexInTab, sizeTable};;
    };
    return end();
}


//                      insert

template <typename Key, size_t N>
void ADS_set<Key,N>::insert(std::initializer_list<key_type> ilist) {
    for(auto &i: ilist)
        insertElem(i);
}

template <typename Key, size_t N>
std::pair<typename ADS_set<Key,N>::iterator,bool> ADS_set<Key,N>::insert(const key_type& key){
    if (findBool(key)) return {find(key),false};
    insertElem(key);
    return {find(key),true};
}

template <typename Key, size_t N>
template<typename InputIt> void  ADS_set<Key,N>::insert(InputIt first, InputIt last) {
    for (auto &i = first; i != last; ++i) {
        insertElem(*i);
    }
}


//                      insert EXTRA

template <typename Key, size_t N>
void ADS_set<Key,N>::insertElemUn(const key_type& key) {
    while (useBucket(findIndex(key)).sizeBucket == N)
        split(findIndex(key));
    useBucket(findIndex(key)).inBucket(key);
}

template <typename Key, size_t N>
void ADS_set<Key,N>::insertElem(const key_type& key){
    if (!findBool(key)) {
        insertElemUn(key);
        ++numbElem;
    }
}


//                      erase

template <typename Key, size_t N>
size_t ADS_set<Key,N>::erase(const key_type& key) {
    if (findBool(key)){
        useBucket(findIndex(key)).outBucket(key);
        --numbElem;
        return 1;}
    return 0;
}


//                      dump

template <typename Key, size_t N>
void ADS_set<Key,N>::dump(std::ostream& o) const {
    for (size_t i{0}; i < sizeTable; ++i)
        //if (i == topIndex(i))               // don't repeat the same buckets
        useBucket(i).dumpBucket(o);
}




//                      EXTRA DECLARATION


//                      topIndex // delet this

template <typename Key, size_t N>
size_t ADS_set<Key,N>::topIndex(const size_t& idx) const {
    if (useBucket(idx).localDepth == globalDepth()) return idx;
    size_t numberOfPointers= pow(2,(globalDepth()-useBucket(idx).localDepth));
    size_t mod=(pow(2,globalDepth())) / numberOfPointers;
    size_t rest= idx % mod;
    return rest;
}


//                      split

template <typename Key, size_t N>
void ADS_set<Key,N>::split(const size_t& idx) {
    if (useBucket(idx).localDepth == globalDepth()) grow();
    size_t numberOfPointers= pow(2,(globalDepth()-useBucket(idx).localDepth));
    size_t mod=2* ((pow(2,globalDepth())) / numberOfPointers);
    size_t rest= idx % mod;
    size_t newDepth= ++(useBucket(idx).localDepth);
    Bucket* oldBucket= table[idx];
    table[idx] = new Bucket;
    useBucket(idx).localDepth = newDepth;
    (*oldBucket).localDepth = newDepth;
    for (size_t i=rest; i<sizeTable;) {
        table[i]=table[idx];
        i+=mod;
    };
    
    if (rest == (*oldBucket).bucketTopIndex)
        (*oldBucket).bucketTopIndex = rest+(mod/2);
    useBucket(idx).bucketTopIndex = rest;
    
    for (size_t i=0; i< (*oldBucket).sizeBucket;)
        if ( table[findIndex((*oldBucket).bucket[i])] != oldBucket) {
            insertElemUn((*oldBucket).bucket[i]);
            (*oldBucket).outBucket((*oldBucket).bucket[i]);
            
        }
        else ++i;
}


//                      grow

template <typename Key, size_t N>
void ADS_set<Key,N>::grow() {
    /*
     Bucket* *buf = new Bucket*[sizeTable*2];
     Bucket* *old = table;
     size_t oldSi =sizeTable;
     table= buf;
     sizeTable= oldSi*2;
     for (size_t i=0; i<oldSi; ++i)
     table[i]=old[i];
     size_t l{0};
     for (size_t i=oldSi; i<sizeTable; ++i)
     table[i]=old[l++];
     
     delete[] old;*/
    Bucket* *buf = new Bucket*[sizeTable*2];
    
    
    for (size_t i=0; i<sizeTable; ++i)
        buf[i]=table[i];
    size_t l{0};
    for (size_t i=sizeTable; i<sizeTable*2; ++i)
        buf[i]=table[l++];
    delete[] table;
    table= buf;
    sizeTable= sizeTable*2;
    
    
    
    
    
}

//                      findBool
template <typename Key, size_t N>
bool ADS_set<Key,N>::findBool(const key_type& key) const {
    return useBucket(findIndex(key)).findBucket(key);
}







// don't forget: findIndex is NOT always topIndex




#endif // ADS_SET_H
