// 16 bit addresses, no offset 
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <cmath>
#include <random>
#include <numeric>
#include <algorithm>
#include <bitset> // c++17

template <class T>
class Cache 
{
public:
	std::vector<T> mem;

	Cache(uint16_t n, uint16_t m) :
		n_{n},
		m_{m},
		bit_mask_{static_cast<uint16_t>((1u << static_cast<int>(std::ceil(std::log2(n_)))) - 1)},
		mem(n * m, 0)
	{
		bool check = (n & (n - 1)) || (m & (m - 1)); // n, m = 2^k
		if (check) { throw std::invalid_argument("Cache constructor: dimensions aren't powers of 2"); }
	}

	// auto operator[](int i, int j) { return mem[row_major(i, j)]; } turns out you can't do this. [] is hard coded to require one param
	// needed to preserve a[b] = *(a + b);

	auto& 	    operator[] (uint32_t i) 	      		{ return mem[i]; } 		   // mem[...] type depends on T, careful on proxy behaviour
	auto& 	    operator() (uint32_t i, uint32_t j)         { return mem[row_major(i, j)]; }
	const auto& operator() (uint32_t i, uint32_t j) const   { return mem[row_major(i, j)]; }

	template<class U>
	friend std::ostream& operator<<(std::ostream& os, const Cache<U>& obj);

	uint16_t getMask() const { return bit_mask_; }

	void insert(uint16_t addr);
private:
	uint32_t n_; // n-direct,          lines
	uint32_t m_; // m-way associative, columns

	uint16_t bit_mask_;
	
	uint32_t row_major(uint32_t i, uint32_t j) const; // needs const to be called from const-context ( const auto& () const higher)
};

// this->operator() shenanigans defeat the whole purpose of moving it out for clarity but oh well learning
template<class T>
void Cache<T>::insert(uint16_t addr) 
{
	uint16_t set{ static_cast<uint16_t>(addr & this->bit_mask_) }; // cast because of int promotion: any arithmetic/bitwise operation on types smaller than int promoted to int before operation
	for (auto j{ this->m_ - 1 }; j>=1; j--) { this->operator()(set, j) = this->operator()(set, j-1); }	// FIFO
	this->operator()(set, 0) = addr;
}

template<class T>
uint32_t Cache<T>::row_major(uint32_t i, uint32_t j) const
{
		if ( i >= this->n_ || j >= this->m_) { throw std::invalid_argument("Incorrect row_major arguments: Element out of bounds"); }
		return i*this->m_ + j;
}

template<class U>
std::ostream& operator<<(std::ostream& os, const Cache<U>& obj) 
{
	for (int i{ 0 }; i < obj.n_; i++) 
	{
		for (int j{ 0 }; j < obj.m_; j++) { os << obj(i,j) << ' '; } os << '\n';
	}
	return os;
}

double mean(const std::vector<uint16_t>& v)
{
	if (v.empty()) return 0.0;
	double sum = std::accumulate(v.begin(), v.end(), 0.0);
	return sum / v.size();
}

// yeah, i know nth_element does some nasty stuff but I've spent too much time on this
uint16_t median(std::vector<uint16_t>& v)
{
	size_t n = v.size() / 2;
	std::nth_element(v.begin(), v.begin() + n, v.end());
	return v[n];
}

double stdev(const std::vector<uint16_t>& v)
{
	if (v.size() < 2) return 0.0;
	double m = mean(v); 
	double sq_sum = std::inner_product(v.begin(), v.end(), v.begin(), 0.0);
	return std::sqrt(sq_sum / v.size() - m * m);
}

// I drop templating because I need to get on with my life
int simulate_cache(uint16_t n, uint16_t m, uint16_t dim)
{
	Cache<uint16_t> c(n, m);

	// rng
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distr(1, 65535); // cheating here to avoid collision with 0: starting from 1
							 // vec is 0 initialized
	
	// two lines for prior std::cout-ing, could be merged
	std::vector<uint16_t> addresses{};
	for ( int n=0; n< std::pow(2, dim); n++)
	{
		addresses.push_back(distr(gen));
	}
	for (auto i : addresses) { c.insert(i); }

	int k{ 0 };
	for (uint16_t i : c.mem ) { if (i != 0) { k++; } }	
	return k;
}

// simulates 100 2^k-way associative caches
void stat_pair(uint16_t k, uint16_t dim)
{
	std::vector<uint16_t> stats; 
	stats.reserve(100);

	double m, med, dev;

	for (int sim_count=0; sim_count < 100; sim_count++)
	{
		stats.push_back(simulate_cache(std::pow(2, k), std::pow(2, dim - k), dim));
	}

	std::cout << "mean: " << mean(stats) << '\n';
	std::cout << "median: " << median(stats) << '\n';
	std::cout << "stdev: " << stdev(stats) << '\n';
}



int main()
{
	int dim = 5; // log2(cache size)

	for (int i=dim; i>=0; i--)
	{
		std::cout << "Cache dimensions: " << std::pow(2, i) << ", " << std::pow(2, dim - i) << '\n';
		stat_pair(i, dim);
	}
}
