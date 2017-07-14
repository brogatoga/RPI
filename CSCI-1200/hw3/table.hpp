#include "table.h" 


// Creates a table with dimensions r-by-c populated with t in every entry
template<class T> void Table<T>::create(int r, int c, const T& t)
{
	if (r <= 0 || c <= 0) {
		std::cerr << "Invalid dimensions for initializing Table" << std::endl;
		exit(1);
	}
	
	rows = r; 
	cols = c;
	values = new T*[rows];

	for (T** p = values; p < values+rows; p++) {
		*p = new T[cols]; 
		for (T* q = *p; q < *p + cols; q++)
			*q = t; 
	}
}

// Creates a copy of the specified table
template<class T> void Table<T>::copy(const Table<T>& t)
{
	cleanup();
	rows = t.rows; 
	cols = t.cols; 
	values = new T*[rows]; 

	for (T** p = values; p < values+rows; p++) {
		*p = new T[cols]; 
		for (T* q = *p; q < *p+cols; q++)
			*q = *(*(t.values + (p - values)) + (q - *p)); 
	}
}

// Deallocates all allocated memory in the object
template<class T> void Table<T>::cleanup()
{
	if (values == NULL)
		return;

	for (T** p = values; p < values+rows; p++) {
		if (*p != NULL)
			delete[] *p; 
		*p = NULL; 
	}
	delete[] values; 
	values = NULL; 
	rows = cols = 0; 
}

// Resize the table to r rows, and allocate or deallocate memory as needed
template<class T> void Table<T>::resize_rows(size_type r)
{
	if (r == rows)		// no changes needed
		return;
	else if (r <= 0) {	// if resizing to zero rows, then this means table will be empty, so reset it to its default state
		cleanup();
		return;
	}

	T** new_values = new T*[r]; 
	size_type max = (r > rows) ? rows : r; 

	for (T** p = new_values; p < new_values+max; p++)	// copy addresses of rows [0,r-1] to new array
		*p = *(values + (p - new_values)); 

	if (r < rows) {						// if we are shrinking the rows, then delete column arrays starting from the r'th
		for (T** p = values+rows-1; p >= values+r; p--)
			delete[] *p;  
	}
	else if (r > rows) {				// if we are growing the rows, then allocate memory for new column sub-arrays
		for (T** p = new_values+rows; p < new_values+r; p++)
			*p = new T[cols]; 
	}

	delete[] values; 			// deallocate primary array and make values point to the start of the new array
	values = new_values; 
	rows = r; 
}

// Resize the table to c columns, and allocate or deallocate memory as needed
template<class T> void Table<T>::resize_cols(size_type c)
{
	if (c == cols)		// no changes needed
		return;
	else if (c <= 0) {		// same as in previous function
		cleanup(); 
		return;
	}

	for (T** p = values; p < values+rows; p++) {
		T* new_col = new T[c]; 
		size_type max = (c > cols) ? cols : c; 	// max number of elements that can and should be copied to new_col

		for (T* q = new_col; q < new_col+max; q++)
			*q = *(*(values + (p - values)) + (q - new_col));  // copy values from the old array to the new array

		delete[] *p;  		// deallocate old column array and make values[i] point to start of new array
		*p = new_col;
	}
	cols = c; 
}

// Returns the value of the table at [r, c]
template<class T> const T& Table<T>::get(size_type r, size_type c) const
{
	if (r >= rows || c >= cols || r < 0 || c < 0) {
		std::cerr << "Invalid row or column index for Table::get() call" << std::endl;
		exit(1);
	}
	return *(*(values+r)+c); 
}

// Sets the value of the table at [r, c] to t
template<class T> void Table<T>::set(size_type r, size_type c, const T& t)
{
	if (r >= rows || c > cols || r < 0 || c < 0) {
		std::cerr << "Invalid row or column index for Table::set() call" << std::endl;
		exit(1);
	}
	*(*(values+r)+c) = t; 
}

// Prints the table's dimensions and contents
template<class T> void Table<T>::print() const
{
	std::cout << "table: " << rows << " rows, " << cols << " cols" << std::endl;
	for (T** p = values; p < values+rows; p++) {
		for (T* q = *p; q < *p+cols; q++)
			std::cout << *q << " "; 
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

// Assignment operator overload function
template<class T> Table<T>& Table<T>::operator=(const Table<T>& t)
{
	if (this != &t)	  // make sure we aren't assigning the same object to itself
		copy(t);
	return *this; 
}

// Adds new row to end of table
template<class T> void Table<T>::push_back_row(const std::vector<T>& v)
{
	if (v.size() != cols) {
		std::cerr << "Unable to push_back_row because size of vector != columns" << std::endl;
		exit(1);
	}
	resize_rows(rows+1);
	T* b = *(values+rows-1);
	for (T* p = b; p < b+cols; p++)
		*p = v[p-b]; 
}

// Adds new column to end of table
template<class T> void Table<T>::push_back_column(const std::vector<T>& v)
{
	if (v.size() != rows) {
		std::cerr << "Unable to push_back_column because size of vector != rows" << std::endl;
		exit(1);
	}
	resize_cols(cols+1);
	for (T** p = values; p < values+rows; p++)
		*(*(p)+cols-1) = v[p-values]; 
}

// Adds one or more rows to end of table
template<class T> void Table<T>::push_back_rows(const Table<T>& v)
{
	if (v.rows == 0 || v.cols != cols) {
		std::cerr << "Unable to push_back_row because table is either empty or column sizes do not match" << std::endl;
		exit(1);
	}

	size_type start = rows;
	resize_rows(rows+v.rows); 
	for (T** p = v.values; p < v.values+v.rows; p++) {
		for (T* q = *p; q < *p+v.cols; q++)
			*(*(values+start+(p-v.values))+(q-*p)) = *q;  
	}
}

// Adds one or more columns to end of table
template<class T> void Table<T>::push_back_columns(const Table<T>& v)
{
	if (v.cols == 0 || v.rows != rows) {
		std::cerr << "Unable to push_back_column because table is either empty or row sizes do not match" << std::endl;
		exit(1);
	}

	size_type start = cols; 
	resize_cols(cols+v.cols);
	for (T** p = v.values; p < v.values+v.rows; p++) {
		for (T* q = *p; q < *p+v.cols; q++)
			*(*(values+(p-v.values))+start+(q-*p)) = *q; 
	}
}

// Removes last row from the table
template<class T> void Table<T>::pop_back_row()
{
	if (rows == 0) {
		std::cerr << "Cannot pop back row because there are no rows left" << std::endl;
		exit(1);
	}
	resize_rows(rows-1);
}

// Removes last column from the table
template<class T> void Table<T>::pop_back_column()
{
	if (cols == 0) {
		std::cerr << "Cannot pop back column because there are no columns left" << std::endl;
		exit(1);
	}
	resize_cols(cols-1);
}