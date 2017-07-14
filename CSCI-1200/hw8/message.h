#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

class Person; 


// ================================================================
// Messages are passed between people.  A person can only pass a
// message to someone they consider to be their friend.  A message
// thus has a current owner.

class Message {
public:
    Message(const std::string &message, Person *person) : m_message(message), m_owner(person) {}
    const std::string& get_message() const { return m_message; }
    Person* get_owner() const { return m_owner; }
    
    void change_contents(const std::string& message) { m_message = message; }
    void change_owner(Person* person) { m_owner = person; }

private:
    Person* m_owner;
    std::string m_message;
};


// Comparison function
inline bool message_sorter(const Message* a, const Message* b) {
    return (a->get_message() < b->get_message());
}


#endif
