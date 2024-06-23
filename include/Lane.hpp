/*A Lane mainly functions as a single ended queue but random insertion and deletion is allowed, the elements T have a sorting operator against other T and against double pos, (the two ordering operators are transitive with each other)

Elements are looked up, based on either other elements, or the double they are equal to.

It is legal for elements to change what double they compare to, even outside control of the lane, but it is implicitly assumed that elements don't change their order

This implements the requirements for Container, but only SOME requirements for SequenceContainer, many things, such as insert, swap and emplace at an iterator does not make sense for a sorted container

It is not a fully fledged Associative container either ... it is doing its own thing


It is implemented as an AVL tree, with a double-linked-list connecting "adjacent" nodes
*/

#include <cstddef>//size_t
#include <list>
#include <ranges>


//C++ 20 feature, for ensuring that the element has the require operation
#include <concepts>

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


size_t loading_counter=0;


template<LaneElement T>
class Lane
{

public:
    using value_type      = std::list<T>::value_type;//T;
    using pointer_type    = T*;
    using size_type       = std::list<T>::size_type;//size_t;
    using difference_type = std::list<T>::difference_type;//size_t;
    using reference       = std::list<T>::reference;// T&;
    using const_reference = std::list<T>::const_reference;//const T&;

    //If we made everything from scratch, we would need to make our own iterator class
    using iterator       = std::list<T>::iterator;
    using const_iterator = std::list<T>::const_iterator;



//EVERYTHING BELOW ARE Requirements for CONTAINER (see https://en.cppreference.com/w/cpp/named_req/Container)
    Lane(){;//default constructor

    }

    Lane(const Lane& a)
    {
        elements=a.elements;//invokes copy constructor
    }

    Lane(Lane&& a)
    {
        elements=std::move(a.elements);
    }

    Lane& operator=(const Lane& other)//copy assignment
    {
        elements=other.elements;//invokes copy constructor
        return *this;
    }
    Lane& operator=(Lane&& other)//move assignment
    {
        elements=std::move(other.elements);
        return *this;
    }


    //destructor
    ~Lane()
    {
        delete head;
    }//elements.~list<T>() is called by default

    //iterator begin() {return elements.begin();}
    //iterator end()   {return elements.end();}

    //const const_iterator begin()const {return elements.begin();}
    //const const_iterator end()  const {return elements.end();}

    //three way comparison of iterator

    bool operator==(const Lane<T>& b) const
    {
        return true;
    }

    bool operator!=(const Lane<T>& b) const
    {
        return !(operator==(b));
    }

    void swap(Lane<T>& b)
    {
        elements.swap(b.elements);
    }

    static void swap(Lane<T>& a,Lane<T>& b)
    {
        a.swap(b);
    }

    size_type size()
    {
        return elements.size();
    }

    size_type max_size()
    {
        return elements.max_size();
    }
    bool empty()
    {
        return elements.empty();
    }

//We only implement SOME Requirements for SEQUENCE CONTAINER (see https://en.cppreference.com/w/cpp/named_req/SequenceContainer), it doesn't make sense to implement all

    Lane(size_type n, T t)
    {
        elements=std::list<T>(n,t);//Ok, they all have the same value, so this is sorted
    }

    //Accept any input iterators
    template<LegacyInputIterator Iter>
        Lane(Iter start, Iter end)
        {
            //elements=std::list<T>(start,end);//Ok, they all have the same value, so this is sorted
            std::copy(start,end,std::back_inserter(*this)); //This is O(N log N) the same as std::sort
        }
    //Full disclosure, I could not get enough documentation to implement Lane(std::from_range, rg), thus this is NOT C++23 compatible

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

    //Now the important ones, the actual insertions followed by sorting
    //I name it push_back, to get a working std::back_inserter
    void push_back(T t)
    {
        if (head==nullptr)
        {
            head     = new Node(std::move(t));
            least    = head;
            greatest = head;
        }
        else
        {
            head = head->insert(std::move(t),least,greatest);
        }

        std::cout<<"POST INSERT "<<t<<std::endl;
        DEBUG_print_tree();
    }

    void DEBUG_print_tree() const
    {
        if (head==nullptr)
            std::cout<<"Empty"<<std::endl;
        else
        {
            std::cout<<"By tree:\n";
            head->DEBUG_print();

            std::cout<<"\nBy size: ";
            for (const Node* N = least; N!=nullptr; N=N->next)
                std::cout<<N->value<<' ';
            std::cout<<std::endl;
        }
    }

private:

    //First version, we use a linked list behind the scenes, while enforcing a tree-structure for access and insertion
    //The double linked list has easy access to next and previous elements, and a working iterator implementation
    std::list<T> elements;

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
            ++loading_counter;
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

            --loading_counter;
        }


        Node* insert(T&& val,Node*& least, Node*& greatest)
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



            return insert(New);
        }

        //Returns new head of this subtree
        //ASSUMES right is not nullptr and has balance factor 1, otherwise this would not be called
        Node* rotateLeft()
        {
            std::cout<<"R ON "<<value<<std::endl;
            Node* oldRight = right;

            Node* oldRightLeft = right->left;//may be null, doesn't matter

            //Now that is my right
            right=oldRightLeft;
            if (oldRightLeft!=nullptr)
                oldRightLeft->parent=oldRight;

            //Move this onto old right
            oldRight->left = this;
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
            std::cout<<"R ON "<<value<<std::endl;
            Node* oldLeft = left;

            Node* oldLeftRight = left->right;//may be null, doesn't matter

            //Now that is my left
            left=oldLeftRight;
            if (oldLeftRight!=nullptr)
                oldLeftRight->parent=oldLeft;

            //Move this onto old left
            oldLeft->right = this;
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
            std::cout<<"RL ON "<<value<<std::endl;
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
            std::cout<<"LR ON "<<value<<std::endl;
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
        Node* insert(Node* New)
        {
            //Whether this pass through, or is inserted, it may be the next or prev neighbour




            //First, perform standard BST insertion
            if (New->value < value)
            {
                //pass through or insert here, if this is the closest lesser number it is my prev and I am your next
                if (prev ==nullptr || New->value>prev->value)
                {


                    Node* oldPrev = prev;
                    prev = New;

                    //Insert the New one in-between
                    New->next = this;
                    New->prev = oldPrev;
                }

                if (left==nullptr)
                {
                    left=New;
                    left->parent=this;
                }
                else
                    left=left->insert(New);
                height=std::max(left->height+1,height);

            }
            else
            {
                if (next ==nullptr || New->value < next->value)
                {


                    Node* oldNext = next;
                    next = New;

                    //Insert the New one in-between
                    New->prev = this;
                    New->next = oldNext;
                }


                if (right==nullptr)
                {
                    right=New;
                    right->parent=this;
                }
                else
                    right = right->insert(New);
                height=std::max(right->height+1,height);


            }

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


        void DEBUG_print(int indent=0) const
        {
            std::cout<<std::string(indent,' ')<<"{ val="<<value<<" height "<<height<<" balance : "<<balance_factor<<"\n"<<std::string(indent,' ')<<"left: ";
            if (left!=nullptr) {std::cout << "\n";left ->DEBUG_print(indent+4);}
            else std::cout << "null\n";
            std::cout<<std::string(indent,' ')<<"right: ";
            if (right!=nullptr) {std::cout << "\n";right->DEBUG_print(indent+4);}
            else std::cout << "null\n";

            std::cout<<std::string(indent,' ')<<'}'<<std::endl;
        }
    };

    //owned
    Node* head    =nullptr;

    //Non-owned
    Node* least   =nullptr;
    Node* greatest=nullptr;
};
