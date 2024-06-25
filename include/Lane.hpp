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
#include <ranges>
#include <limits>
#include <iterator>
#include <utility>//std::swap


#include <type_traits>

//C++ 20 feature, for ensuring that the element has the require operation
#include <concepts>

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
};


//We are going to accept LegacyInputIterators, these are the requirements for them, this code snippet was generated with AI
template <typename Iter>
concept LegacyInputIterator = requires(Iter it) {
    {*it} -> std::convertible_to<typename std::iterator_traits<Iter>::value_type>;
    {++it} -> std::same_as<Iter&>;
    {it++} -> std::convertible_to<Iter>;
    typename std::iterator_traits<Iter>::difference_type;
    typename std::iterator_traits<Iter>::value_type;
    typename std::iterator_traits<Iter>::reference;
    typename std::iterator_traits<Iter>::pointer;
    typename std::iterator_traits<Iter>::iterator_category;
};



#ifdef DEBUG_loading_counter

//idk if this debugging option will be used in a multithreaded context, but I assume so
#include<mutex>
std::mutex loading_counter_lock;
size_t loading_counter=0;
#endif


#ifdef DEBUG_PRINT_ENABLE

template <typename Elem>
concept DebugPrintable_LaneElement = requires(Elem A, Elem B, double d){
    {A<B} -> std::convertible_to<bool>;
    {A<d} -> std::convertible_to<bool>;
    {Elem::toString()} ->std::convertible_to<bool>;
};



//to_string will
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


template<LaneElement T>
class Lane
{
//Define these public types, which we will need for our private members, therefore our private members are defined at the very bottom
public:

    using value_type      = T;
    using pointer_type    = T*;
    using size_type       = size_t;
    using difference_type = std::ptrdiff_t;
    using reference       = T&;
    using const_reference = const T&;

 //We need the Node class, before we can make our iterator, so go into private mode and define that
 private:
    //We will also use a binary tree to search elements,
    struct Node{
        T value;

        //OWNED nodes
        Node* left=nullptr;
        Node* right=nullptr;


        //Non owned node, which we nevertheless know
        Node* next=nullptr;
        Node* prev=nullptr;
        Node* parent=nullptr;


        size_type height=0;//longest path to leaf
        int balance_factor=0;


        Node(T val, Node* p=nullptr)
        {
            #ifdef DEBUG_loading_counter
            loading_counter_lock.lock();
            ++loading_counter;
            loading_counter_lock.unlock();
            #endif

            height=1;
            value = std::move(val);
            left=nullptr;
            right=nullptr;
            balance_factor=0;

            next=nullptr;
            prev=nullptr;
            parent=p;
        }

        ~Node()
        {
            //Already checks null
            delete left;
            delete right;

            #ifdef DEBUG_loading_counter
            loading_counter_lock.lock();
            --loading_counter;
            loading_counter_lock.unlock();
            #endif
        }

        //Am I the same as this? in terms of subtree content and structure
        bool is_same_subtree(const Node* that)const
        {
            //Short circuit logic = We make the most trivial comparisons first, if they fail we skip expensive calculations
            return that!=nullptr && that->value==value//Same value, and that thing is not broken
             && ((left==nullptr && that->left==nullptr) || (left!=nullptr && that->left !=nullptr && left->is_same_subtree(that->left)))//Left side is the same (both null, or the subtree is the same)
            && ((right==nullptr && that->right==nullptr) || (right!=nullptr && that->right !=nullptr && right->is_same_subtree(that->right)));//Same with the right side
            //Do not make the same kind of search of the next and previous, if the tree looks the same they are the same

        }


        Node* insert(T&& val,Node*& least, Node*& greatest,size_type& mySize)
        {
            Node* New = new Node(std::move(val),this);
            if (least==nullptr   || least->value>New->value)
            {
                least=New;
            }
            if (greatest==nullptr|| greatest->value<New->value)
            {
                greatest=New;
            }



            return insert(New,mySize);
        }

        //Returns new head of this subtree
        //ASSUMES right is not nullptr and has balance factor 1, otherwise this would not be called
        Node* rotateLeft()
        {
            #ifdef DEBUG_PRINT_ON_INSERT
            std::cout<<"R ON "<<my_to_string(value)<<std::endl;
            #endif
            Node* oldRight = right;

            Node* oldRightLeft = right->left;//may be null, doesn't matter

            //Now that is my right
            right=oldRightLeft;
            if (oldRightLeft!=nullptr)
                oldRightLeft->parent=oldRight;

            //Move this onto old right
            oldRight->left = this;
            oldRight->parent= parent;
            parent = oldRight;

            if (oldRight->balance_factor==0)
            {
                balance_factor=1;
                oldRight->balance_factor=-1;
            }
            else
            {
                balance_factor=0;
                oldRight->balance_factor=0;

            }

            //Now reset the heights, oldRightLeft has not changed children, so it is unchanged,
            height = std::max(left ==nullptr ? size_type(0) : left->height, right==nullptr ? size_type(0) : right->height)+1;
            oldRight->height = std::max(height, oldRight->right==nullptr ? size_type(0) : oldRight->right->height)+1;

            return oldRight;//This is the parent of this subtree now
        }
        //Same but rotate right
        Node* rotateRight()
        {
            #ifdef DEBUG_PRINT_ON_INSERT
            std::cout<<"R ON "<<my_to_string(value)<<std::endl;
            #endif
            Node* oldLeft = left;

            Node* oldLeftRight = left->right;//may be null, doesn't matter

            //Now that is my left
            left=oldLeftRight;
            if (oldLeftRight!=nullptr)
                oldLeftRight->parent=oldLeft;

            //Move this onto old left
            oldLeft->right = this;
            oldLeft->parent= parent;
            parent = oldLeft;

            if (oldLeft->balance_factor==0)
            {
                balance_factor=1;
                oldLeft->balance_factor=-1;
            }
            else
            {
                balance_factor=0;
                oldLeft->balance_factor=0;

            }

            //Now reset the heights, oldLeftRight has not changed children, so it is unchanged,
            height = std::max(right ==nullptr ? size_type(0) : right->height, left==nullptr ? size_type(0) : left->height)+1;
            oldLeft->height = std::max(height, oldLeft->left==nullptr ? size_type(0) : oldLeft->left->height)+1;

            return oldLeft;//This is the parent of this subtree now
        }

        //Same but rotate right-left
        Node* rotateRightLeft()
        {
            #ifdef DEBUG_PRINT_ON_INSERT
            std::cout<<"RL ON "<<my_to_string(value)<<std::endl;
            #endif
            //If we get here this is valid
            Node* oldRight     = right;
            Node* oldRightLeft = right->left;


            // if we get here, oldRightLeft is not null, but its children might be
            Node* oldRightLeftRight = oldRightLeft->right;

            oldRight->left = oldRightLeftRight;
            if (oldRightLeftRight != nullptr)
                oldRightLeftRight ->parent = oldRight;
            oldRightLeft -> right = oldRight;
            oldRight->parent = oldRightLeft ;

            Node* oldRightLeftLeft = oldRightLeft->left;
            this->right = oldRightLeftLeft;

            if (oldRightLeftLeft!= nullptr)
                oldRightLeftLeft->parent = this;
            oldRightLeft->left = this;
            this->parent = oldRightLeft ;

            if (oldRightLeft->balance_factor == 0) {
                this->balance_factor = 0;
                oldRight->balance_factor = 0;
            } else if (oldRightLeft->balance_factor > 0) {
                this->balance_factor = -1;
                oldRight->balance_factor = 0;
            } else {
                this->balance_factor = 0;
                oldRight->balance_factor = +1;
            }
            oldRightLeft->balance_factor = 0;

            //The overall hierachy after the change looks like this:
            /*
            oldRight->left = oldRightLeftRight;
            this->right    = oldRightLeftLeft;
            oldRightLeft -> right = oldRight;
            oldRightLeft->left = this;

            So, we should update the height of the two at the top, before doing the last,safely knowing that its nodes are not null
            */
            oldRight->height = std::max(oldRight->left==nullptr ? size_type(0) : oldRight->left->height , oldRight->right==nullptr ? size_type(0) : oldRight->right->height)+1;
            this->height = std::max(this->left==nullptr ? size_type(0) : this->left->height , this->right==nullptr ? size_type(0) : this->right->height)+1;
            oldRightLeft->height = std::max(oldRightLeft->left->height ,  oldRightLeft->right->height)+1;


            return oldRightLeft ; // return new root of rotated subtree
        }

        //Same but rotate left-right
        Node* rotateLeftRight()
        {
            #ifdef DEBUG_PRINT_ON_INSERT
            std::cout<<"LR ON "<<my_to_string(value)<<std::endl;
            #endif
            //If we get here this is valid
            Node* oldLeft     = left;
            Node* oldLeftRight = left->right;

            // if we get here, oldLeftRight is not null, but its children might be
            Node* oldLeftRightLeft = oldLeftRight->left;

            oldLeft->right = oldLeftRightLeft;
            if (oldLeftRightLeft != nullptr)
                oldLeftRightLeft ->parent = oldLeft;
            oldLeftRight -> left = oldLeft;
            oldLeft->parent = oldLeftRight ;

            Node* oldLeftRightRight = oldLeftRight->right;
            this->left = oldLeftRightRight;

            if (oldLeftRightRight!= nullptr)
                oldLeftRightRight->parent = this;
            oldLeftRight->right = this;
            this->parent = oldLeftRight ;

            if (oldLeftRight->balance_factor == 0) {
                this->balance_factor = 0;
                oldLeft->balance_factor = 0;
            } else if (oldLeftRight->balance_factor > 0) {
                this->balance_factor = -1;
                oldLeft->balance_factor = 0;
            } else {
                this->balance_factor = 0;
                oldLeft->balance_factor = +1;
            }
            oldLeftRight->balance_factor = 0;



            oldLeft->height = std::max(oldLeft->left==nullptr ? size_type(0) : oldLeft->left->height , oldLeft->left==nullptr ? size_type(0) : oldLeft->left->height)+1;
            this->height = std::max(this->left==nullptr ? size_type(0) : this->left->height , this->left==nullptr ? size_type(0) : this->left->height)+1;
            oldLeftRight->height = std::max(oldLeftRight->left->height ,  oldLeftRight->left->height)+1;


            return oldLeftRight ; // return new root of rotated subtree
        }



        //Returns new head of this subtree
        Node* insert(Node* New,size_type& mySize)
        {
            //Whether this pass through, or is inserted, it may be the next or prev neighbour




            //First, perform standard BST insertion
            if (New->value < value)
            {
                if (left==nullptr)
                {
                    left=New;
                    left->parent=this;
                    ++mySize;

                    //If this is inserted to our left, it is surely our new prev

                    Node* oldPrev = prev;
                    prev = New;

                    //Insert the New one in-between
                    New->next = this;
                    New->prev = oldPrev;
                    if(nullptr!=oldPrev) oldPrev->next = New;
                }
                else
                    left=left->insert(New,mySize);
                height=std::max(left->height+1,height);

            }
            else// if ((value<New->value)) put this back to not allow dublicate values
            {


                if (right==nullptr)
                {
                    right=New;
                    right->parent=this;
                    ++mySize;

                    Node* oldNext = next;
                    next = New;

                    //Insert the New one in-between
                    New->prev = this;
                    New->next = oldNext;
                    if (oldNext!=nullptr) oldNext->prev = New;
                }
                else
                    right = right->insert(New,mySize);
                height=std::max(right->height+1,height);


            }
            //else//Put this back to have dublicate values OVERWRITE existing
            //{
            //    value=New->value;
            //    delete New;
            //}

            size_type hr = (right == nullptr ? 0 : right->height);
            size_type hl = (left == nullptr ? 0 : left->height);

            //Really just hr-hl, but make sure we don't overflow, no risk from this compression since the height difference is NEVER more than 2
            balance_factor = hr<hl ? -static_cast<int>(hl-hr) : static_cast<int>(hr-hl);
            if (balance_factor<-1)
            {
                //If we get here, balance_factor=hr-hl <= -hl<=-2; so hl!=0 so left is surely not nullptr
                if (left->balance_factor>0)
                    return rotateLeftRight();
                else
                    return rotateRight();
            }
            else if (balance_factor>1)
            {
                //Same as before, we know right is not nullptr if we ever get here
                if (right->balance_factor<0)
                    return rotateRightLeft();
                else
                    return rotateLeft();
            }
            else
                return this;//balanced
        }

        #ifdef DEBUG_PRINT_ENABLE
        void DEBUG_print(int indent=0) const noexcept
        {
            std::cout<<std::string(indent,' ')<<"{ val="<<my_to_string(value)<<" height "<<height<<" balance : "<<balance_factor<<" parent "<<(parent==nullptr ?  "nullptr" : my_to_string(parent->value) )<<"\n"<<std::string(indent,' ')<<"left: ";
            if (left!=nullptr) {std::cout << "\n";left ->DEBUG_print(indent+4);}
            else std::cout << "null\n";
            std::cout<<std::string(indent,' ')<<"right: ";
            if (right!=nullptr) {std::cout << "\n";right->DEBUG_print(indent+4);}
            else std::cout << "null\n";

            std::cout<<std::string(indent,' ')<<'}'<<std::endl;
        }
        #endif
    };

    class my_iterator
    {
    public:
        //Look at design document for LaneContainer for exactly what requirements are needed
        using difference_type = std::ptrdiff_t;
        using value_type = T;//Not required since C++20
        using reference= T&;
        using const_reference= const T&;
        using pointer= T*;
        using const_pointer= const T*;
        using iterator_category= std::forward_iterator_tag;
    private:
        Node* myNode=nullptr;//This is used as Lane.end(), since that is what we get if we use one too many ++
    public:

        reference  operator*()
        {
            return (myNode->value);
        }

        const_reference operator*() const
        {
            return (myNode->value);
        }

        //UNTESTED
        pointer operator-> ()
        {
            return &(myNode->value);
        }

        //UNTESTED
        const_pointer operator-> () const
        {
            return &(myNode->value);
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
                    my_N=new Node(N->value);
                    Node_lookup[N]=my_N;
                }
                else
                    my_N=it->second;

                //Copy the value into this new Node
                my_N->value=N->value;
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
                        my_N->left=new Node(N->left->value);
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
                        my_N->right=new Node(N->right->value);
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
                        my_N->prev=new Node(N->prev->value);
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
                        my_N->next=new Node(N->next->value);
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
                        my_N->parent=new Node(N->parent->value);
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
        head = other.head;
        other.head=nullptr;
        least=other.least;
        other.least=nullptr;
        greatest=other.greatest;
        other.greatest=nullptr;
    }

    Lane& operator=(const Lane& other) noexcept//copy assignment
    {

        if (this!=&other)//This would result in a double-free
        {

            this->~Lane();

            mySize=other.mySize;

            if (this!=&other)
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
                            my_N=new Node(N->value);
                            Node_lookup[N]=my_N;
                        }
                        else
                            my_N=it->second;

                        //Copy the value into this new Node
                        my_N->value=N->value;
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
                                my_N->left=new Node(N->left->value);
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
                                my_N->right=new Node(N->right->value);
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
                                my_N->prev=new Node(N->prev->value);
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
                                my_N->next=new Node(N->next->value);
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
                                my_N->parent=new Node(N->parent->value);
                                Node_lookup[N->parent]=my_N->parent;
                            }
                            else
                                my_N->parent=it->second;
                        }
                    }
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
            return false;
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
            return (head==nullptr && b.head==nullptr) ||  (head!=nullptr && head->is_same_subtree(b.head));
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
            head     = new Node(std::move(t));
            least    = head;
            greatest = head;
            mySize = 1;
        }
        else
        {
            head = head->insert(std::move(t),least,greatest,mySize);

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
            head->DEBUG_print();

            std::cout<<"\nBy size: ";
            for (const Node* N = least; N!=nullptr; N=N->next)
                std::cout<<my_to_string(N->value)<<' ';
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
    static_assert(std::is_same<typename std::iterator_traits<Lane<T>::iterator>::pointer, T*>::value);
    static_assert(std::is_same<typename std::iterator_traits<Lane<T>::iterator>::pointer,T*>::value);
    static_assert(std::is_same<typename std::iterator_traits<Lane<T>::iterator>::iterator_category,std::forward_iterator_tag>::value);
};



