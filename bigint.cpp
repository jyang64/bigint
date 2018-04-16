//
// Started by Tom on 2/9/18.
//

#include "bigint.h"


//gets rid of any extra zeroes in front of the big integer (back of vector)
void bigint::strip_zeros() {
	while (number.back() == 0 and number.size() > 1){	//the size must never drop below zero.
		number.pop_back();						
	}
}

/* Constructors
 * 
 * */
//Default constructor does not need strip_zeros because it does not read anything, just creates {0}
bigint::bigint() {
	number.clear();
	number.push_back(0); 
}
//
bigint::bigint(const std::vector<vec_bin> &that) {
	number.clear();
	number = that;
	strip_zeros();
}
//turns an unsigned long long into a bigint
bigint::bigint(unsigned long long i) {
	number.clear();
	while(i > 0){
		number.push_back(i % 10);	//By repeatedly getting the remainder by dividing by 10, I can remove the 1st digit of the unsgned long long while also getting it as an individual value.
		i = i/10;
	} 
	//strip_zeros();
	if (number.empty()){
		number.push_back(0);	//But in the case where i = 0, the vector becomes empty, so this if statement corrects it.
	}
}
//the string needs to be converted from ASCII to integer by subtracting '0' or 48
bigint::bigint(std::string str) {
	number.clear();
	for (int i = str.length() - 1; i >= 0; i--){
		number.push_back((int)str[i] - 48);	
	}
	strip_zeros();
}
//reads from a file and creates a bigint
bigint::bigint(std::ifstream &infile) {
	number.clear();
	std::string line;
	std::string str;	
	while (getline(infile, line)){	
		str += line;	//each line is converted into a string and is added to str
	}
	*this = bigint(str);	//the resulting string is the argument in the already defined string constructor
	strip_zeros();
}
//creates an identical bigint
bigint::bigint(const bigint &that) {
	*this = that;
	strip_zeros();
}


/* Number Access
 * 
 * */
 //returns bigint in vector form
const std::vector<vec_bin> &bigint::getNumber() const {
	return number;
}
//returns a single digit from bigint
vec_bin bigint::operator[](size_t index) const {
	return number[index];
}


/** Comparators
 *
 * */
 //starting from the back of the vector, checks if each digit is equal to each other
bool bigint::operator==(const bigint &that) const { 
	return number == that.number;
}
//this inequality operator just returns the opposite of equality.
bool bigint::operator!=(const bigint &that) const {
	return !(*this == that);
}

bool bigint::operator<(const bigint &that) const {
	bool g = true;
	if (number.size() < that.number.size()){	//first, if size is different, there is no need to compare digit by digit
		return true;
	}else if (number.size() > that.number.size()){
		return false;
	}
	for (size_t n = 0;n < number.size(); ++n){	
		if (this->number[number.size()-n-1] < that[number.size()-n-1]){
			return true;	//as soon a digit is greater than the other number's same digit, loop can exit
		} else if(this->number[number.size()-n-1] > that[number.size()-n-1]){
			return false;
		}
	}
	return false;
}

//<= is just combining == and <
bool bigint::operator<=(const bigint &that) const {
	if (*this == that){	
		return true;
	}
	return(*this < that);
}
// I need == and < to return false to return true for > 
bool bigint::operator>(const bigint &that) const {
	if (*this == that){
		return false;
	}
	return !(*this < that);
}
//>= always returns the opposite of <
bool bigint::operator>=(const bigint &that) const {
	return !(*this < that);
}


/** Addition
 *
 * */
//adds two bigints
bigint bigint::add(const bigint &that) const {
	std::vector<vec_bin> sum;
	bigint k = bigint(that);
	int digitsum;
	int carry = 0;
	if (number.size() > that.number.size()){	//first, if size is different, use size reference for bigger int
		k = bigint(number);
	}		
	for (size_t n = 0; n < k.number.size(); n++){
		if (n+1 > number.size() or n+1 > that.number.size()){	
			digitsum = k[n] + carry;
		} else{
			digitsum = number[n] + that.number[n] + carry;
		}
		carry = digitsum/10;
		sum.push_back(digitsum % 10);
	}
	sum.push_back(carry);
	return bigint(sum);
}
//ditto to add
bigint bigint::operator+(const bigint &that) const {
	return this->add(that);
}
//add and return same number
bigint &bigint::operator+=(const bigint &that) {
	*this = *this + that;
	return *this;
}
//add 1
bigint &bigint::operator++() {
	*this = *this + bigint(1);
	return *this;
}
//add 1, returns previous number
bigint bigint::operator++(int) {
	bigint temp = *this;
	*this += bigint(1);
	return temp;
}


/** Subtraction
 *
 * */
//subtracts two bigints
bigint bigint::subtract(const bigint &that) const {
	/*Process:
		first check if number being subtracted from is smaller than the number being subtracted, if not return error
		start from ones digit {1,2,3,4,5} so start from leftmost (location 0)
		if even or greater, just subtract
		if smaller, subtract 1 from next digit up (if zero, turn into 9 and go to next digit)
	*/
	std::vector<vec_bin> dif;
	int digitdif = 0;
	int carry = 0;
	if (*this <= that){	//used to make sure the number being subtracted from is the bigger number.
		if (*this < that){
			std::cerr << "error: failed";
			abort();
		} else{
		return bigint(0);
		}
	}
	dif.clear();
	for (size_t n = 0; n < number.size(); ++n){
		if(n < that.number.size()){	
			if(number[n] >= that.number[n] + carry){
				digitdif = number[n] - carry - that.number[n];
				carry = 0;
			} else{
				digitdif = 10 + number[n] - carry - that.number[n];
				carry = 1;
			}
		} else{
			digitdif = number[n] - carry;
			carry = 0;
		}
		if (digitdif < 0){
			digitdif += 10;	// turns negative numbers into 9's
		}
		dif.push_back(digitdif);
	}
	if(bigint(dif) > *this){
		dif.pop_back();		//the top digit of *this is needed there for subtraction, but can be removed after
	}
	return bigint(dif);
}
//subtracts that from this
bigint bigint::operator-(const bigint &that) const {
	return this->subtract(that);
}
//subtracts that from this, returns this
bigint &bigint::operator-=(const bigint &that) {
	*this = *this - that;
	return *this;
}
//subtract 1
bigint &bigint::operator--() {
	*this = *this - bigint(1);
	return *this;
}
//subtract 1 and return previous number
bigint bigint::operator--(int) {
	bigint temp = *this;
	*this -= bigint(1);
	return temp;
}


/** Multiplication
 *
 * */
//multiply, by adding "bigint" times
bigint bigint::multiply(const bigint &that) const {
	bigint product = 0;
	for (bigint i = 0; i < that; ++i){
		product += *this;
	}
	return product;
}
//multiply this by that
bigint bigint::operator*(const bigint &that) const {
	return this->multiply(that);
}
//multiply this by that, product is in *this
bigint &bigint::operator*=(const bigint &that) {
	*this = *this * that;
	return *this;
}


/** Division
 *
 * */
//divide, by adding "bigint" times until *this is reached/surpassed
bigint bigint::divide(const bigint &that) const {
	bigint quotient = 0;
	bigint i = 0;
	if(*this < that){
		return 0;
	}
	while (quotient <= *this){
		++i;
		quotient += that;
	}
	return i - 1;
}
// divide *this by that
bigint bigint::operator/(const bigint &that) const {
	return this->divide(that);
}
// divide *this by that, quotient is in *this
bigint &bigint::operator/=(const bigint &that) {
	*this = *this / that;
	return *this;
}


/** Modulo
 *
 * */
//get remainder by adding "bigint" times - 1, then get difference between product and *this
bigint bigint::mod(const bigint &that) const {
	bigint quotient = 0;
	bigint i = 0;
	if(*this < that){
		return 0;
	}
	while (quotient <= *this){
		++i;
		quotient += that;
	}
	quotient = (i-1) * that;
	return *this - quotient;
}
//modulo operator
bigint bigint::operator%(const bigint &that) const {
	return this->mod(that);
}
//mod, then put in *this
bigint &bigint::operator%=(const bigint &that) {
	*this = *this % that;
	return *this;
}


/** Power
 *
 * */
// exponent. Special case for 0 is needed
bigint bigint::pow(unsigned long long n) {
	if (n==0){
		return bigint(1);
	}
	unsigned long long i =0;
	bigint exponent = *this;
	for(i = 0; i < n-1 ;i++){
		exponent *= *this;
	}
	return exponent;
}


/** Display methods
 *
 * */
//put bigint into ostream
std::ostream &operator<<(std::ostream &os, const bigint &bigint1) {
	os << bigint1.to_string(true);
	return os;
}
//returns the bigint as a string, with a boolean argument to decide if commas should be included
std::string bigint::to_string(bool commas) const {
	std::string s;
	int n = 0;
	while  (n != number.size()){
		if (commas){
			s += std::to_string(number[number.size() - n -1]);  //a digit is turned into string form which is pushed into a string
			n++;
			if ((number.size() - n) % 3 == 0 and n != number.size()){	//To prevent a comma after ones place, the loop is not allowed to put a comma down when the remaining digits are zero
				s += ",";
			}
		}else{
			s += std::to_string(number[number.size() - n -1]);  // the comma rule can be ignored if the argument is false.
			n++;
		}
	}	
	return s;
}
//returns the bigint as a string in scientific notation. 
std::string bigint::scientific(unsigned int decimal_points) const {
	std::string str;									
	str += std::to_string(number.back());		//the first character in the string is the string version of the first digit (back of vector)
	str += ".";
	for (int i = 0; i < decimal_points; i++){	//for each decimal point, point to the next digit and add it to the string
		str += std::to_string(number[number.size()- 2 - i]);
	}
	str += "E";
	str += std::to_string(number.size() - 1);	//the exponent is the size of the vector minus 1
	return str;
}
//writes to a file the bigint. "wrap" is how long the lines are in the file
void bigint::to_file(std::ofstream &outfile, unsigned int wrap) {
	std::string str = this-> to_string();	//predefined string conversion makes the bigint a string
	for(size_t n = 0; n < str.length(); n++){	
		outfile << str[n];
		if ((n+1) % wrap == 0){		//n+1 divided by wrap's remainder is zero when the user wants the line to end
			outfile << std::endl;
		}
	}

}
