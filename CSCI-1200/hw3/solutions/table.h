#ifndef _Table_h_
#define _Table_h_

#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>

// =======================================================================
// =======================================================================

// A templated table class to store a matrix-like 2D array of data in rows & columns.
// The table can be dynamically resized with push & pop operations on rows & columns.

template <class T> 
class Table {

public:   
  typedef unsigned int size_type;
  typedef unsigned int index_type;

  // CONSTRUCTORS, ASSIGNMNENT OPERATOR, & DESTRUCTOR
  Table(size_type r, size_type c, const T& v) { this->create(r,c,v); }
  Table(const Table& t) { copy(t); }
  const Table& operator=(const Table& t); 
  ~Table() { destroy(); } 

  // ACCESORS
  // numRows: O(1)
  size_type numRows() const { return rows; }
  // numColumns: O(1)
  size_type numColumns() const { return cols; }
  const T& get(index_type i, index_type j) const;

  // MODIFIERS
  void set(index_type i, index_type j, const T& v);
  void push_back_row(const std::vector<T> &v);
  void push_back_column(const std::vector<T> &v);
  void pop_back_row();
  void pop_back_column();

  // OUTPUT 
  void print() const;
  
private:  

  // PRIVATE MEMBER FUNCTIONS
  void create(size_type r, size_type c);
  void create(size_type r, size_type c, const T& v);
  void destroy();
  void copy(const Table<T>& t); 
  int maxEntryWidth() const;

  // REPRESENTATION
  size_type rows,cols;
  T** values;
};


// =======================================================================
// =======================================================================


// Create an empty Table
template <class T>  void Table<T>::create(size_type r, size_type c) {
  assert (r >= 1 && c >= 1);
  rows = r;
  cols = c;
  values = new T*[rows];
  for (unsigned int i = 0; i < rows; i++) {
    values[i] = new T[cols];
  }
}


// Fill in default values
template <class T>  void Table<T>::create(size_type r, size_type c, const T& v) {
  create(r,c);
  for (unsigned int i = 0; i < rows; i++) {
    for (unsigned int j = 0; j < cols; j++) {
      values[i][j] = v;
    }
  }
}


// cleanup all memory from the table
template <class T> void Table<T>::destroy() {
  for (unsigned int j = 0; j < rows; j++) {
    delete [] values[j];
  }
  delete [] values; 
}


// Assign one Table to another, avoiding self-assignment
template <class T> const Table<T>& Table<T>::operator=(const Table<T>& v) {
  if (this != &v) {
    destroy();
    this->copy(v);
  }
  return *this;
}


// Create the Table as a copy of the given Table
template <class T> void Table<T>::copy(const Table<T>& v) {
  this->create(v.rows,v.cols);
  for (unsigned int i = 0; i < rows; i++) {
    for (unsigned int j = 0; j < cols; j++) {
      values[i][j] = v.values[i][j];
    }
  }  
}


// =======================================================================
// =======================================================================


// ACCESSOR
// get: O(1)
template <class T> const T& Table<T>::get(index_type i, index_type j) const {
  // verify legal element
  if (!(i >= 0 && i < rows && 
        j >= 0 && j < cols)) {
    std::cerr << "ERROR in get: attempt to use invalid index" << std::endl;
    exit(1);
  }
  return values[i][j];
}


// MODIFIERS
// set: O(1)
template <class T> void Table<T>::set(index_type i, index_type j, const T& t) {
  // verify legal element
  if (!(i >= 0 && i < rows && 
        j >= 0 && j < cols)) {
    std::cerr << "ERROR in set: attempt to use invalid index" << std::endl;
    exit(1);
  }
  values[i][j] = t;
}


// =======================================================================
// =======================================================================


// push_back_row: O(n+m)
template <class T> void Table<T>::push_back_row(const std::vector<T> &v) {
  // verify new data size
  if (!(v.size() == cols)) {
    std::cerr << "ERROR in push_back_row: incorrect dimensions" << std::endl;
    exit(1);
  }
  // resize to add a new row
  T** tmp = values;
  values = new T*[rows+1];
  for (unsigned int i = 0; i < rows; i++) {
    values[i] = tmp[i];
  }
  delete [] tmp;
  // copy the data over
  values[rows] = new T[cols];
  for (unsigned int j = 0; j < cols; j++) {
    values[rows][j] = v[j];
  }
  rows++;
}


// push_back_column: O(n*m)
template <class T> void Table<T>::push_back_column(const std::vector<T> &v) {
  // verify new data size
  if (!(v.size() == rows)) {
    std::cerr << "ERROR in push_back_column: incorrect dimensions" << std::endl;
    exit(1);
  }

  // resize each row
  for (unsigned int i = 0; i < rows; i++) {
    T* tmp = values[i];
    values[i] = new T[cols+1];
    for (unsigned int j = 0; j < cols; j++) {
      values[i][j] = tmp[j];
    }
    // copy new entry for this row
    values[i][cols] = v[i];
    delete [] tmp;
  }
  cols++;
}


// pop_back_row: O(n+m)
template <class T> void Table<T>::pop_back_row() {
  // verify at least one row
  assert (rows > 1);
  T** tmp = new T*[rows-1];
  // copy over old data
  for (unsigned int i = 0; i < rows-1; i++) {
    tmp[i] = values[i];
  }
  delete [] values[rows-1];
  delete [] values;
  values = tmp;
  rows--;
}


// pop_back_column: O(n*m)
template <class T> void Table<T>::pop_back_column() {
  // verify at least one column
  assert (cols > 1);
  for (unsigned int i = 0; i < rows; i++) {
    T* tmp = values[i];
    values[i] = new T[cols-1];
    // copy over old data
    for (unsigned int j = 0; j < cols-1; j++) {
      values[i][j] = tmp[j];
    }
    delete [] tmp;
  }
  cols--;
}


// =======================================================================
// =======================================================================


// PRINT
// print: O(n*m)
template <class T> void Table<T>::print() const {
  std::cout << "table: " << rows << " rows, " << cols << " cols" << std::endl;
  int entry_width = maxEntryWidth();
  for (unsigned int i = 0; i < rows; i++) {
    for (unsigned int j = 0; j < cols; j++) {
      std::cout << std::setw(entry_width) << values[i][j] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

// MAX ENTRY WIDTH: SPECIALIZATION FOR CHAR
template <>
int Table<char>::maxEntryWidth() const {
  return 1;
}

// MAX ENTRY WIDTH: SPECIALIZATION FOR STRING
template <>
int Table<std::string>::maxEntryWidth() const {
  int answer = 1;
  for (unsigned int i = 0; i < rows; i++) {
    for (unsigned int j = 0; j < cols; j++) {
      answer = std::max(answer, (int)values[i][j].length());
    }
  }
  return answer;
}

// MAX ENTRY WIDTH: SPECIALIZATION FOR INT
template <>
int Table<int>::maxEntryWidth() const {
  int answer = 1;
  for (unsigned int i = 0; i < rows; i++) {
    for (unsigned int j = 0; j < cols; j++) {
      int tmp = 1;
      if (values[i][j] < 0) 
	tmp = log10((float)(-values[i][j])) + 2;
      else if (values[i][j] > 0)
	tmp = log10((float)values[i][j]) + 1;
      answer = std::max(answer, tmp);
    }
  }
  return answer;
}

// MAX ENTRY WIDTH: ANY OTHER TYPE
template <class T> int Table<T>::maxEntryWidth() const {
  return 10;
}


// =======================================================================
// =======================================================================

#endif
