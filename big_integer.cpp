#include <string>
#include "big_integer.h"

#define notation 4294967296

struct tmpKeeper{
    tmpKeeper(size_t size){
        uint32_t *tmp = new uint32_t[size];
        data_=tmp;
    }
    void put(uint32_t a, size_t place){
        data_[place]=a;
    }
    void clear(size_t size){
        for (size_t i=0; i< size;i++){
            data_[i]=0;
        }
    }
    uint32_t get (size_t place){
        return data_[place];
    }
    ~tmpKeeper(){
        delete[] data_;
    }

    void swap (tmpKeeper& x){
        std::swap(x.data_,data_);
    }
    void swap (uint32_t * & x){
        std::swap(x,data_);
    }
    void inverse(size_t size){
        for (size_t i=0;i<size;i++){
            data_[i]=~data_[i];
        }
        uint64_t carry = 1;
        for (size_t i = 0; i < size; i++) {
            carry = carry + data_[i];
            data_[i] = carry % notation;
            carry = carry / notation;
        }
    }

private:
    uint32_t* data_;
};
void mmemcpy(uint32_t * &to, uint32_t const *from, size_t size) {
    for (size_t i = 0; i < size; i++)
        to[i] = from[i];
}

big_integer::big_integer(uint32_t *buf, size_t size, bool sign) {
    uint32_t *tmp = new uint32_t[size];
    mmemcpy(tmp, buf, size);
    data_=tmp;
    size_ = size;
    sign_ = sign;
}//

big_integer &big_integer::dangerAdd(big_integer const &b) {//должно быть не меньше по модулю, знаки равны
    size_t tmpSize = this->size();
    tmpKeeper t(tmpSize);
    t.clear(tmpSize);
    int64_t carry = 0,tmpA,tmpB;
    for (size_t i = 0; i < b.size(); i++) {
        tmpA = this->data_[i];
        tmpB = b.data_[i];
        carry = carry + tmpA + tmpB;
        t.put(carry % notation,i);
        carry = carry / notation;
    }

    for (size_t i = b.size(); i < tmpSize; i++) {
        tmpA =this->data_[i];
        carry+=tmpA;
        t.put( carry% notation,i);
        carry = carry/ notation;
    }

    if (carry > 0) {
        tmpKeeper t1(tmpSize+1);
        for (size_t i = 0; i < tmpSize; i++)
            t1.put(t.get(i),i);
        t1.put(carry,tmpSize);
        tmpSize++;
        t.swap(t1);

    }
    t.swap(this->data_);
    this->size_=tmpSize;
    this->sign_=false;
    return (*this);
}//

big_integer &big_integer::dangerSub(big_integer const &b) {
    size_t tmpSize = this->size();
    tmpKeeper t(tmpSize);//uint32_t *tmp = new uint32_t[tmpSize];
    t.clear(tmpSize);
    int64_t  carry = 0, tmpA,tmpB;
    for (size_t i = 0; i < b.size(); i++) {
        tmpA =this->data_[i];
        tmpB= b.data_[i];
        carry= carry + tmpA-tmpB;
        if (carry<0){
            t.put((carry+notation)% notation,i);
            carry = carry/ notation-1;
        }
        else{
            t.put(carry% notation,i);
            carry = carry/ notation;
        }
    }

    for (size_t i = b.size(); i < tmpSize; i++) {
        tmpA =this->data_[i];
        carry+=tmpA;
        if (carry<0){
            t.put((carry+notation)% notation,i);
            carry = carry/ notation-1;
        }else{
            t.put(carry  % notation,i);
            carry = carry/ notation;
        }
    }
    size_t newSize=0;
    for (size_t i=0; i<tmpSize; i++){
        if (t.get(i)!=0)
            newSize=i;
    }
    newSize++;
    if (newSize != this->size()) {
        tmpKeeper t1(newSize);//uint32_t *tmpTmp = new uint32_t[newSize];
        for (size_t i = 0; i < newSize; i++)
            t1.put(t.get(i),i);
        t.swap(t1);
    }
    t.swap(this->data_);
    this->size_=newSize;
    this->sign_=false;
    return (*this);
}//

void big_integer::negate() {
    sign_ = !sign_;
}//

big_integer big_integer::negate(big_integer const &a) const {
    big_integer safe = a;
    safe.negate();
    return safe;
}//

size_t big_integer::size() const {
    return size_;
}//

bool big_integer::sign() const {
    return sign_;
}//
void big_integer:: swap(big_integer& a, big_integer& b){
    std::swap(a.data_, b.data_);
    std::swap(a.sign_, b.sign_);
    std::swap(a.size_, b.size_);
}
big_integer::big_integer() {
    uint32_t *tmp = new uint32_t[1];
    tmp[0] = 0;
    data_ = tmp;
    size_ = 1;
    sign_ = false;
}
big_integer::big_integer(int a) {
    int64_t tmp = a;
    sign_ = false;
    if (tmp < 0) {
        sign_ = true;
        tmp *= (-1);
    }
    if (tmp >= notation) {
        size_ = 2;
        uint32_t *tmp2 = new uint32_t[size_];
        tmp2[0] = tmp % notation;
        tmp2[1] = tmp / notation;
        data_ = tmp2;
    } else {
        size_ = 1;
        uint32_t *tmp2 = new uint32_t[size_];
        tmp2[0] = tmp;
        data_ = tmp2;
    }
}
big_integer::big_integer(uint64_t a) {
    uint64_t tmp = a;
    sign_ = false;
    if (tmp >= notation) {
        size_ = 2;
        uint32_t *tmp2 = new uint32_t[size_];
        tmp2[0] = tmp % notation;
        tmp2[1] = tmp / notation;
        data_ = tmp2;
    } else {
        size_ = 1;
        uint32_t *tmp2 = new uint32_t[size_];
        tmp2[0] = tmp;
        data_ = tmp2;
    }
}
big_integer::big_integer(big_integer const &other) {
    size_ = other.size_;
    sign_ = other.sign_;
    uint32_t * tmp = new uint32_t[size_];
    mmemcpy(tmp, other.data_, size_);
    data_=tmp;
}
big_integer::big_integer(std::string const &str) {
    big_integer other;
    int mul = 1;
    size_t begin=0;
    if (str[0] == '-') {
        mul=-1;
        begin = 1;
    }
    for (size_t i = begin; i < str.size(); i++) {
        other *= 10;
        other += ((str[i] - '0') * mul);
    }
    size_ = other.size_;
    sign_ = other.sign_;
    uint32_t * tmp = new uint32_t[size_];
    mmemcpy(tmp, other.data_, size_);
    data_=tmp;
}

big_integer::~big_integer() {
    delete[] data_;
}

big_integer &big_integer::operator=(big_integer const &other) {
    big_integer tmp(other);
    swap(tmp,*this);
    return *this;
}
big_integer &big_integer::operator++() {
    return *this += 1;
}
big_integer &big_integer::operator--() {
    return *this -= 1;
}
big_integer big_integer::operator++(int) {
    big_integer r = *this;
    ++*this;
    return r;
}
big_integer big_integer::operator--(int) {
    big_integer r = *this;
    --*this;
    return r;
}//


big_integer &big_integer::operator+=(big_integer const &rhs) {
    big_integer safe=rhs;
    if (this->sign_  == rhs.sign()) {
        if ((rhs>(*this))==!rhs.sign()){
            swap(safe,(*this));
        }
        (*this)=dangerAdd(safe);
        this->sign_ = safe.sign_ ;
        return (*this);
    }
    else {
        safe = negate(rhs);
        if ((*this > safe) == !sign()) {
            bool tmpSign = (*this).sign_;
            (*this) = dangerSub(safe);
            (*this).sign_ = tmpSign;
        } else {
            swap(safe, (*this));
            bool tmpSign = (*this).sign_;
            (*this) = dangerSub(safe);
            (*this).sign_ = !tmpSign;
        }
        return (*this);
    }
}
big_integer &big_integer::operator-=(big_integer const &rhs) {
    return (*this) += negate(rhs);
}//

big_integer big_integer::operator+() const {
    return *this;
}
big_integer big_integer::operator-() const {
    return negate(*this);
}
big_integer big_integer::operator~() const {
    big_integer answer(*this);
    answer.negate();
    answer--;
    return answer;
}//


big_integer &big_integer::operator*=(big_integer const &rhs) {
    size_t tmpSize = this->size() + rhs.size() + 1;
    tmpKeeper t(tmpSize);//uint32_t *tmp = new uint32_t[tmpSize];
    t.clear(tmpSize);
    size_t k=0;
    uint64_t carry, tmpData1, tmpData2;
    for (size_t i = 0; i < this->size(); i++) {
        carry = 0;
        for (size_t j = 0; j < rhs.size(); j++) {
            tmpData1 = data_[i];
            tmpData2 = rhs.data_[j];
            carry = carry + (uint64_t)t.get(i+j) +(tmpData1 * tmpData2)%notation;
            t.put(carry % notation,i + j);
            carry = carry/notation + (tmpData1 * tmpData2)/notation;
        }
        k=0;
        while (carry!=0){
            carry=carry+(uint64_t)t.get(i+rhs.size()+k);
            t.put(carry%notation,i+rhs.size()+k);
            carry=carry/notation;
            k++;
        }
    }
    size_t i = 0, newSize = 0;
    while (i < tmpSize) {
        if (t.get(i) != 0)
            newSize = i;
        i++;
    }
    newSize++;

    if (newSize!=tmpSize){
        tmpKeeper t1(newSize);//uint32_t * realTmp = new uint32_t[newSize];
        for (size_t i = 0; i < newSize; i++)
            t1.put(t.get(i),i);//mmemcpy(realTmp, tmp, newSize);
        t.swap(t1);
    }
    t.swap(this->data_);
    (*this).size_ = newSize;
    (*this).sign_ = (*this).sign_ != rhs.sign();
    return *this;
}//

void big_integer::upgrade(big_integer &x, size_t size) {
    tmpKeeper t(x.size() + size);//uint32_t *tmp = new uint32_t[x.size() + size];
    for (size_t i = 0; i < x.size() + size; i++) {
        if (i < size)
            t.put(0,i);
        else
            t.put(x.data_[i - size],i);
    }
    t.swap(x.data_);
    x.size_=x.size()+size;
}

big_integer &big_integer::operator/=(big_integer const &rhs) {
    size_t newSize = this->size() - rhs.size() + 1;
    uint32_t *tmp = new uint32_t[newSize];

    bool tmpSign = this->sign()!=rhs.sign();
    this->sign_=false;
    big_integer rhsSave(rhs);
    rhsSave.sign_=false;

    big_integer div;
    for (size_t i = 0; i < newSize; i++) {
        uint64_t l = 0;
        uint64_t r = notation+1;
        uint64_t m;
        while (l + 1 < r) {
            m = (l + r) / 2;
            div = rhsSave * m;
            upgrade(div, newSize - i - 1);
            if (div <= (*this))
                l = m;
            else
                r = m;
        }
        div = (rhsSave * l);
        upgrade(div, newSize -i - 1);
        (*this) -= div;
        tmp[i] = l;
    }
    int32_t i = newSize-1;
    int32_t newSizeSh = 1;
    while (i >=0) {
        if (tmp[i] != 0) {
            newSizeSh = newSize-i;
        }
        i--;
    }
    uint32_t *tmp1 = new uint32_t[newSizeSh];
    for (int32_t j = newSize; j > newSize-newSizeSh; j--)
        tmp1[newSize - j ] = tmp[j-1];
    delete[] tmp;
    this->sign_=tmpSign;
    std::swap(this->data_, tmp1);
    this->size_ = newSizeSh;
    delete[] tmp1;
    return (*this);
}//
big_integer &big_integer::operator%=(big_integer const &rhs) {
    bool sign = this->sign();
    *this -= (*this / rhs) * rhs;
    this->sign_ = sign;
    return *this;
}


bool operator==(big_integer const &a, big_integer const &b) {
    if (a.sign() != b.sign()) {
        if ((a.size() == 1 && a.data_[0] == 0) && (b.size() == 1 && b.data_[0] == 0))
            return true;
        return false;
    }
    if (a.size() != b.size())
        return false;
    for (size_t i = 0; i != a.size(); i++)
        if (a.data_[i] != b.data_[i])
            return false;
    return true;
}
bool operator!=(big_integer const &a, big_integer const &b) {
    return !(a == b);
}
bool operator<(big_integer const &a, big_integer const &b) {
    if (a.sign() != b.sign())
        return a.sign() > b.sign();
    if (a.size() != b.size())
        return (a.size() < b.size())!=a.sign();
    for (size_t i = a.size(); i != 0; i--)
        if (a.data_[i-1] != b.data_[i-1])
            return (a.data_[i-1] < b.data_[i-1]) != a.sign();
    return false;
}
bool operator>=(big_integer const &a, big_integer const &b) {
    return !(a < b);
}
bool operator>(big_integer const &a, big_integer const &b) {
    return (a >= b) && !(a == b);
}
bool operator<=(big_integer const &a, big_integer const &b) {
    return !(a > b);
}//

/*void big_integer::inverse (uint32_t* x, size_t size){
    for (size_t i=0;i<size;i++){
        x[i]=~x[i];
    }
        uint64_t carry = 1;
        for (size_t i = 0; i < size; i++) {
            carry = carry + x[i];
            x[i] = carry % notation;
            carry = carry / notation;
    }
}
*/
big_integer &big_integer::operator&=(big_integer const &rhs) {
    size_t sizeN = std::max((*this).size(), rhs.size())+1;
    tmpKeeper x(sizeN);//uint32_t* x = new uint32_t[sizeN];
    tmpKeeper y(sizeN);//uint32_t* y = new uint32_t[sizeN];
    for (size_t i=0; i<sizeN; i++){
        if (i<this->size())
            x.put(this->data_[i],i);
        else
            x.put(0,i);
        if (i<rhs.size())
            y.put(rhs.data_[i],i);
        else
            y.put(0,i);
    }
    if (this->sign()){
        x.inverse(sizeN);//inverse(x, sizeN);
    }
    if (rhs.sign()){
        y.inverse(sizeN);//inverse(y, sizeN);
    }
    for (size_t i = 0; i < sizeN; i++) {
        x.put(x.get(i) & y.get(i),i);//x[i] &= y[i];
    }
    (*this).sign_ = false;
    if (x.get(sizeN-1)!=0){
        this->sign_=true;
        x.inverse(sizeN);
    }
    size_t newSize=0;
    for (size_t i=0;i<sizeN;i++){
        if (x.get(i)!=0)
            newSize=i;
    }
    newSize++;
    tmpKeeper t1(newSize);
    for (size_t i = 0; i < newSize; i++)
        t1.put(x.get(i),i);
    t1.swap(this->data_);
    //mmemcpy(this->data_, x,newSize);
    this->size_=newSize;
    return (*this);
}

big_integer &big_integer::operator|=(big_integer const &rhs) {
    (*this)=(~(*this))&(~rhs);
    (*this)=(~(*this));
    return (*this);

}

big_integer &big_integer::operator^=(big_integer const &rhs) {
    big_integer tmp = (*this)&(~rhs);
    big_integer tmp2 = (~(*this))&(rhs);
    (*this)=tmp|tmp2;
    return (*this);
}


big_integer &big_integer::operator<<=(int rhs) {
   big_integer tmp(1);
    for (int i =0;i <rhs; i++)
        tmp*=2;
    (*this)*=tmp;
    return (*this);
}

big_integer &big_integer::operator>>=(int rhs) {
    big_integer tmp(1);
    for (int i =0;i <rhs; i++)
        tmp*=2;
    if(this->sign()){
        big_integer tmpTmp = (*this)/tmp;
        if (tmp*tmpTmp!=(*this)){
            tmpTmp--;
            swap((*this),tmpTmp);
        }
    }
        else
        (*this)/=tmp;
    return (*this);
}//


big_integer operator+(big_integer a, big_integer const &b) {
    return a += b;

}

big_integer operator-(big_integer a, big_integer const &b) {
    return a -= b;
}

big_integer operator*(big_integer a, big_integer const &b) {
    return a *= b;
}

big_integer operator/(big_integer a, big_integer const &b) {
    return a /= b;
}

big_integer operator%(big_integer a, big_integer const &b) {
    return a %= b;
}


big_integer operator&(big_integer a, big_integer const &b) {
    return a &= b;
}

//
big_integer operator|(big_integer a, big_integer const &b) {
    return a |= b;
}

//
big_integer operator^(big_integer a, big_integer const &b) {
    return a ^= b;
}

//
big_integer operator<<(big_integer a, int b) {
    return a <<= b;
}

big_integer operator>>(big_integer a, int b) {
    return a >>= b;
}

//
std::string to_string(big_integer const &a) {
    char* str = new char[a.size()*32];
    big_integer safe(a), tmp;
    size_t i=0;
    big_integer zero;
    while(safe!=zero) {
        tmp = safe % 10;
        safe /= 10;
        str[i] = (tmp.data_[0] + '0');
        i++;
    }
    std::string answer("");
    if (a.sign()){
        answer+='-';
    }
    for (int32_t j=i-1;j>-1;j--) {
        answer = answer + str[j];
    }
    delete[] str;
    if (answer=="")
        return "0";
    return answer;
}

//
std::ostream &operator<<(std::ostream &s, big_integer const &a) {
    return s << to_string(a);
}//

void operator>>(std::istream &s, big_integer &a) {
    std::string str;
    s >> str;
    a = big_integer(str);
}//
