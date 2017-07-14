#ifndef DS_HASHSET_H
#define DS_HASHSET_H

#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <algorithm>


template <typename KeyType, typename HashFunc> class ds_hashset 
{
private:
    typedef typename std::list<KeyType>::iterator hash_list_itr;

public:

    // Internal iterator nested class
    class iterator {
    private:
        ds_hashset* m_hs;          
        int m_index;               // current index in the hash table
        hash_list_itr m_list_itr;  // current iterator at the current index

        iterator(ds_hashset* hs) : m_hs(hs), m_index(-1) {}
        iterator(ds_hashset* hs, int index, hash_list_itr loc)
            : m_hs(hs), m_index(index), m_list_itr(loc) {}

    public:
        friend class ds_hashset; 

        iterator() : m_hs(0), m_index(-1)  {}
        iterator(const iterator& itr) : m_hs(itr.m_hs), m_index(itr.m_index), m_list_itr(itr.m_list_itr) {}

        iterator& operator=(const iterator& old) {
            m_hs = old.m_hs;
            m_index = old.m_index; 
            m_list_itr = old.m_list_itr;
            return *this;
        }

        // The dereference operator need only worry about the current
        // list iterator, and does not need to check the current index.
        const KeyType& operator*() const { return *m_list_itr; }

        // The comparison operators must account for the list iterators
        // being unassigned at the end.
        friend bool operator==(const iterator& lft, const iterator& rgt)
        { return lft.m_hs == rgt.m_hs && lft.m_index == rgt.m_index && 
            (lft.m_index == -1 || lft.m_list_itr == rgt.m_list_itr); }

        friend bool operator!= (const iterator& lft, const iterator& rgt)
        { return lft.m_hs != rgt.m_hs || lft.m_index != rgt.m_index || 
            (lft.m_index != -1 && lft.m_list_itr != rgt.m_list_itr); }
       
        iterator& operator++() 
        { 
            this->next();
            return *this;
        }

        iterator operator++(int)
        {
            iterator temp(*this);
            this->next();
            return temp;
        }

        iterator & operator--() 
        { 
            this->prev();
            return *this;
        }

        iterator operator--(int) 
        {
            iterator temp(*this);
            this->prev();
            return temp;
        }

    private:

        // Find the next entry in the table
        void next() 
        {
            ++ m_list_itr;  // next item in the list

            // If we are at the end of this list
            if (m_list_itr == m_hs->m_table[m_index].end()) {
                // Find the next non-empty list in the table
                for (++m_index; 
                         m_index < int(m_hs->m_table.size()) && m_hs->m_table[m_index].empty();
                         ++m_index) {}
                
                // If one is found, assign the m_list_itr to the start
                if (m_index != int(m_hs->m_table.size()))
                    m_list_itr = m_hs->m_table[m_index].begin();
                
                // Otherwise, we are at the end
                else
                    m_index = -1;
            }
        }

        // Find the previous entry in the table
        void prev() 
        {
            // If we aren't at the start of the current list, just decrement
            // the list iterator
            if (m_list_itr != m_hs->m_table[m_index].begin())
                m_list_itr -- ;

            else {
                // Otherwise, back down the table until the previous
                // non-empty list in the table is found
                for (--m_index; m_index >= 0 && m_hs->m_table[m_index].empty(); --m_index) {}

                // Go to the last entry in the list.
                m_list_itr = m_hs->m_table[m_index].begin();
                hash_list_itr p = m_list_itr; ++p;
                for (; p != m_hs->m_table[m_index].end(); ++p, ++m_list_itr) {}
            }
        }
    };
    // end of ITERATOR CLASS


private:
    std::vector< std::list<KeyType> > m_table;  // actual table
    HashFunc m_hash;                            // hash function
    unsigned int m_size;                        // number of keys

public:
    ds_hashset(unsigned int init_size = 10) : m_table(init_size), m_size(0) {}
    ds_hashset(const ds_hashset<KeyType, HashFunc>& old) : m_table(old.m_table), m_size(old.m_size) {}
    ~ds_hashset() {}

    ds_hashset& operator=(const ds_hashset<KeyType,HashFunc>& old) {
        if (&old != this)
            *this = old;
    }

    unsigned int size() const { return m_size; }


    // Insert the key if it is not already there
    std::pair< iterator, bool > insert(KeyType const& key) {
        const float LOAD_FRACTION_FOR_RESIZE = 1.25;

        if (m_size >= LOAD_FRACTION_FOR_RESIZE * m_table.size())
            this->resize_table(2*m_table.size()+1);

        // My code
        int hash = m_hash(key) % m_table.size();
        hash_list_itr p = std::find(m_table[hash].begin(), m_table[hash].end(), key);

        if (p != m_table[hash].end())
            return std::make_pair(iterator(this, hash, p), false);

        m_table[hash].push_back(key);
        p = std::find(m_table[hash].begin(), m_table[hash].end(), key);
        m_size++;
        return std::make_pair(iterator(this, hash, p), true);
    }


    // Find the key, using hash function, indexing and list find
    iterator find(const KeyType& key) 
    {
        unsigned int hash_value = m_hash(key);
        unsigned int index = hash_value % m_table.size();
        hash_list_itr p = std::find(m_table[index].begin(), m_table[index].end(), key);

        return (p == m_table[index].end()) ? this->end() : iterator(this, index, p);
    }


    // Erase the key from the hashset
    int erase(const KeyType& key) 
    {
        iterator p = find(key);
        if (p == end())
            return 0;
        erase(p);
        return 1;
    }

    // Erase at the iterator
    void erase(iterator p)
    {
        m_table[p.m_index].erase(p.m_list_itr);
    }

    // Find the first entry in the table and create an associated iterator
    iterator begin() {
        for (int c = 0; c < m_table.size(); c++) {
            if (m_table[c].size() > 0) {
                return iterator(this, c, m_table[c].begin());
            }
        }
        return end();
    }

    // Create an end iterator.
    iterator end() {
        iterator p(this);
        p.m_index = -1;
        return p;
    }
    
    // A public print utility.
    void print(std::ostream & ostr) {
        for (unsigned int i=0; i<m_table.size(); ++i) {
            ostr << i << ": ";
            for (hash_list_itr p = m_table[i].begin(); p != m_table[i].end(); ++p)
                ostr << ' ' << *p;
            ostr << std::endl;
        }
    }

private:
    void resize_table(unsigned int new_size) {
        std::vector<KeyType> data;  // Stores all keys that are part of this current set

        // Reset size
        m_size = 0;
        
        // Load all the keys into data
        for (int c = 0; c < m_table.size(); c++) {
            if (m_table[c].size() == 0)
                continue; 

            for (typename std::list<KeyType>::iterator it = m_table[c].begin(); it != m_table[c].end(); it++)
                data.push_back(*it);
        }

        // Create a new vector of the requested size
        m_table = typename std::vector<std::list<KeyType> >(new_size);

        // Add all of the keys back into the new vector 
        for (int c = 0; c < data.size(); c++)
            insert(data[c]);
    }
};


#endif
