#include <iostream>
#include <set>

// Assuming class T has the required comparison operators
class T {
public:
    double value;
    T(double v) : value(v)
    {
        std::cout<<"CALLED T CONSTRUCTOR WITH VALUE "<<v<<std::endl;
    }

    bool operator<(const T& other) const {
        return value < other.value;
    }

    bool operator<(double other) const {
        return value < other;
    }
};

// Comparator functor
struct Comparator {
    bool operator()(const T& lhs, const T& rhs) const {
        return lhs < rhs;
    }
};

int main() {
    std::set<T, Comparator> MySet;
    MySet.insert(T(1.5));
    MySet.insert(T(3.7));
    MySet.insert(T(5.2));
    MySet.insert(T(7.8));

    double threshold = 4.0;

    auto it = MySet.upper_bound(threshold);

    if (it != MySet.begin()) {
        --it; // move iterator to the largest element less than the threshold
        std::cout << "The largest element less than " << threshold << " is: " << it->value << std::endl;
    } else {
        std::cout << "No element less than " << threshold << " found." << std::endl;
    }

    return 0;
}
