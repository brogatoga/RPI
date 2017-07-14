#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <iostream>
#include <vector>
#include <cassert>


template <class T> class priority_queue
{
private:
    std::vector<T> m_heap;


    // Adds all elements from the given vector into the priority queue -- O(n)
    void build_heap_from_vector(const std::vector<T>& values)
    {
        this->m_heap = values; 
        int n = this->m_heap.size(); 

        for (int c = (n-1)/2; c >= 0; c--)
            percolate_down(c); 
    }


    // Restores the heap property by swapping elements up the heap to parents
    void percolate_up(int index)
    {
        if (index < 0 || index >= m_heap.size() || m_heap.size() == 1)
            return; 

        int parent = (index - 1) / 2; 
        while (parent >= 0) {
            if (m_heap[parent] > m_heap[index]) {
                int temp = m_heap[parent];
                m_heap[parent] = m_heap[index];
                m_heap[index] = temp; 
                index = parent;
                parent = (index - 1) / 2; 
            }
            else break; 
        }
    }


    // Restores the heap property by swapping elements down the heap to children
    void percolate_down(int index)
    {
        if (index < 0 || index >= m_heap.size() || m_heap.size() == 1)
            return;

        int left = (2*index)+1,
            right = (2*index)+2, 
            size = m_heap.size(),
            child; 

        while (left < size) {
            if (right < size && m_heap[right] < m_heap[left])
                child = right; 
            else 
                child = left; 

            if (m_heap[index] >= m_heap[child]) {
                int temp = m_heap[index]; 
                m_heap[index] = m_heap[child]; 
                m_heap[child] = temp;

                index = child; 
                left = (2*index)+1; 
                right = left+1; 
            }
            else break; 
        }
    }


public:
    priority_queue() {}
    priority_queue(const priority_queue<T>& pq) : m_heap(pq.m_heap) {}
    priority_queue(const std::vector<T>& values) { this->build_heap_from_vector(values); }


    // Adds new element to PQ and restores heap by percolating up -- O(log n)
    void push(const T& entry)
    {
        m_heap.push_back(entry);
        percolate_up(m_heap.size()-1); 
    }


    // Removes lowest element from PQ and restores heap by percolating down -- O(log n)
    void pop() 
    {
        if (m_heap.empty())
            return;

        m_heap[0] = m_heap.back(); 
        m_heap.pop_back(); 
        percolate_down(0); 
    }


    // Accessor functions
    const T& top() const { assert(!m_heap.empty()); return m_heap[0]; }
    int size() const { return m_heap.size(); }
    bool empty() const { return m_heap.empty(); }
    bool check_heap() const { return this->check_heap(this->m_heap); }


    //  Check an external vector to see that the heap property is realized -- O(n)
    bool check_heap(const std::vector<T>& heap) const
    {
        int size = heap.size(); 
        int left, right; 

        if (size <= 1)
            return true; 

        for (int c = 0; c < size; c++) {
            left = (2*c)+1; 
            right = left+1;  

            if ((left < size && heap[c] >= heap[left]) || (right < size && heap[c] >= heap[right]))
                return false; 
        }
        return true; 
    }


    //  A utility to print the contents of the heap.  Use it for debugging.
    void print_heap(std::ostream& ostr) const
    {
        for (unsigned int i = 0; i < m_heap.size(); i++)
            ostr << i << ": " << m_heap[i] << std::endl;
    }  
};


// Heap-sort algorithm
template <class T> void heap_sort(std::vector<T>& v)
{
    if (v.size() <= 1)
        return;

    priority_queue<T> pq = v; 

    v.clear();  
    while (!pq.empty()) {
        v.push_back(pq.top()); 
        pq.pop(); 
    }
}


#endif