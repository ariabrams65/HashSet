// HashSet.hpp
//
// ICS 46 Spring 2022
// Project #4: Set the Controls for the Heart of the Sun
//
// A HashSet is an implementation of a Set that is a separately-chained
// hash table, implemented as a dynamically-allocated array of linked
// lists.  At any given time, the HashSet has a "size" indicating
// how many elements are stored within it, along with a "capacity"
// indicating the size of the array.
//
// As elements are added to the HashSet and the proportion of the HashSet's
// size to its capacity exceeds 0.8 (i.e., there are more than 80% as many
// elements as there are array cells), the HashSet should be resized so
// that it is twice as large as it was before.
//
// You are not permitted to use the containers in the C++ Standard Library
// (such as std::set, std::map, or std::vector) to store the information
// in your data structure.  Instead, you'll need to use a dynamically-
// allocated array and your own linked list implemenation; the linked list
// doesn't have to be its own class, though you can do that, if you'd like.

#ifndef HASHSET_HPP
#define HASHSET_HPP

#include <functional>
#include "Set.hpp"
#include <algorithm>
#include <iostream>

template <typename ElementType>
class HashSet : public Set<ElementType>
{
public:
    // The default capacity of the HashSet before anything has been
    // added to it.
    static constexpr unsigned int DEFAULT_CAPACITY = 10;

    // A HashFunction is a function that takes a reference to a const
    // ElementType and returns an unsigned int.
    using HashFunction = std::function<unsigned int(const ElementType&)>;

public:
    // Initializes a HashSet to be empty, so that it will use the given
    // hash function whenever it needs to hash an element.
    explicit HashSet(HashFunction hashFunction);

    // Cleans up the HashSet so that it leaks no memory.
    ~HashSet() noexcept override;

    // Initializes a new HashSet to be a copy of an existing one.
    HashSet(const HashSet& s);

    // Initializes a new HashSet whose contents are moved from an
    // expiring one.
    HashSet(HashSet&& s) noexcept;

    // Assigns an existing HashSet into another.
    HashSet& operator=(const HashSet& s);

    // Assigns an expiring HashSet into another.
    HashSet& operator=(HashSet&& s) noexcept;


    // isImplemented() should be modified to return true if you've
    // decided to implement a HashSet, false otherwise.
    bool isImplemented() const noexcept override;


    // add() adds an element to the set.  If the element is already in the set,
    // this function has no effect.  This function triggers a resizing of the
    // array when the ratio of size to capacity would exceed 0.8, in which case
    // the new capacity should be determined by this formula:
    //
    //     capacity * 2 + 1
    //
    // In the case where the array is resized, this function runs in linear
    // time (with respect to the number of elements, assuming a good hash
    // function); otherwise, it runs in constant time (again, assuming a good
    // hash function).  The amortized running time is also constant.
    void add(const ElementType& element) override;


    // contains() returns true if the given element is already in the set,
    // false otherwise.  This function runs in constant time (with respect
    // to the number of elements, assuming a good hash function).
    bool contains(const ElementType& element) const override;


    // size() returns the number of elements in the set.
    unsigned int size() const noexcept override;


    // elementsAtIndex() returns the number of elements that hashed to a
    // particular index in the array.  If the index is out of the boundaries
    // of the array, this function returns 0.
    unsigned int elementsAtIndex(unsigned int index) const;


    // isElementAtIndex() returns true if the given element hashed to a
    // particular index in the array, false otherwise.  If the index is
    // out of the boundaries of the array, this functions returns false.
    bool isElementAtIndex(const ElementType& element, unsigned int index) const;

    int capacity();


private:
    struct Node
    {
        ElementType value;
        Node* next;
    };

    HashFunction hashFunction;
    int _sz;
    int _cap;
    Node** _list;

    void delete_list();

    void copy_list(Node** list);

    void resize();
};


namespace impl_
{
    template <typename ElementType>
    unsigned int HashSet__undefinedHashFunction(const ElementType& element)
    {
        return 0;
    }
}

namespace
{
    template <typename T>
    void null_init_list(T** list, int size)
    {
        for (int i = 0; i < size; ++i)
        {
            list[i] = nullptr;
        }
    }
}


template <typename ElementType>
int HashSet<ElementType>::capacity()
{
    return _cap;
}

template <typename ElementType>
void HashSet<ElementType>::delete_list()
{
    for (int i = 0; i < _cap; ++i)
    {
        Node* cur = _list[i];
        while (cur != nullptr)
        {
            Node* next = cur->next;
            delete cur;
            cur = next;
        }
    }
    if (_list != nullptr)
    {
        delete[] _list;
    }
}

template <typename ElementType>
void HashSet<ElementType>::copy_list(Node** list)
{
    for (int i = 0; i < _cap; ++i)
    {
        Node* curNode = list[i];
        if (curNode == nullptr)
        {
            _list[i] = nullptr;
            continue;
        }
        Node* newNode = new Node{curNode->value, nullptr};
        _list[i] = newNode;
        curNode = curNode->next;

        while (curNode != nullptr)
        {
            newNode->next = new Node{curNode->value, nullptr}; 

            curNode = curNode->next;
            newNode = newNode->next;
        }
    }
}

template <typename ElementType>
void HashSet<ElementType>::resize()
{
    int newCap = _cap * 2 + 1;
    Node** newList = new Node*[newCap];    
    null_init_list<Node>(newList, newCap);

    for (int i = 0; i < _cap; ++i)
    {
        Node* cur = _list[i];
        if (cur == nullptr)
        {
            continue;
        }
        while (cur != nullptr)
        {
            int index = hashFunction(cur->value) % newCap;
            newList[index] = new Node{cur->value, newList[index]}; 
            cur = cur->next;
        }
    }
    delete_list(); 
    _list = newList;
    _cap = newCap;
}


template <typename ElementType>
HashSet<ElementType>::HashSet(HashFunction hashFunction)
    : hashFunction{hashFunction}, _sz{0}, _cap{DEFAULT_CAPACITY}, _list{new Node*[DEFAULT_CAPACITY]}
{
    null_init_list<Node>(_list, _cap); 
}


template <typename ElementType>
HashSet<ElementType>::~HashSet() noexcept
{
    delete_list();
}


template <typename ElementType>
HashSet<ElementType>::HashSet(const HashSet& s)
    : hashFunction{s.hashFunction},
      _sz{s._sz}, _cap{s._cap}, _list{new Node*[_cap]}
{
    copy_list(s._list);
}


template <typename ElementType>
HashSet<ElementType>::HashSet(HashSet&& s) noexcept
    : hashFunction{impl_::HashSet__undefinedHashFunction<ElementType>},
      _sz{0}, _cap{0}, _list{nullptr}
{
    std::swap(_list, s._list);
    std::swap(_sz, s._sz);
    std::swap(_cap, s._cap);
    std::swap(hashFunction, s.hashFunction);
}


template <typename ElementType>
HashSet<ElementType>& HashSet<ElementType>::operator=(const HashSet& s)
{
    delete_list();

    _sz = s._sz;
    _cap = s._cap;
    hashFunction = s.hashFunction;

    _list = new Node*[_cap];
    copy_list(s._list); 

    return *this;
}


template <typename ElementType>
HashSet<ElementType>& HashSet<ElementType>::operator=(HashSet&& s) noexcept
{
    std::swap(_list, s._list);
    std::swap(_sz, s._sz);
    std::swap(_cap, s._cap);
    std::swap(hashFunction, s.hashFunction);
    return *this;
}


template <typename ElementType>
bool HashSet<ElementType>::isImplemented() const noexcept
{
    return true;
}


template <typename ElementType>
void HashSet<ElementType>::add(const ElementType& element)
{
    if(contains(element))
    {
        return;
    }
    ++_sz;
    if ((_sz * 1.0) / _cap > .8)
    {
        resize();
    }
    int index = hashFunction(element) % _cap;
    _list[index] = new Node{element, _list[index]};
}


template <typename ElementType>
bool HashSet<ElementType>::contains(const ElementType& element) const
{
    int index = hashFunction(element) % _cap;
    return isElementAtIndex(element, index);
    
}


template <typename ElementType>
unsigned int HashSet<ElementType>::size() const noexcept
{
    return _sz;
}


template <typename ElementType>
unsigned int HashSet<ElementType>::elementsAtIndex(unsigned int index) const
{
    if (index >= _cap)
    {
        return 0;
    }
    int num = 0;

    Node* cur = _list[index];
    while (cur != nullptr)
    {
        ++num;
        cur = cur->next;
    }
    return num;
}


template <typename ElementType>
bool HashSet<ElementType>::isElementAtIndex(const ElementType& element, unsigned int index) const
{
    Node* cur = _list[index];
    while (cur != nullptr)
    {
        if (cur->value == element)
        {
            return true;
        }
        cur = cur->next;
    }
    return false;
}



#endif

