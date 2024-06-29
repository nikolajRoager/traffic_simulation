#pragma once
/*
A Lane mainly functions as a single ended queue but random insertion and deletion is allowed, the elements T have an ordering operator against other T and against double pos, (the two ordering operators are transitive with each other)

Elements are looked up, based on either other elements, or the double they are equal to.

It is legal for elements to change what double they compare to, even outside control of the lane, a function `bool is_still_sorted()` exist for checking if the ordering has been altered.

All other functions assumes that `is_still_sorted()` has been verified true, or that no alterations have been made. The container DOES NOT verify this automatically, and running any functions on a Lane where `is_still_sorted()==false` will result in undefined behaviour.

This container will only be used for the RoadVehicle class, but will work for anything T, with a < operator for T and double

This is a modified AVL tree, modified for O(1) lookup of adjacent element.
*/

#include <cstddef>//size_t
#include <list>
#include <unordered_map>
#include <functional>
#include <limits>
#include <iterator>
#include <memory>
#include <utility>//std::swap


#include <type_traits>

//C++ 20 feature, for ensuring that the element has the require operation
#include <concepts>

//TEMP
#define DEBUG_PRINT_ENABLE

#ifdef DEBUG_PRINT_ON_INSERT
#define DEBUG_PRINT_ENABLE
#endif

#ifdef DEBUG_PRINT_ENABLE
#include<iostream>



#endif

template <typename Elem>
concept LaneElement = requires(Elem A, Elem B, double d){
    {A<B} -> std::convertible_to<bool>;
    {A<d} -> std::convertible_to<bool>;
    {A==B}-> std::convertible_to<bool>;
    {A==d}-> std::convertible_to<bool>;
};


//We are going to accept LegacyInputIterators, these are the requirements for them, this code snippet was generated with AI
template <typename Iter>
concept LegacyInputIterator = requires(Iter it) {
    {*it} -> std::convertible_to<typename std::iterator_traits<Iter>::value_type>;
    {++it}-> std::same_as<Iter&>;
    {it++}-> std::convertible_to<Iter>;
    typename std::iterator_traits<Iter>::difference_type;
    typename std::iterator_traits<Iter>::value_type;
    typename std::iterator_traits<Iter>::reference;
    typename std::iterator_traits<Iter>::pointer;
    typename std::iterator_traits<Iter>::iterator_category;
};





#ifdef DEBUG_PRINT_ENABLE

template <typename Elem>
concept DebugPrintable_LaneElement = requires(Elem A, Elem B, double d){
    {A<B} -> std::convertible_to<bool>;
    {A<d} -> std::convertible_to<bool>;
    {Elem::toString()} ->std::convertible_to<bool>;
};



//You can implement your own template function with the same name for any specific class, to override this printing function, see the design_document for Lane
template <LaneElement T>
    std::string inline my_to_string(const T& t)
    {
        //std::to_string works with all arithmetic types (integers or floating point), chars should just be printed
        if constexpr(std::is_same_v<T,char>)
            return std::string(1,t);
        else if constexpr(std::is_arithmetic_v<T>)
            return std::to_string(t);
        else
        {
            //Debug print should not be used in this case
            return "UNPRINTABLE";
        }
    }



#endif


//This class has NO public members!
//It was supposed to be an internal class in Lane<T>, but for reasons described in the design document for Lane and Node it was split up into its own class library
//By marking every member as private, and making the Lane<T> the only friend class, we can get the same effect
class Node{
private:
    //This was really supposed to be a struct, inside of Lane<T> but it is so large that it was moved to a separate class library, though friend statements are considered bad OOP, this is the best way to move this class outside Lane<T>, without givin anybody else access to its content, with minimal refactoring.
    template<LaneElement T> friend class Lane;

    std::shared_ptr<void> pvalue;

    using compare = std::function<bool(const std::shared_ptr<void>&, const std::shared_ptr<void>&)>;
    compare val_less;
    compare val_equal;

    using size_type         = size_t;



    //OWNED nodes
    Node* left=nullptr;
    Node* right=nullptr;


    //Non owned node, which we nevertheless know
    Node* next=nullptr;
    Node* prev=nullptr;
    Node* parent=nullptr;


    size_type height=0;//longest path to leaf
    int balance_factor=0;


    std::shared_ptr<void> get_pvalue() {return pvalue;}//get pointer to my value
    std::shared_ptr<const void> get_pvalue() const {return pvalue;}

    Node(std::shared_ptr<void> val,compare less, compare equal,Node* p=nullptr);

    ~Node();

    //Am I the same as this? in terms of subtree content and structure
    bool is_same_subtree(const Node* that)const;


    Node* insert(std::shared_ptr<void> val,Node*& least, Node*& greatest,size_type& mySize);

    //Returns new head of this subtree
    //ASSUMES right is not nullptr and has balance factor 1, otherwise this would not be called
    Node* rotateLeft();

    //Same but rotate right
    Node* rotateRight();

    //Same but rotate right-left
    Node* rotateRightLeft();

    //Same but rotate left-right
    Node* rotateLeftRight();

    //Returns new head of this subtree
    Node* insert(Node* New,size_type& mySize);


    void DEBUG_print(int indent, std::function<std::string(const std::shared_ptr<void>&)> printer ) const noexcept;


};






template<LaneElement T>
class Lane
{
//Define these public types, which we will need for our private members, therefore our private members are defined at the very bottom
public:

    using value_type        = T;
    using pointer_type      = std::shared_ptr<T>;
    using const_pointer_type= std::shared_ptr<const T>;
    using size_type         = Node::size_type;
    using difference_type   = std::ptrdiff_t;
    using reference         = T&;
    using const_reference   = const T&;




 //We need the Node class, before we can make our iterator, so go into private mode and define that
 private:

    //These are the comparisons which our Nodes shall use to sort object
    static bool T_less_than(const std::shared_ptr<void>& a, const std::shared_ptr<void>& b) {
        return *std::static_pointer_cast<T>(a) < *std::static_pointer_cast<T>(b);
    };
    static bool T_equal(const std::shared_ptr<void>& a, const std::shared_ptr<void>& b) {
        return *std::static_pointer_cast<T>(a) == *std::static_pointer_cast<T>(b);
    };




    class my_iterator
    {
    public:
        //Look at design document for LaneContainer for exactly what requirements are needed
        using difference_type = std::ptrdiff_t;
        using value_type = T;//Not required since C++20
        using reference= T&;
        using const_reference= const T&;
        using pointer= std::shared_ptr<T>;
        using const_pointer= std::shared_ptr<const T>;
        using iterator_category= std::forward_iterator_tag;
    private:
        Node* myNode=nullptr;//This is used as Lane.end(), since that is what we get if we use one too many ++
    public:

        reference  operator*()
        {
            return *(std::static_pointer_cast<value_type>(myNode->pvalue));
        }

        const_reference operator*() const
        {
            return *(std::static_pointer_cast<const value_type>(myNode->pvalue));
        }


        //UNTESTED
        pointer operator-> ()
        {
            return std::static_pointer_cast<value_type>(myNode->pvalue);
        }

        //UNTESTED
        const_pointer operator-> () const
        {
            return std::static_pointer_cast<value_type>(myNode->pvalue);
        }

        my_iterator(Node* N=nullptr) noexcept {myNode=N;}//The nullptr node is the same as Lane.end()

        void swap(my_iterator& it)
        {
            std::swap(myNode,it.myNode);
        }


        my_iterator& operator++()
        {
            if (myNode!=nullptr) myNode=myNode->next;
            return *this;
        }

        my_iterator(const my_iterator& j)//copy constructor
        {
            myNode=j.myNode;
        }

        const my_iterator& operator=(const  my_iterator& j)//copy assignment
        {
            myNode=j.myNode;
            return *this;
        }

        //move constructor and assignment, not required but good to have
        my_iterator(my_iterator&& j)
        {
            myNode=j.myNode;
            j.myNode=nullptr;
        }

        my_iterator& operator=(my_iterator&& j)
        {
            myNode=j.myNode;
            j.myNode=nullptr;
            return *this;
        }

        ~my_iterator()
        {
            //No delete, we don't own the pointer
        }

        bool operator==(const my_iterator& j) const
        {
            return j.myNode==myNode;
        }

        bool operator!=(const my_iterator& j) const
        {
            //equivilant !(*this==j)
            return j.myNode!=myNode;
        }
    };
 public:

    using iterator = my_iterator;
    using const_iterator = const my_iterator;

//EVERYTHING BELOW ARE Requirements for CONTAINER (see https://en.cppreference.com/w/cpp/named_req/Container)
    Lane() noexcept{;//default constructor
        head=nullptr;
        least=nullptr;
        greatest=nullptr;
        mySize=0;
    }

    //UNTESTED ISSUE, Do the accessed elements resolve to unique elements?
    Lane(const Lane<T>& other) noexcept
    {
        mySize=other.mySize;
        if (other.head==nullptr)
        {
            head=nullptr;
            least=nullptr;//If head is null, the others should be null as well
            greatest=nullptr;
        }
        else
        {
            //Hashtable lookup for easy access of which new nodes in my tree, correspond to nodes in the old tree
            std::unordered_map<const Node*,Node*> Node_lookup;

            for (Node* N = other.least; N!=nullptr; N=N->next)
            {
                Node* my_N;

                //Check if this Node has already been initialized in the new tree, if not make it, and put it in the table
                auto it = Node_lookup.find(N);
                if (it == Node_lookup.end())
                {
                    my_N=new Node(std::make_shared<T>( *std::static_pointer_cast<T>(N->pvalue) /*Force copy constructor!*/),T_less_than,T_equal);
                    Node_lookup[N]=my_N;
                }
                else
                {
                    my_N=it->second;
                    my_N->pvalue=std::make_shared<T>( *std::static_pointer_cast<T>(N->pvalue) /*Force copy constructor!*/);
                }

                //Copy the value into this new Node
                my_N->height=N->height;
                my_N->balance_factor=N->balance_factor;

                //Check all Nodes connected to this, if they don't already exist, make them, no need to bother about their individual values, we will either get to them later, or have already been there

                if (N->left==nullptr)
                    my_N->left=nullptr;
                else
                {
                    it = Node_lookup.find(N->left);
                    if (it == Node_lookup.end())
                    {
                        my_N->left=new Node(std::make_shared<T>( *std::static_pointer_cast<T>(N->left->pvalue)),T_less_than,T_equal);
                        Node_lookup[N->left]=my_N->left;
                    }
                    else
                        my_N->left=it->second;
                }
                if (N->right==nullptr)
                    my_N->right=nullptr;
                else
                {
                    it = Node_lookup.find(N->right);
                    if (it == Node_lookup.end())
                    {
                        my_N->right=new Node(std::make_shared<T>( *std::static_pointer_cast<T>(N->right->pvalue)),T_less_than,T_equal);
                        Node_lookup[N->right]=my_N->right;
                    }
                    else
                        my_N->right=it->second;
                }
                if (N->prev==nullptr)
                    my_N->prev=nullptr;
                else
                {
                    //This one is guaranteed to exist, since we started from the least
                    it = Node_lookup.find(N->prev);
                    if (it == Node_lookup.end())
                    {
                        my_N->prev=new Node(std::make_shared<T>( *std::static_pointer_cast<T>(N->prev->pvalue)),T_less_than,T_equal);
                        Node_lookup[N->prev]=my_N->prev;
                    }
                    else
                        my_N->prev=it->second;
                }
                if (N->next==nullptr)
                    my_N->next=nullptr;
                else
                {
                    it = Node_lookup.find(N->next);
                    if (it == Node_lookup.end())
                    {
                        my_N->next=new Node(std::make_shared<T>( *std::static_pointer_cast<T>(N->next->pvalue)),T_less_than,T_equal);
                        Node_lookup[N->next]=my_N->next;
                    }
                    else
                        my_N->next=it->second;
                }


                if (N->parent==nullptr)
                {
                    my_N->parent=nullptr;
                    head=my_N;//This is the head
                }
                else
                {
                    it = Node_lookup.find(N->parent);
                    if (it == Node_lookup.end())
                    {
                        my_N->parent=new Node(std::make_shared<T>( *std::static_pointer_cast<T>(N->parent->pvalue)),T_less_than,T_equal);
                        Node_lookup[N->parent]=my_N->parent;
                    }
                    else
                        my_N->parent=it->second;
                }
            }


            //If other was a valid Lane, these should already exist and be non-null
            least   = Node_lookup[other.least];
            greatest= Node_lookup[other.greatest];
        }

    }


    Lane(Lane&& other) noexcept
    {


        mySize=other.mySize;
        other.mySize=0;
        head = other.head;
        other.head=nullptr;
        least=other.least;
        other.least=nullptr;
        greatest=other.greatest;
        other.greatest=nullptr;
    }

    //UNTESTED ISSUE, Do the accessed elements resolve to unique elements?
    Lane& operator=(const Lane& other) noexcept//copy assignment
    {



        if (this!=&other)//This would result in a double-free
        {

            this->~Lane();

            mySize=other.mySize;

            {
                if (other.head==nullptr)
                {
                    head=nullptr;
                    least=nullptr;//If head is null, the others should be null as well
                    greatest=nullptr;
                }
                else
                {
                    //Hashtable lookup for easy access of which new nodes in my tree, correspond to nodes in the old tree
                    std::unordered_map<const Node*,Node*> Node_lookup;

                    for (Node* N = other.least; N!=nullptr; N=N->next)
                    {
                        Node* my_N;

                        //Check if this Node has already been initialized in the new tree, if not make it, and put it in the table
                        auto it = Node_lookup.find(N);
                        if (it == Node_lookup.end())
                        {
                            my_N=new Node(std::make_shared<T>( *std::static_pointer_cast<T>(N->pvalue) /*Force copy constructor!*/),T_less_than,T_equal);
                            Node_lookup[N]=my_N;
                        }
                        else
                        {
                            my_N=it->second;
                            my_N->pvalue=std::make_shared<T>( *std::static_pointer_cast<T>(N->pvalue) /*Force copy constructor!*/);
                        }

                        //Copy the value into this new Node
                        my_N->height=N->height;
                        my_N->balance_factor=N->balance_factor;

                        //Check all Nodes connected to this, if they don't already exist, make them, no need to bother about their individual values, we will either get to them later, or have already been there

                        if (N->left==nullptr)
                            my_N->left=nullptr;
                        else
                        {
                            it = Node_lookup.find(N->left);
                            if (it == Node_lookup.end())
                            {
                                my_N->left=new Node(std::make_shared<T>( *std::static_pointer_cast<T>(N->left->pvalue)),T_less_than,T_equal);
                                Node_lookup[N->left]=my_N->left;
                            }
                            else
                                my_N->left=it->second;
                        }
                        if (N->right==nullptr)
                            my_N->right=nullptr;
                        else
                        {
                            it = Node_lookup.find(N->right);
                            if (it == Node_lookup.end())
                            {
                                my_N->right=new Node(std::make_shared<T>( *std::static_pointer_cast<T>(N->right->pvalue)),T_less_than,T_equal);
                                Node_lookup[N->right]=my_N->right;
                            }
                            else
                                my_N->right=it->second;
                        }
                        if (N->prev==nullptr)
                            my_N->prev=nullptr;
                        else
                        {
                            //This one is guaranteed to exist, since we started from the least
                            it = Node_lookup.find(N->prev);
                            if (it == Node_lookup.end())
                            {
                                my_N->prev=new Node(std::make_shared<T>( *std::static_pointer_cast<T>(N->prev->pvalue)),T_less_than,T_equal);
                                Node_lookup[N->prev]=my_N->prev;
                            }
                            else
                                my_N->prev=it->second;
                        }
                        if (N->next==nullptr)
                            my_N->next=nullptr;
                        else
                        {
                            it = Node_lookup.find(N->next);
                            if (it == Node_lookup.end())
                            {
                                my_N->next=new Node(std::make_shared<T>( *std::static_pointer_cast<T>(N->next->pvalue)),T_less_than,T_equal);
                                Node_lookup[N->next]=my_N->next;
                            }
                            else
                                my_N->next=it->second;
                        }


                        if (N->parent==nullptr)
                        {
                            my_N->parent=nullptr;
                            head=my_N;//This is the head
                        }
                        else
                        {
                            it = Node_lookup.find(N->parent);
                            if (it == Node_lookup.end())
                            {
                                my_N->parent=new Node(std::make_shared<T>( *std::static_pointer_cast<T>(N->parent->pvalue)),T_less_than,T_equal);
                                Node_lookup[N->parent]=my_N->parent;
                            }
                            else
                                my_N->parent=it->second;
                        }
                    }


                    //If other was a valid Lane, these should already exist and be non-null
                    least   = Node_lookup[other.least];
                    greatest= Node_lookup[other.greatest];

                }
            }
        }
        return *this;
    }

    Lane& operator=(Lane&& other) noexcept//move assignment
    {


        if (this!=&other)
        {
            this->~Lane();

            head = other.head;
            least=other.least;
            greatest=other.greatest;
            mySize=other.mySize;
            other.mySize=0;
            other.head=nullptr;
            other.least=nullptr;
            other.greatest=nullptr;
        }
        return *this;
    }


    //destructor
    ~Lane() noexcept
    {
        delete head;//Destructs everything
    }

    iterator begin()noexcept {return iterator(least);}
    iterator end()  noexcept {return iterator(nullptr);}

    const const_iterator begin()const noexcept {return iterator(least);}
    const const_iterator end()  const noexcept {return iterator(nullptr);}

    //As per the C++ standard https://en.cppreference.com/w/cpp/named_req/Container this returns TRUE if the container contains the SAME ELEMENTS, in the SAME ORDER.
    //It is possible that this and b have DIFFERENT internal tree-structure but still return true
    bool operator==(const Lane<T>& b) const noexcept
    {
        if (b.size()==size())
        {
            return std::equal(begin(),end(),b.begin());
        }
        else
        {
            return false;
        }
    }

    bool operator!=(const Lane<T>& b) const noexcept
    {
        return !(operator==(b));
    }


    //This much stricter search requires the same structure, the C++ standard says this is TOO strict for the == operator, but this could still be useful
    bool strict_equal(const Lane<T>& b) const noexcept
    {
        if (b.size()==size())
        {
            return (head==nullptr && b.head==nullptr) ||  (head!=nullptr && head->is_same_subtree(b.head)) && *(std::static_pointer_cast<const T>(least->pvalue)) == *(std::static_pointer_cast<const T>(b.least->pvalue)) && *(std::static_pointer_cast<const T>(greatest->pvalue)) == *(std::static_pointer_cast<const T>(b.greatest->pvalue));
        }
        else
            return false;
    }




    void swap(Lane<T>& b) noexcept
    {
        //No need to reinvent what already exists
        std::swap(head,b.head);
        std::swap(least,b.least);
        std::swap(greatest,b.greatest);
        std::swap(mySize,b.mySize);
    }

    static void swap(Lane<T>& a,Lane<T>& b) noexcept
    {
        a.swap(b);
    }

    size_type size()const noexcept
    {
        //Should equal std::distance(begin(),end());
        return mySize;
    }

    size_type max_size()const noexcept
    {
        return std::numeric_limits<size_type>::max();
    }

    bool empty()const noexcept
    {
        return head==nullptr;
    }


//END OF CONTAINER REQUIREMENTS

//We only implement SOME Requirements for SEQUENCE CONTAINER (see https://en.cppreference.com/w/cpp/named_req/SequenceContainer), it doesn't make sense to implement all

    //Accept any input iterators
    template<LegacyInputIterator Iter>
        Lane(Iter start, Iter end)
        {
            std::copy(start,end,std::back_inserter(*this)); //This is O(N log N) the same as std::sort
        }


    //I could not get enough documentation to implement Lane(std::from_range, rg), thus this is NOT C++23 compatible
    Lane(std::initializer_list<T> il):Lane(il.begin(),il.end()){}

    Lane& operator =(std::initializer_list<T> il)
    {
        //Call the constructor on a fresh object
        Lane that(il.begin(),il.end());

        //now this is that and that is this
        std::swap(*this,that);
        return *this;
        //as that goes out of scope, the old data is safely handled by the destructor
    }


    void inline insert(T&& t) noexcept
    {

        push_back(std::move(t));
    }

    //Now the important ones, the actual insertions followed by sorting
    //I name it push_back, to get a working std::back_inserter
    void push_back(T t) noexcept
    {
        if (head==nullptr)
        {
            head     = new Node(std::make_shared<T>(std::move(t)),T_less_than,T_equal);
            least    = head;
            greatest = head;
            mySize = 1;
        }
        else
        {
            head = head->insert(std::make_shared<T>(std::move(t)),least,greatest,mySize);

        }

        #ifdef DEBUG_PRINT_ON_INSERT
        std::cout<<"AFTER INSERT OF "<<my_to_string(t)<<" THE TREE LOOKS LIKE THIS: "<<std::endl;
        DEBUG_print_tree();
        #endif
    }

    /*MISSIG IMPLEMENTATION

    void erase(iterator& it)
    {

    }

    void erase(const T& t)
    {
        erase(find(t));
    }

    iterator& find(T& t) noexcept
    {

    }

    const_iterator& find(const T& t) noexcept
    {

    }

        size_type count(double t)noexcept;
        size_type count(const T& t)noexcept;
        iterator find(const T& t)noexcept;
        iterator find(double t)noexcept;
        iterator lower_bound(T t)noexcept;
        iterator lower_bound(double t)noexcept;
        iterator upper_bound(T t)noexcept;
        iterator upper_bound(double t)noexcept;

        const_iterator find(const T& t) const noexcept;
        const_iterator find(double t) const noexcept;
        const_iterator lower_bound(T t) const noexcept;
        const_iterator lower_bound(double t) const noexcept;
        const_iterator upper_bound(T t) const noexcept;
        const_iterator upper_bound(double t) const noexcept;

        bool is_still_sorted() noexcept;


        iterator first() noexcept;
        iterator last () noexcept;
        const_iterator first() const noexcept;
        const_iterator last () const noexcept;

        iterator next(const_iterator&) noexcept;
        iterator prev(const_iterator&) noexcept;
        const_iterator next(const_iterator&) const noexcept;
        const_iterator prev(const_iterator&) const noexcept;
    */


    #ifdef DEBUG_PRINT_ENABLE
    void DEBUG_print_tree() const noexcept
    {
        if (head==nullptr)
            std::cout<<"Empty"<<std::endl;
        else
        {
            std::cout<<"By tree:\n";
            head->DEBUG_print(0,[](const std::shared_ptr<const void>& P){ return my_to_string(*std::static_pointer_cast<const T>(P));});

            std::cout<<"\nBy size: ";
            for (const Node* N = least; N!=nullptr; N=N->next)
                std::cout<<my_to_string( *std::static_pointer_cast<T>(N->pvalue) )<<' ';
            std::cout<<std::endl;
        }
    }
    #endif

private:

    //owned
    Node* head    =nullptr;

    //Non-owned
    Node* least   =nullptr;
    Node* greatest=nullptr;

    size_type mySize=0;

    //Call basic static asserts, ensuring that our iterator is indeed an iterator
    static_assert(std::is_copy_constructible_v<Lane<T>::iterator>);
    static_assert(std::is_copy_assignable_v<Lane<T>::iterator>);
    static_assert(std::is_destructible_v<Lane<T>::iterator>);
    static_assert(std::is_swappable_v<Lane<T>::iterator>);
    static_assert(std::is_default_constructible_v<Lane<T>::iterator>);

    static_assert(std::is_same<typename std::iterator_traits<Lane<T>::iterator>::difference_type,std::ptrdiff_t>::value);
    static_assert(std::is_same<typename std::iterator_traits<Lane<T>::iterator>::reference,T&>::value);
    static_assert(std::is_same<typename std::iterator_traits<Lane<T>::iterator>::pointer,std::shared_ptr<T> >::value);
    static_assert(std::is_same<typename std::iterator_traits<Lane<T>::iterator>::iterator_category,std::forward_iterator_tag>::value);
};
