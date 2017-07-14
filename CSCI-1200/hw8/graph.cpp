	#include "graph.h"
#include "person.h"
#include "message.h"
#include <cctype>


// Constructor, properly initializes a new Graph object
Graph::Graph()
{
}


// Destructor, frees all allocated memory and other cleanup routines
Graph::~Graph()
{
	for (int c = 0; c < m_people.size(); c++) {
		if (m_people[c] != NULL)
			delete m_people[c]; 
	}

	for (int c = 0; c < m_messages.size(); c++) {
		if (m_messages[c] != NULL)
			delete m_messages[c];
	}
}


// Returns pointer to person with the given name, or NULL if he doesn't exist
Person* Graph::find_person(const std::string &name) const
{
	for (int c = 0; c < m_people.size(); c++) {
		if (m_people[c] != NULL && m_people[c]->get_name() == name)
			return m_people[c]; 
	}
	return NULL; 
}


// Adds a new person to the graph and returns success status
bool Graph::add_person(const std::string& person_name)
{
	if (find_person(person_name) != NULL || person_name.length() == 0)
		return false; 
	m_people.push_back(new Person(person_name)); 
	return true; 
}


// Removes a person with given name from the graph and returns success status
bool Graph::remove_person(const std::string& person_name)
{
	Person* p = find_person(person_name); 
	if (p == NULL)
		return false; 

	// Iterate through all the people in the graph
	for (std::vector<Person*>::iterator it = m_people.begin(); it != m_people.end(); ) {

		// First, remove the person from the graph
		if (*it == p) {
			it = m_people.erase(it); 
			continue; 
		}

		// Second, remove this person from everyone's friend list
		(*it)->remove_friend(p); 
		it++; 
	}

	// Third, remove all messages that belong to this person
	for (std::vector<Message*>::iterator it = m_messages.begin(); it != m_messages.end(); ) {
		if (*it != NULL && (*it)->get_owner() == p) {
			delete *it; 
			it = m_messages.erase(it); 
			continue; 
		}
		it++; 
	}

	delete p; 
	return true; 
}


// Adds a new friendship between two people in the graph
bool Graph::add_friendship(const std::string& person_name1, const std::string& person_name2)
{
	Person *p1 = find_person(person_name1), 
		   *p2 = find_person(person_name2); 
	return (p1 != NULL && p2 != NULL) ? p1->add_friend(p2) : false;
}


// Removes a friendship between two people in the graph
bool Graph::remove_friendship(const std::string& person_name1, const std::string& person_name2)
{
	Person *p1 = find_person(person_name1),
		   *p2 = find_person(person_name2); 
	return (p1 != NULL && p2 != NULL) ? p1->remove_friend(p2) : false; 	
}


// Adds a new message to a specific person's message list in the graph
bool Graph::add_message(const std::string& person_name, const std::string& message)
{
	Person* p = find_person(person_name); 
	if (p != NULL) {
		Message* m = new Message(message, p);
		m_messages.push_back(m); 
		return p->add_message(m);
	}
	return false; 
}


// Move messages up one forward link in the graph
void Graph::pass_messages(MTRand &mtrand)
{
	for (int m = 0; m < m_messages.size(); m++) {
		if (m_messages[m] == NULL)
			continue; 

		Person* owner = m_messages[m]->get_owner(); 
		Person* target = NULL; 
		const std::list<Person*>& friends = owner->get_friends(); 
		std::string new_message; 
		int size = friends.size(); 
		int random = mtrand.randInt(size); 

		// If this person has no friends OR random is zero (which means do nothing), then skip over this message
		if (size == 0 || random == 0)
			continue; 
		
		// Otherwise, pass this message to the owner's Nth friend
		std::list<Person*>::const_iterator f = friends.begin(); 
		int count = 0; 
		while (count < random-1 && f != friends.end()) {
			count++; 
			f++; 
		}

		// If an error occurred, then skip this message
		if (f == friends.end())
			continue; 

		// Mutate the message and garble it a little so it doesn't exactly resemble the original message
		std::string original = m_messages[m]->get_message(); 
		std::string vowels = "aeiou";
		int max_mutations = mtrand.randInt(original.size() / 4); 
		int number_mutated = 0; 
		int starting_point = mtrand.randInt(original.size()-3) + 1; 
		
		new_message += original.substr(0, starting_point+1); 

		for (int c = starting_point; c < original.size()-1; c++) {

			// If we have already reached the maximum number of mutations, then just blindly add the remaining characters
			// to the end of the newly mutated message. 
			if (number_mutated >= max_mutations) {
				new_message += original[c]; 
				continue; 
			}

			// Vowels might end up getting changed (20% chance of each)
			if (mtrand.randInt(10) <= 5) { 
				for (int d = 0; d < vowels.length(); d++) {
					if (original[c] == vowels[d]) {
						int rand_index = mtrand.randInt(vowels.length()-1); 
						new_message += vowels[rand_index]; 
						number_mutated++; 
						continue; 
					}
				}
			}


			// If the previous character was a space, maybe the first character of the new word will be deleted
			if (c > 0 && original[c-1] == ' ') {
				if (mtrand.randInt(10) <= 4) {
					number_mutated++;
					continue; 
				}
			}

			// If next character is going to be a space, maybe last character of current word will be deleted
			if (c < original.size()-1 && original[c+1] == ' ') {
				int rand = mtrand.randInt(10); 
				if (rand <= 6) {
					number_mutated++;
					continue; 
				}
			}

			// There is also chance of any letter being mutated to its upper case form
			if (mtrand.randInt(10) <= 3) {
				new_message += toupper(original[c]); 
				number_mutated++; 
				continue; 
			}

			// If nothing was mutated, then just add this character to the mutated message as well
			new_message += original[c]; 
		}

		new_message += "\""; 


		// Make the changes
		target = *f; 
		target->add_message(m_messages[m]); 			// Add message to the targeted friend's message list
		owner->remove_message(m_messages[m]);			// Remove message from current person's message list
		m_messages[m]->change_owner(target); 			// Change owner of the message to the targeted friend

		if (new_message.length() > 2)
			m_messages[m]->change_contents(new_message); 	// Change the contents of the message to the mutated version
	}
}


// Prints out a visualization of the entire graph
void Graph::print(std::ostream &ostr) const
{
	std::vector<Person*> people_ordered;

	for (int c = 0; c < m_people.size(); c++) {
		if (m_people[c] != NULL)
			people_ordered.push_back(m_people[c]);
	}
	std::sort(people_ordered.begin(), people_ordered.end(), friend_sorter);

	ostr << std::endl;
	for (std::vector<Person*>::iterator it = people_ordered.begin(); it != people_ordered.end(); it++) {
		const std::list<Person*>& unsorted_people = (*it)->get_friends();
		const std::list<Message*>& unsorted_messages = (*it)->get_messages();
		std::list<Person*> p = unsorted_people; 
		std::list<Message*> m = unsorted_messages; 

		p.sort(friend_sorter);
		m.sort(message_sorter);

		ostr << (*it)->get_name() << std::endl;
		ostr << "  friends:";

		for (std::list<Person*>::const_iterator p_it = p.begin(); p_it != p.end(); p_it++)
			ostr << " " << (*p_it)->get_name(); 

		ostr << std::endl;
		ostr << "  messages:";

		for (std::list<Message*>::const_iterator m_it = m.begin(); m_it != m.end(); m_it++)
			ostr << " " << (*m_it)->get_message();  
		ostr << std::endl;
	}
	ostr << std::endl;
}


// Prints out the names of people invited to a party within N range of the particular person who is hosting the party
bool Graph::print_invite_list(std::ostream &ostr, const std::string &name, int n)
{
	Person* me = find_person(name);
	std::vector<Person*> invited, invited_duplicates;  

	if (n < 0 || me == NULL)
		return false;
	invited_duplicates = get_invite_list(ostr, n, 0, me);

	// "Remove" duplicates from vector to get final result vector with no duplicates
	for (int c = 0; c < invited_duplicates.size(); c++) {
		bool duplicate = false; 

		for (int d = 0; d < invited.size(); d++) {
			if (c != d && invited_duplicates[c] == invited[d]) {
				duplicate = true; 
				break;
			}
		}

		if (!duplicate) {
			invited.push_back(invited_duplicates[c]);
		}
	}

	// Sort the list in alphabetical order
	std::sort(invited.begin(), invited.end(), friend_sorter);

	// Print the list to the given outputs tream
	ostr << "Invite list of " << name << " " << n << ": ";
	for (int c = 0; c < invited.size(); c++)
		ostr << invited[c]->get_name() << " "; 

	ostr << std::endl;
	return true;
}


// Recursive helper function for print_invite_list(), returns a vector of pointers to all people for the given invite list
std::vector<Person*> Graph::get_invite_list(std::ostream& ostr, int n, int current, Person* p)
{
	if (current >= n || p == NULL)
		return std::vector<Person*>();

	std::vector<Person*> result; 
	std::list<Person*> friends = p->get_friends(); 
	std::list<Person*>::iterator it; 

	// Add all of this person's friends to the resulting vector
	for (it = friends.begin(); it != friends.end(); it++) {
		result.push_back(*it);

		// And recursively go into that friend's friends as well and add those too
		std::vector<Person*> more_friends = get_invite_list(ostr, n, current+1, *it);
		result.insert(result.end(), more_friends.begin(), more_friends.end());
	}

	return result; 
}