#include "Lane.hpp"


//This class has NO public members!
//It was supposed to be an internal class in Lane<T>, but for reasons described in the design document for Lane and AVL_Node it was split up into its own class library
//By marking every member as private, and making the Lane<T> the only friend class, we can get the same effect


AVL_Node::AVL_Node(std::shared_ptr<void> val,compare less, compare equal,AVL_Node* p): val_less(less), val_equal(equal)
{

    height=1;
    pvalue = val;
    left=nullptr;
    right=nullptr;
    balance_factor=0;

    next=nullptr;
    prev=nullptr;
    parent=p;
}

AVL_Node::~AVL_Node()
{
    //Already checks null
    delete left;
    delete right;
}

//Am I the same as this? in terms of subtree content and structure
bool AVL_Node::is_same_subtree(const AVL_Node* that)const
{
    //Short circuit logic = We make the most trivial comparisons first, if they fail we skip expensive calculations
    return that!=nullptr && val_equal(that->pvalue,pvalue)//Same value, and that thing is not broken
     && ((left==nullptr && that->left==nullptr) || (left!=nullptr && that->left !=nullptr && left->is_same_subtree(that->left)))//Left side is the same (both null, or the subtree is the same)
    && ((right==nullptr && that->right==nullptr) || (right!=nullptr && that->right !=nullptr && right->is_same_subtree(that->right)));//Same with the right side
    //Do not make the same kind of search of the next and previous, if the tree looks the same they are the same

}


AVL_Node* AVL_Node::insert(std::shared_ptr<void> val,AVL_Node*& least, AVL_Node*& greatest,size_type& mySize)
{
    AVL_Node* New = new AVL_Node(val,val_less,val_equal,this);
    if (least==nullptr   ||  val_less((New->pvalue),(least->pvalue)) )
    {
        least=New;
    }
    if (greatest==nullptr|| val_less((greatest->pvalue),(New->pvalue)))
    {
        greatest=New;
    }



    return insert(New,mySize);
}

//Returns new head of this subtree
//ASSUMES right is not nullptr and has balance factor 1, otherwise this would not be called
AVL_Node* AVL_Node::rotateLeft()
{
    AVL_Node* oldRight = right;

    AVL_Node* oldRightLeft = right->left;//may be null, doesn't matter

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
AVL_Node* AVL_Node::rotateRight()
{
    AVL_Node* oldLeft = left;

    AVL_Node* oldLeftRight = left->right;//may be null, doesn't matter

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
AVL_Node* AVL_Node::rotateRightLeft()
{
    //If we get here this is valid
    AVL_Node* oldRight     = right;
    AVL_Node* oldRightLeft = right->left;


    // if we get here, oldRightLeft is not null, but its children might be
    AVL_Node* oldRightLeftRight = oldRightLeft->right;

    oldRight->left = oldRightLeftRight;
    if (oldRightLeftRight != nullptr)
        oldRightLeftRight ->parent = oldRight;
    oldRightLeft -> right = oldRight;
    oldRight->parent = oldRightLeft ;

    AVL_Node* oldRightLeftLeft = oldRightLeft->left;
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
AVL_Node* AVL_Node::rotateLeftRight()
{
    //If we get here this is valid
    AVL_Node* oldLeft     = left;
    AVL_Node* oldLeftRight = left->right;

    // if we get here, oldLeftRight is not null, but its children might be
    AVL_Node* oldLeftRightLeft = oldLeftRight->left;

    oldLeft->right = oldLeftRightLeft;
    if (oldLeftRightLeft != nullptr)
        oldLeftRightLeft ->parent = oldLeft;
    oldLeftRight -> left = oldLeft;
    oldLeft->parent = oldLeftRight ;

    AVL_Node* oldLeftRightRight = oldLeftRight->right;
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
AVL_Node* AVL_Node::insert(AVL_Node* New,size_type& mySize)
{
    //Whether this pass through, or is inserted, it may be the next or prev neighbour




    //First, perform standard BST insertion
    if (val_less(New->pvalue,pvalue))
    {
        if (left==nullptr)
        {
            left=New;
            left->parent=this;
            ++mySize;

            //If this is inserted to our left, it is surely our new prev

            AVL_Node* oldPrev = prev;
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

            AVL_Node* oldNext = next;
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


void AVL_Node::DEBUG_print(int indent, std::function<std::string(const std::shared_ptr<void>&)> printer ) const noexcept
{
    std::cout<<std::string(indent,' ')<<"{ val="<<printer(pvalue)<<" height "<<height<<" balance : "<<balance_factor<<" parent "<<(parent==nullptr ?  "nullptr" : printer(parent->pvalue) )<<"\n"<<std::string(indent,' ')<<"left: ";
    if (left!=nullptr) {std::cout << "\n";left ->DEBUG_print(indent+4,printer);}
    else std::cout << "null\n";
    std::cout<<std::string(indent,' ')<<"right: ";
    if (right!=nullptr) {std::cout << "\n";right->DEBUG_print(indent+4,printer);}
    else std::cout << "null\n";

    std::cout<<std::string(indent,' ')<<'}'<<std::endl;
}
