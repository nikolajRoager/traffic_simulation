A custom container for storing vehicles on a road in sorted order
============
A Lane mainly functions as a single ended queue but random insertion and deletion is allowed, the elements T have an ordering operator against other T and against double pos, (the two ordering operators are transitive with each other)

Elements are looked up, based on either other elements, or the double they are equal to.

It is legal for elements to change what double they compare to, even outside control of the lane, a function `bool is_still_sorted()` exist for checking if the ordering has been altered.

All other functions assumes that `is_still_sorted()` has been verified true, or that no alterations have been made. The container DOES NOT verify this automatically, and running any functions on a Lane where `is_still_sorted()==false` will result in undefined behaviour.

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


Optional debug flags
-------
The following flags can be set BEFORE including the header file, to give additional information (and degrade performance):


    #define DEBUG_PRINT_ENABLE

Makes available the `void Lane<T>::DEBUG_print_tree() const noexcept` function, which prints the underlying tree structure. For this to work, `T` need to be convertible to `std::string`, if it is not, you can define your own conversion function by defining this anywhere in your file, where `myClass` is your specific class:


    template <>
        std::string inline my_to_string<myClass>(const myClass& t)
        {
            return /*Custom function for printing*/
        }

Additionally, the flag:

    #define DEBUG_PRINT_ON_INSERT

Automatically enables `DEBUG_PRINT_ENABLE`. It prints the tree every time an element is inserted, it also prints whenever any rotation is called during insert into the AVL tree.

The flag:

    #define DEBUG_loading_counter

Enables the `size_t loading_counter`, which counts how many internal node structs are currently loaded, providing a crude way of checking for memory leak. Also enables `std::mutex loading_counter_lock;` used to make any debugging and tests thread-safe.

Member functions and typedefs
-----
The tree has a custom iterator for stepping through the tree from lowest to smallest, it implements all requirements for containers: These work exactly the way they are required for a container (plus some extra from SequenceContainter and Associative container):

    Lane() noexcept;
    Lane(const Lane& other) noexcept;
    Lane(Lane&& other) noexcept;
    Lane& operator=(const Lane& other) noexcept;//O(N) copy constructor
    Lane& operator=(Lane&& other) noexcept;
    Lane(Iter start, Iter end);
    ~Lane() noexcept;
    Lane::iterator begin() noexcept;
    Lane::iterator end() noexcept;
    const Lane::const_iterator begin()const noexcept;
    const Lane::const_iterator end()  const noexcept;
    bool operator==(const Lane<T>& other) const noexcept;
    bool operator!=(const Lane<T>& other) const noexcept;
    void swap(Lane<T>& other) noexcept;
    static void swap(Lane<T>& a,Lane<T>& b) noexcept;
    size_type size() const noexcept;
    size_type max_size() const noexcept;
    bool empty() const noexcept;

Be aware that `==` (and by extension `!=`), as per the requirements checks that: the size is the same, and the elements between `begin()` and `end()` are the same. Another function exists which also checks that the tree structure is the same:

    bool strict_equal(const Lane<T>& Other) const;

Since AVL trees are not unique, it is possible to have to Lanes `a==b` be true but `a.strict_equal(b)` be false. This is unlikely to have any effects on anything

To fulfill the requirements, the following types have been defined:

    Lane<T>::value_type      = T;
    Lane<T>::pointer_type    = T*;
    Lane<T>::size_type       = size_t;
    Lane<T>::difference_type = std::ptrdiff_t;
    Lane<T>::reference       = T&;
    Lane<T>::const_reference = const T&;
    Lane<T>::iterator        = my_iterator;//custom LegacyForwardIterator
    Lane<T>::const_iterator  = const my_iterator;

Additionally these constructors can copy any other container in O(N log(N)):

    template<LegacyInputIterator Iter>//Accepts any input iterator
        Lane(Iter start, Iter end);
    Lane(std::initializer_list<T> il);
    Lane& operator=(std::initializer_list<T> il);



These functions are provided by accessing the underlying AVL-tree, these should run in time O(log(N)) (but only if `DEBUG_PRINT_ON_INSERT` is disabled):

    void insert(T&& t);
    void push_back(T t);//Same as insert, used for the std::back_inserter to work
    void erase(iterator i);
    void erase(const T& t);//same as erase(find(t);
    size_type count(double t)noexcept;
    size_type count(const T& t)noexcept;

The following finding functions all return `Lane<T>end()` for not found member, should be O(log(N))

    iterator find(const T& t)noexcept;
    iterator find(double t)noexcept;
    iterator lower_bound(T t)noexcept;
    iterator lower_bound(double t)noexcept;
    iterator upper_bound(T t)noexcept;
    iterator upper_bound(double t)noexcept;

Const versions also exists

    const_iterator find(const T& t) const noexcept;
    const_iterator find(double t) const noexcept;
    const_iterator lower_bound(T t) const noexcept;
    const_iterator lower_bound(double t) const noexcept;
    const_iterator upper_bound(T t) const noexcept;
    const_iterator upper_bound(double t) const noexcept;


The underlying linked list structure provides a function for checking if everything is still sorted:

    bool is_still_sorted() noexcept;

It also provides functions for get the element just before, or just before or after an iterator in time O(1), or at the start and end:

    iterator first() noexcept;
    iterator last () noexcept;
    const_iterator first() const noexcept;
    const_iterator last () const noexcept;

    iterator next(const_iterator&) noexcept;
    iterator prev(const_iterator&) noexcept;
    const_iterator next(const_iterator&) const noexcept;
    const_iterator prev(const_iterator&) const noexcept;

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
        The type X satisfies CopyConstructible,
        The type X satisfies CopyAssignable,
        The type X satisfies Destructible,
        lvalues of type X satisfy Swappable,
        std::iterator_traits<X> has member typedefs value_type(until C++20), difference_type, reference, pointer, and iterator_category (This is just a fancy way of saying that we need to write using difference_type = whatever in our class)
    For LegacyInputIterator:
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
| `++r`       | `X&`                      | `r` is incrementable           |
| `X(j)`      |                           |  T(j) is equal to j, j is unchanged|
| `i=j`       | `X&`                      |  i is equal to j, j is unchanged|
| `i.~X()`    |                           |  All resources owned by i are reclaimed, no exceptions are thrown |
| `i==j`      | convertible to `bool`     |  Equivalence relation          |
| `using std::swap;swap(i,j)`|            |  Swaps values of i and j       |
| `i!=j`      | convertible to `bool`     |Same as `!(i==j)`               |
| `*i`        |`R`, convertible to `T`    |`i==j` then `*i==*j`            |
| `i->m`      |                           |`(*i).m`                        |
| `(void)r++` | Equivalent to `(void)++r` |                                |
| `r++`       | convertible to `const X&` | Same as `X x=r; ++r; return x;`|
| `*r++`      | convertible to `R`        |                                |





[cpprforward]:  https://en.cppreference.com/w/cpp/named_req/ForwardIterator "Cppreference, Forward Iterator"
