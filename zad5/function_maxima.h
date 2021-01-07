#ifndef FUNCTION_MAXIMA_H
#define FUNCTION_MAXIMA_H

#include <set>
#include <memory>
#include <array>

class InvalidArg: public std::exception{};

template <typename A, typename V>
class FunctionMaxima{
    public:
    // Typ point_type umożliwiający dostęp do "punktów" funkcji
    class point_type;

    private:
    class MaximaComparator;
    class PointArgComparator;
    using PointSet = std::set<point_type, PointArgComparator>;
    using MaxSet = std::set<point_type, MaximaComparator>;

    public:

    // Typ iterujący po punktach funkcji.
    using iterator = typename PointSet::const_iterator;

    // Typ iterujący po lokalnych maksimach funkcji.
    using mx_iterator = typename MaxSet::const_iterator;



    // Tworzy funkcję o pustej dziedzinie
    FunctionMaxima() = default;

    // Konstruktor kopiujący
    FunctionMaxima(const FunctionMaxima& other) = default;

    // Operator przypisania
    FunctionMaxima& operator=(FunctionMaxima other);



    // Zwraca wartość w punkcie a, rzuca wyjątek InvalidArg, jeśli a nie
    // należy do dziedziny funkcji. Złożoność najwyżej O(log n).
    const V& value_at(const A& a) const;

    // Zmienia funkcję tak, żeby zachodziło f(a) = v. Jeśli a nie należy do
    // obecnej dziedziny funkcji, jest do niej dodawany. Najwyżej O(log n).
    void set_value(const A& a, const V& v);

    // Usuwa a z dziedziny funkcji. Jeśli a nie należało do dziedziny funkcji,
    // nie dzieje się nic. Złożoność najwyżej O(log n).
    void erase(const A& a);



    // iterator wskazujący na pierwszy punkt
    iterator begin() const noexcept;

    // iterator wskazujący za ostatni punkt
    iterator end() const noexcept;

    // Iterator, który wskazuje na punkt funkcji o argumencie a lub end(),
    // jeśli takiego argumentu nie ma w dziedzinie funkcji.
    iterator find(const A& a) const;

    // iterator wskazujący na pierwsze lokalne maksimum
    mx_iterator mx_begin() const noexcept;

    // iterator wskazujący za ostatnie lokalne maksimum
    mx_iterator mx_end() const noexcept;



    using size_type = typename PointSet::size_type;

    size_type size() const noexcept;
    


    private:
    MaxSet maxSet;
    PointSet pointSet;


    bool isMaximum(iterator point) const;
    bool isMaximumLeftGap(iterator point) const;
    bool isMaximumRightGap(iterator point) const;
    void swap(FunctionMaxima& other) noexcept;

    template <typename SetType, std::size_t maxOps>
    class SetGuard;
};


template <typename A, typename V>
class FunctionMaxima<A, V>::point_type{
    // To save space we are storing pointers instead of plain A and V.
    // Since A and V once stored inside our class are very much constant
    // we can safely share them between multiple copies of FunctionMaxima.
    // To simplify management we of course use shared pointers.

    // Note:
    // We could bring space saving to the extreme by holding static sets of As and Vs
    // and only using pointers to these statically held values. This would completely
    // eliminate A and V duplication across all instances of FunctionMaxima.
    // This however saves space only with big As and Vs and carries slight performance overhead
    // (log(n) where n is maximum from a total number of unique As or Vs). Aiming for more
    // general use we provide some memory optimizations while ensuring performance is dependent
    // only on local instance size.

    friend class FunctionMaxima;
    std::shared_ptr<A> a;
    std::shared_ptr<V> v;

    // Creates point_type by making copies of A and V.
    point_type(const A& arg, const V& val){
        a = std::make_shared<A>(arg);
        v = std::make_shared<V>(val);
    }

    class PointValueGuard{
        public:
        bool reverse = false;
        point_type* point;  // Raw pointer because we will not be able to initialize a reference in default constructor
        std::shared_ptr<V> oldValue;

        PointValueGuard() {};

        ~PointValueGuard(){
            if(reverse){
                point->v = oldValue;
            }
        }

        void commit(){
            reverse = false;
        }
    };

    // Changes value in point to copy of given val.
    void setValue(PointValueGuard& guard, const V& val){
        guard.reverse = true;
        guard.point = this;
        guard.oldValue = v;

        v = std::make_shared<V>(val);
    }

    public:

    // Zwraca argument funkcji.
    const A& arg() const{
        return *a;
    }

    // Zwraca wartość funkcji w tym punkcie.
    const V& value() const{
        return *v;
    }
};

// Utility class used for providing transactionality for groups of set operations.
// Uses RAII to provide its functionality.
template <typename A, typename V>
template <typename SetType, std::size_t maxOps>
class FunctionMaxima<A, V>::SetGuard{
    SetType& set;

    class SetOp{
        public:
        SetType* set;   // "Ugly" pointer. No need for any smart pointer, and reference does not necesarilly work,
                        // since it would have to be initialized immediatelly, which could possibly be wasteful.
        bool erase = false;

        void doneInsert(){
            erase = false;
        }

        void doneErase(){
            erase = (place != set->end());
        }

        void (SetOp::*commit)();
        typename SetType::iterator place;
                
        ~SetOp(){
            if(erase){
                set->erase(place); // noexcept
            }
        }
    };
    
    std::array<SetOp, maxOps> operations = {};
    std::size_t currOp = 0;

    SetOp& getOp(){
        SetOp& op = operations[currOp++];
        op.set = &set;
        return op;
    }

    public:

    SetGuard(SetType& set): set(set){};

    void insert(const typename SetType::value_type& p){
        SetOp& op = getOp();
        auto ret = set.insert(p);
        op.place = ret.first;
        op.erase = ret.second;
        op.commit = &SetOp::doneInsert;
    }

    void insert(typename SetType::iterator pos, typename SetType::value_type&& p){
        SetOp& op = getOp();
        op.place = set.insert(pos, std::move(p));
        op.erase = true;
        op.commit = &SetOp::doneInsert;
    }

    template<typename... Args>
    void emplace_hint(typename SetType::iterator pos, Args&&... args){
        SetOp& op = getOp();
        op.place = set.emplace_hint(pos, std::forward<Args>(args)...);
        op.erase = true;
        op.commit = &SetOp::doneInsert;
    }

    template<typename... Args>
    void emplace(Args&&... args){
        SetOp& op = getOp();
        op.place = set.emplace(std::forward<Args>(args)...);
        op.erase = true;
        op.commit = &SetOp::doneInsert;
    }

    void erase(const typename SetType::value_type& p){
        SetOp& op = getOp();
        op.place = set.find(p);
        op.commit = &SetOp::doneErase;
    }

    void erase(const typename SetType::iterator pos){
        SetOp& op = getOp();
        op.place = pos;
        op.commit = &SetOp::doneErase;
    }

    void commit(){
        for(std::size_t i = 0; i < currOp; i++){
            SetOp& op = operations[i];
            (op.*(op.commit))();
        }
    }
};




// Compares point_type objects providing desired maxima ordering
template <typename A, typename V>
class FunctionMaxima<A, V>::MaximaComparator{
    public:
    bool operator()(const point_type& a, const point_type& b) const{
        if(a.value() < b.value()){
            return false;
        }
        if(b.value() < a.value()){
            return true;
        }
        if(a.arg() < b.arg()){
            return true;
        }
        return false;
    };
};

// Compares point_type by argument values
template <typename A, typename V>
class FunctionMaxima<A, V>::PointArgComparator{
    public:
    // This allows us to define additional comparators used by find(), lower_bound() etc.
    // Thanks to that we can search set<point_type> without providing fake point_type - just A is enough
    using is_transparent = void;
    bool operator()(const point_type& p, const point_type& q) const{
        return p.arg() < q.arg();
    }
    bool operator()(const point_type& p, const A& a) const{
        return p.arg() < a;
    }
    bool operator()(const A& a, const point_type& p) const{
        return a < p.arg();
    }
};




// Checks whether function has maximum in point pointed to by iterator
template <typename A, typename V>
bool FunctionMaxima<A, V>::isMaximum(iterator point) const{
    const V& v2 = point->value();
    if(point != begin()){
        --point;
        const V& v1 = point->value();
        if(v2 < v1){
            return false;
        }
        ++point;
    }
    ++point;
    if(point != end()){
        const V& v3 = point->value();
        if(v2 < v3){
            return false;
        }
    }
    return true;
}

// Checks whether function will have maximum in point pointed to by iterator
// after erasing its left neighbour
template <typename A, typename V>
bool FunctionMaxima<A, V>::isMaximumLeftGap(iterator point) const{
    const V& v2 = point->value();
    --point;
    if(point != begin()){
        --point;
        const V& v1 = point->value();
        if(v2 < v1){
            return false;
        }
        ++point;
    }
    ++point;
    ++point;
    if(point != end()){
        const V& v3 = point->value();
        if(v2 < v3){
            return false;
        }
    }
    return true;
}

// Checks whether function will have maximum in point pointed to by iterator
// after erasing its right neighbour
template <typename A, typename V>
bool FunctionMaxima<A, V>::isMaximumRightGap(iterator point) const{
    const V& v2 = point->value();
    if(point != begin()){
        --point;
        const V& v1 = point->value();
        if(v2 < v1){
            return false;
        }
        ++point;
    }
    ++point;
    ++point;
    if(point != end()){
        const V& v3 = point->value();
        if(v2 < v3){
            return false;
        }
    }
    return true;
}

template <typename A, typename V>
void FunctionMaxima<A, V>::swap(FunctionMaxima& other) noexcept{
    using std::swap;
    swap(this->pointSet, other.pointSet);
    swap(this->maxSet, other.maxSet);
}

template <typename A, typename V>
FunctionMaxima<A, V>& FunctionMaxima<A, V>::operator=(FunctionMaxima other){
    this->swap(other);
    return *this;
}

template <typename A, typename V>
const V& FunctionMaxima<A, V>::value_at(const A& a) const{
    iterator pointIt = find(a);
    if(pointIt == end()){
        throw InvalidArg();
    }
    return pointIt->value();
}

template <typename A, typename V>
void FunctionMaxima<A, V>::set_value(const A& a, const V& v){
    // Strong exception safety provided by SetGuard and PointValueGuard
    SetGuard<PointSet, 1> pointSetGuard = {pointSet};
    SetGuard<MaxSet, 4> maxSetGuard = {maxSet};
    typename point_type::PointValueGuard valueGuard;


    // Find place where point should be
    iterator it = pointSet.lower_bound(a);

    // If it's not there
    if(it == pointSet.end() || pointSet.key_comp()(a, *it)){
        // Create new point_type and move it there (can't use emplace because of private constructor)
        // There are hacks possible to make emplace work, but point_type is quite light so moving it is ok.
        pointSetGuard.insert(it, {a, v});
        it--;  // iterator now points to inserted element
    } else if (!((v < it->value()) || (it->value() < v))){
        // If point exists and updated value is the same return early. This fixes incorrect behaviour
        // caused by fact that SetGuard does not perform operations immediatelly, so erase and insert on
        // the same element actually deletes that element.
        return;
    } else {
        // Otherwise remove point from maxSet (as that pair arg-val no longer exists).
        // If point was not a maximum this has no effect (except it might throw on comparison).
        maxSetGuard.erase(*it);

        // Update point with new value. Unfortunatelly std::se::iterator is always const
        // (because altering elements could violate ordering) so we have to use const_cast.
        // This is safe because pointSet ordering does not depend on points' values.
        // Doing that prevents making redundant copies/moves of point_type objects.
        // We could also declare point_type::v mutable but that wouldn't be optimal since
        // maxSet's ordering does depend on values.
        const_cast<point_type&>(*it).setValue(valueGuard, v);
    }

    // If point is a maximum insert it's copy into maxSet (if it was a maximum but no longer is
    // an old copy had been already removed).
    if(isMaximum(it)){
        maxSetGuard.insert(*it);
    }

    if(it != pointSet.begin()){
        it--;
        if(isMaximum(it)){
            // Same for left neighbour
            maxSetGuard.insert(*it);
        } else {
            // But if it just stopped being maximum we have to remove it.
            // If a point wasn't a maximum before this has no effect.
            // We could optimize this by remembering if a point was a maximum
            // before updating {a, v} but that would clutter code with even more
            // execution branches.
            maxSetGuard.erase(*it);
        }
        it++;
    }

    // Same for right neighbour
    it++;
    if(it != pointSet.end()){
        if(isMaximum(it)){
            maxSetGuard.insert(*it);
        } else {
            maxSetGuard.erase(*it);
        }
    }

    pointSetGuard.commit();
    maxSetGuard.commit();
    valueGuard.commit();
}

template <typename A, typename V>
void FunctionMaxima<A, V>::erase(const A& a){
    // Strong exception safety provided by SetGuard
    SetGuard<PointSet, 1> pointSetGuard = {pointSet};
    SetGuard<MaxSet, 4> maxSetGuard = {maxSet};

    // Find the point
    iterator it = pointSet.find(a);

    // Proceed only when it exists
    if(it != pointSet.end()){

        // Remove it from maxSet and pointSet (it will be deleted on commit
        // so we treat it as it's still there)
        maxSetGuard.erase(*it);
        pointSetGuard.erase(it);

        // Check if left neighbour became a maximum
        if(it != pointSet.begin()){
            it--;
            if(isMaximumRightGap(it)){
                maxSetGuard.insert(*it);
            } else {
                maxSetGuard.erase(*it);
            }
            it++;
        }

        // Same for right neighbour
        it++;
        if(it != pointSet.end()){
            if(isMaximumLeftGap(it)){
                maxSetGuard.insert(*it);
            } else {
                maxSetGuard.erase(*it);
            }
        }
    }

    pointSetGuard.commit();
    maxSetGuard.commit();
    
}

template <typename A, typename V>
typename FunctionMaxima<A, V>::iterator FunctionMaxima<A, V>::begin() const noexcept{
    return pointSet.cbegin();
}

template <typename A, typename V>
typename FunctionMaxima<A, V>::iterator FunctionMaxima<A, V>::end() const noexcept{
    return pointSet.cend();
}

template <typename A, typename V>
typename FunctionMaxima<A, V>::iterator FunctionMaxima<A, V>::find(const A& a) const{
    // set::iterator is implicitly convertible to const_iterator
    // This is nothrow as long as comparator doesn't throw, otherwise it's of course strong safety.
    return pointSet.find(a);
}

template <typename A, typename V>
typename FunctionMaxima<A, V>::mx_iterator FunctionMaxima<A, V>::mx_begin() const noexcept{
    return maxSet.cbegin();
}

template <typename A, typename V>
typename FunctionMaxima<A, V>::mx_iterator FunctionMaxima<A, V>::mx_end() const noexcept{
    return maxSet.cend();
}

template <typename A, typename V>
typename FunctionMaxima<A, V>::size_type FunctionMaxima<A, V>::size() const noexcept{
    return pointSet.size();
}

#endif /* FUNCTION_MAXIMA_H */