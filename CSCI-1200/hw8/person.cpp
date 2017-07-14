#include "person.h"
#include "message.h"
#include <iostream>


// Returns whether this person is friends with another person
bool Person::is_friend(Person* person) const
{
	if (person != NULL) {
		for (std::list<Person*>::const_iterator it = m_friends.begin(); it != m_friends.end(); it++) {
			if ((*it)->get_name() == person->get_name())
				return true; 
		}
	}
	return false; 
}


// Returns whether this person has a particular message string or not
bool Person::message_exists(const std::string& message) const
{
	for (std::list<Message*>::const_iterator it = m_messages.begin(); it != m_messages.end(); it++) {
		if ((*it) != NULL && (*it)->get_message() == message)
			return true; 	
	}
	return false; 
}


// Adds a new friend to this person's friend list
bool Person::add_friend(Person *person)
{
	if (person != NULL && !is_friend(person) && this != person) {
		m_friends.push_front(person); 
		return true; 
	}
	return false; 
}


// Removes a friend from this person's friend list 
bool Person::remove_friend(Person* person)
{
	if (person != NULL && this != person) {
		for (std::list<Person*>::iterator it = m_friends.begin(); it != m_friends.end(); it++) {
			if (*it == person) {
				m_friends.erase(it);
				return true; 
			}
		}
	}
	return false; 
}


// Adds a new message to this person's message list
bool Person::add_message(Message *message)
{
	if (message != NULL && (message->get_message()).length() > 0) {
		m_messages.push_front(message); 
		return true; 
	}
	return false; 
}


// Removes a specific message from this person's message list
bool Person::remove_message(Message *message)
{
	if (message != NULL && m_messages.size() > 0) {
		for (std::list<Message*>::iterator it = m_messages.begin(); it != m_messages.end(); it++) {
			if (*it != NULL && *it == message) {
				m_messages.erase(it); 
				return true; 
			}
		}
	}
	return false;
}