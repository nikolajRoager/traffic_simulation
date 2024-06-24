A custom container for storing vehicles on a road
============
no default container really works well in this case, std::set comes close ... but I do not trust it to always work when the sorting key changes.

This container will only be used for the RoadVehicle class, but will work for anything T, with a < operator for T and double

Prerequisites
-------

This must be a fully functioning C++ container, compatible with all algorithms in the \<algorithm\> header (it will not be a sequence container, since the order of insertion fundamentally does not matter)

The pre-requisite for this container is that vehicles are sorted based on position.
position must be between 0 and length.

position can change without the container knowing about it (e.g. someone else stores a pointer to the vehicle and tell it to update), but we require that the relative ordering of the cars never change (overtaking requires deletion of the car on one lane, and insertion in another), the container should optionally have a "detect sorting errors" function

Vehicles can be inserted in O(log(N)) time based on position, a version of lower bound and upper bound should exist (to get get vehicles before or after an target position, or before or after any other vehicle), all this should use the CURRENT position

Random insertion and deletion must be possible (corresponding to overtaking)
allow fast insertion and deletion of vehicles at any location (should be O(log n)
very fast insertion of vehicles at one end, and deletion in other (this is the main mode vehicles enter and leave) O(1)

Getting next and previous car should be O(1)

For this reason, a AVL tree + double Linked List system will be used.
The AVL tree is a perfectly ordinary AVL tree, which uses left/right rotations, and leftright and rightleft rotations to maintain balance of the trees.

The linked list is updated on insertion and deletion of elements, and links elements in order of value.

Methods and members
-----
The tree has a custom iterator for stepping through the tree from lowest to smallest, it implements all requirements for containers: These work exactly the way they are required for a container (plus some extra from SequenceContainter and Associative container):


    Lane();
    Lane(const Lane& other);
    Lane(Lane&& other);
    Lane& operator=(const Lane& other);
    Lane& operator=(Lane&& other);
    ~Lane();
    Lane::iterator begin();
    Lane::iterator end();
    const Lane::const_iterator begin()const;
    const Lane::const_iterator end()  const;
    bool operator==(const Lane<T>& other);
    bool operator!=(const Lane<T>& other);
    void swap(Lane<T>& other);
    static void swap(Lane<T>& a,Lane<T>& b);
    size_type size();
    size_type max_size();
    bool empty();

Additionally this constructor copies from any other container in O(N log(N)):


    template<LegacyInputIterator Iter>//Accepts any input iterator
        Lane(Iter start, Iter end);


The tree provides these functions, ideally in time O(log(N)), it is likely pretty obvious how they work, there can only be one element with the same value (otherwise it is overwritten):

    void insert(T t);
    void push_back(T t);//Same as insert, used for the std::back_inserter to work
    size_type count(double t);
    size_type count(T t);
    iterator find(T t);
    iterator find(double t);

    //return end() if the element does not exist
    iterator lower_bound(T t);
    iterator lower_bound(double t);
    iterator upper_bound(T t);
    iterator upper_bound(double t);

The linked list structure provides a function for checking if everything is sorted:

    bool is_still_sorted() noexcept;

It also provides functions for get the element just before, or just before or after an iterator in time O(1), or at the start and end, these return false if the requested element do not exist

    bool first(T& out) noexcept;
    bool last (T& out) noexcept;

    bool next(T& out, const_iterator&) noexcept;
    bool prev(T& out, const_iterator&) noexcept;
    bool next(const T& out, const_iterator&) const noexcept;
    bool prev(const T& out, const_iterator&) const noexcept;

By mixing these with `lower\_bound`, `upper\_bound` and `find`, we can make these O(log(N)) functions:

    bool next(T& out, const T&) noexcept;
    bool prev(T& out, const T&) noexcept;
    bool next(const T& out, const T&) const noexcept;
    bool prev(const T& out, const T&) const noexcept;

Lane::iterator
------
The Lane::iterator must be a *LegacyForwardIterator*, quoting  from [cpprforward]: a *LegacyForwardIterator* need to satisfy *LegacyInputIterator* which in turns require *LegacyIterator*. Below are the combined requirements for all three requirements from [cpprforward], (and pages linked from that):

Here `X` is the iterator type, `T` is the value type, `i` and `j` are iterator instances, `R` is `std::iterator_traits<X>::reference`, and `r` is of type `R`

X satisfies *LegacyForwardIterator* if all following conditions are satisfied:


    for LegacyIterator:
        X satisfies DefaultConstructible.
        The type X satisfies CopyConstructible, and
        The type X satisfies CopyAssignable, and
        The type X satisfies Destructible, and
        lvalues of type X satisfy Swappable, and
        std::iterator_traits<X> has member typedefs value_type(until C++20), difference_type, reference, pointer, and iterator_category
    additionally for LegacyInputIterator:
        X satisfies EqualityComparable.
    for LegacyForwardIterator:
        If X is a mutable iterator, R is a reference to T.
        If X is a constant iterator, R is a reference to const T.
        Objects of the type X provide multi-pass guarantee.
        If i and j are equal, then either i and j are both dereferenceable or else neither is dereferenceable.
        If i and j are both dereferenceable, then i == j if and only if *i and *j are bound to the same object.


In addition, the following expressions must be valid, have the expected types and have their specified effects:


|Expression   |	Type                      |  Effects                       |
|-------------|---------------------------|--------------------------------|
| `*r`        | unspecified               | `r` is dereferenceable         |
|`++r`        | `R`                       | `r` is incrementable (the behavior of the expression `++r` is defined)|
| `i!=j`      | convertible to `bool`     |Same as `!(i==j)`               |
| `*i`        |`R`, convertible to `T`    |`i==j` then `*i==*j`            |
| `i->m`      |                           |`(*i).m`                        |
| `++r`       | `R`                       |                                |
| `(void)r++` | Equivilent to `(void)++r` |                                |
|`r++`        | convertible to `const X&` | Same as `X x=r; ++r; return x;`|
| `*r++`      | convertible to `R`        |                                |





[cpprforward]:  https://en.cppreference.com/w/cpp/named_req/ForwardIterator "Cppreference, Forward Iterator"
