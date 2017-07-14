#ifndef PERSON_H
#define PERSON_H

#include <list>
#include <string>

class Message;


// ================================================================
// The person class has a name and list of other people who this person
// considers to be friends.  Note: the friendship relationship is not
// necessarily bi-directional.  A person also has a list of messages.

class Person {
public:
    Person(const std::string &name) : m_name(name) {}

    const std::string& get_name() const { return m_name; }
    const std::list<Person*>& get_friends() const { return m_friends; }
    const std::list<Message*>& get_messages() const { return m_messages; }

    bool is_friend(Person* person) const;
    bool message_exists(const std::string& message) const; 

    bool add_friend(Person* person);
    bool remove_friend(Person* person);
    bool add_message(Message* message);
    bool remove_message(Message* message);

private:
    std::string m_name;
    std::list<Person*> m_friends;
    std::list<Message*> m_messages;
};


// Comparison function
inline bool friend_sorter(const Person* a, const Person* b) {
    return (a->get_name() < b->get_name());
}


#endif