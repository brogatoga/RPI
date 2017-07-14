#ifndef TABLE_H
#define TABLE_H

#include <iostream>
#include <cstddef>
#include <cstdlib>
#include <vector>

typedef unsigned int size_type; 


template<class T> class Table {
private:
	size_type rows;
	size_type cols;
	T** values; 

	void create(int r, int c, const T& t); 
	void copy(const Table<T>& t);
	void cleanup();
	void resize_rows(size_type r);
	void resize_cols(size_type c);

public:
	Table(size_type r, size_type c, const T& t) { create(r, c, t); };
	Table(const Table& t) { values = NULL; rows = cols = 0; copy(t); }
	~Table() { cleanup(); }
	Table<T>& operator=(const Table<T>& t); 

	void push_back_row(const std::vector<T>& v); 
	void push_back_column(const std::vector<T>& v);
	void push_back_rows(const Table<T>& v);
	void push_back_columns(const Table<T>& v);
	void pop_back_row();
	void pop_back_column();

	void print() const;
	const T& get(size_type r, size_type c) const; 
	void set(size_type r, size_type c, const T& t);

	size_type numRows() const { return rows; }
	size_type numColumns() const { return cols; }

}; 

#include "table.hpp"
#endif