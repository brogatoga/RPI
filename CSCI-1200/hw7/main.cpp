#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <ctime>
#include <vector>
#include <string>
#include <list>
#include <map>
#include "MersenneTwister.h"


// GLobal type definitions
typedef std::map<std::string, int>      WF_entry;       // Word -> Frequency
typedef std::map<std::string, WF_entry> WF2_map;        // Word -> (Word -> Frequency)             
typedef std::map<std::string, WF2_map>  WF3_map;        // Word -> (Word -> (Word -> Frequency))


// Read from the input string and get the next word or double quote (which is a special type of word)
bool ReadNextWord(std::istream& istr, std::string& word) 
{
    char c;
    word.clear();

    while (istr) {
        c = istr.peek();

        if (isspace(c)) {
            // skip whitespace before a word starts
            istr.get(c);
            if (word != "") {
                // break words at whitespace
                return true;
            }
        } 

        else if (c == '"') {
            // double quotes are a delimiter and a special "word"
            if (word == "") {
                istr.get(c);
                word.push_back(c);
            }
            return true;
        } 

        else if (isalpha(c)) {
            // this a an alphabetic word character
            istr.get(c);
            word.push_back(tolower(c));
        } 

        else {
            // ignore this character (probably punctuation)
            istr.get(c);
        }
    }
    return false;
}


// Reads all the words inside a pair of quotes into a vector of words
std::vector<std::string> ReadQuotedWords(std::istream& istr) 
{
    std::vector<std::string> answer;
    std::string word;
    bool open_quote = false;

    while (ReadNextWord(istr,word)) {
        if (word == "\"") {
            if (!open_quote) 
                open_quote = true; 
            else break;
        } 
        else answer.push_back(word);
    }
    return answer;
}


// Loads the sample text from the file and stores data into the two maps provided
void LoadSampleText(WF_entry& wf, WF2_map& wf2, WF3_map& wf3, const std::string& filename, 
    int window, const std::string& parse_method) {
    std::ifstream istr(filename.c_str());
    bool ignore_punctuation = false;

    if (!istr) { 
        std::cerr << "ERROR cannot open file: " << filename << std::endl; 
        exit(1);
    } 

    if (window < 2) {
        std::cerr << "ERROR window size must be >= 2:" << window << std::endl;
    }

    if (parse_method == "ignore_punctuation") {
        ignore_punctuation = true;
    } 
    else {
        std::cerr << "ERROR unknown parse method: " << parse_method << std::endl;
        exit(1);
    }


    // Read in the words and add them to the custom map data structure
    std::string current, 
                prev, 
                prev_prev; 

    if (ReadNextWord(istr, prev_prev)) {
        wf[prev_prev]++; 
        if (ReadNextWord(istr, prev)) {
            wf[prev]++; 
            wf2[prev_prev][prev]++; 

            while (ReadNextWord(istr, current)) {
                if (current == "\"")     
                    continue;

                wf[current]++; 
                wf2[prev][current]++; 
                wf3[prev_prev][prev][current]++; 

                prev_prev = prev; 
                prev = current; 
            }
        }
    }
}


// Joins a vector of strings together with a delimiter
std::string join(const std::vector<std::string>& words, const std::string& delimiter)
{
    std::string result; 
    int size = words.size(); 

    if (size == 0) return ""; 
    else if (size == 1) return words[0]; 

    for (int c = 0; c < words.size(); c++)
        result += (words[c] + delimiter);
    return result; 
}


// Converts a WF3 map to a WF2 map and returns it
WF2_map WF3toWF2(const WF3_map& wf3)
{
    WF2_map result; 
    for (WF3_map::const_iterator p = wf3.begin(); p != wf3.end(); p++) {
        for (WF2_map::const_iterator q = (p->second).begin(); q != (p->second).end(); q++) {
            int total_freq = 0; 
            for (WF_entry::const_iterator r = (q->second).begin(); r != (q->second).end(); r++)
                total_freq += r->second; 
            result[p->first][q->first] += total_freq; 
        }
    }
    return result; 
}


// Main function, entry point of the application
int main() 
{
    MTRand generator;           // Random number generator
    WF_entry wf;                // All single words and their respective number of occurrences
    WF2_map wf2;                // Word-frequency map for window_size=2
    WF3_map wf3;                // Word-frequency map for window_size=3
    WF2_map wf3_shrinked;       // This is the wf3 map converted for window_size=2 operations
    std::string command;        // Contains the next string token to be read in from std::cin
    int window_size;            // Window size (either 2 or 3)


    // Main input loop
    while (std::cin >> command) {

        // Load the sample text file
        if (command == "load") {
            std::string filename;
            int window;
            std::string parse_method;
            std::cin >> filename >> window >> parse_method;      

            LoadSampleText(wf, wf2, wf3, filename, window, parse_method);
            wf3_shrinked = WF3toWF2(wf3); 
            window_size = window;

            std::cout << "Loaded " << filename << " with window = " << window << " and parse method = " 
                << parse_method << std::endl << std::endl;;
        } 


        // Print the portion of the map structure with the choices for the next word given a particular sequence.
        else if (command == "print") {
            std::vector<std::string> sentence = ReadQuotedWords(std::cin);

            // Only one word specified, so use WF2 (or WF3 if chosen)
            if (window_size == 2 || (window_size == 3 && sentence.size() == 1)) {
                WF2_map::const_iterator p = (window_size == 2) ? wf2.find(sentence[0]) : wf3_shrinked.find(sentence[0]); 
                WF_entry::const_iterator it = (p->second).begin(); 
                int sum = 0;

                while (it != (p->second).end()) {
                    sum += it->second; 
                    it++;
                }
                it = (p->second).begin();  

                std::cout << p->first << " (" << wf[p->first] << ")" << std::endl;
                while (it != (p->second).end()) {
                    std::cout << p->first << " " << it->first << " (" << it->second << ")" << std::endl;
                    it++; 
                }
            }


            // Two words specified, so use WF3
            else if (window_size == 3) {
                WF3_map::const_iterator p = wf3.find(sentence[0]); 
                WF2_map::const_iterator q = (p->second).find(sentence[1]); 
                WF_entry::const_iterator it = (q->second).begin(); 
                std::string initial = join(sentence, " "); 
                int sum = 0; 

                while (it != (q->second).end()) {
                    sum += it->second; 
                    it++;
                }
                it = (q->second).begin();  

                std::cout << initial << "(" << sum << ")" << std::endl;
                while (it != (q->second).end()) {
                    std::cout << initial << it->first << " (" << it->second << ")" << std::endl;
                    it++; 
                }
            }


            // Three or more words (NOT SUPPORTED)
            else { }

            std::cout << std::endl;
        }


        // Generate the specified number of words 
        else if (command == "generate") {
            std::vector<std::string> sentence = ReadQuotedWords(std::cin);
            std::string current_word = sentence.back(); 
            std::string last_word, last_word_2; 
            std::string selection_method; 
            int length; 
            bool random_flag;

            // Parse input and flags
            std::cin >> length >> selection_method; 

            if (selection_method == "random") {
                random_flag = true;
            } 
            else {
                assert (selection_method == "most_common");
                random_flag = false;
            }


            // Begin generating the sentences
            if (sentence.size() == 1) {
                std::cout << sentence[0] << " ";
                last_word = sentence[0]; 
            }
            else if (sentence.size() == 2) { 
                std::cout << join(sentence, " "); 
                last_word = sentence[0]; 
                last_word_2 = sentence[1]; 
            }

            for (int c = 0; c < length; c++) {
                std::string word;
                int max_freq = 0; 

                // For window size = 2
                if (window_size == 2) {
                    WF2_map::const_iterator p = (window_size == 2) ? wf2.find(last_word) : wf3_shrinked.find(last_word); 
                    WF_entry::const_iterator it = (p->second).begin(); 
                    
                    // Random words
                    if (random_flag) { 
                        int total_freq = 0;     
                        int random; 
                        int counter = 0; 

                        while (it != (p->second).end()) {
                            total_freq += it->second; 
                            it++; 
                        }
 
                        it = (p->second).begin(); 
                        random = generator.randInt(total_freq-1)+1; 

                        while (it != (p->second).end()) {
                            counter += it->second; 
                            if (counter >= random) {
                                word = it->first;
                                break;
                            }
                            it++; 
                        }
                    }

                    // Most common words
                    else {
                        while (it != (p->second).end()) {
                            if (it->second > max_freq) {
                                max_freq = it->second; 
                                word = it->first; 
                            }
                            it++;
                        }
                    }
                }


                // For window size = 3
                else if (window_size == 3) {
                    WF3_map::const_iterator p = wf3.find(last_word); 
                    WF2_map::const_iterator q;
                    WF_entry::const_iterator it;

                    // If a second word was not specified, then automatically choose the most frequent one
                    if (sentence.size() == 1) {
                        WF2_map::const_iterator x = wf2.find(last_word); 
                        std::string second_word; 
                        int max_freq = 0; 

                        // Look up the most frequent second used word from the window_size=2 map
                        for (WF_entry::const_iterator y = (x->second).begin(); y != (x->second).end(); y++) {
                            if (y->second > max_freq) {
                                max_freq = y->second; 
                                second_word = y->first; 
                            }
                        }

                        // Make that the next word (which is last_word_2, second starting word)
                        last_word_2 = second_word; 
                        sentence.push_back(second_word);
                        length--;  
                        std::cout << last_word_2 << " "; 
                    }

                    // Setup the other iterators
                    q = (p->second).find(last_word_2); 
                    it = (q->second).begin(); 
                    
                    // Random words
                    if (random_flag) { 
                        int total_freq = 0;     
                        int random; 
                        int counter = 0;   

                        while (it != (q->second).end()) {
                            total_freq += it->second; 
                            it++; 
                        }

                        it = (q->second).begin(); 
                        random = generator.randInt(total_freq-1)+1; 

                        while (it != (q->second).end()) {
                            counter += it->second; 
                            if (counter >= random) {
                                word = it->first;
                                break;
                            }
                            it++; 
                        }

                    }

                    // Most common words
                    else {
                        while (it != (q->second).end()) {
                            if (it->second > max_freq) {
                                max_freq = it->second; 
                                word = it->first; 
                            }
                            it++;
                        }
                    }
                }


                // Print next word
                std::cout << word << " "; 


                // Update last word(s)
                if (sentence.size() == 1) {
                    last_word = word; 
                }
                else if (sentence.size() == 2) {
                    last_word = last_word_2; 
                    last_word_2 = word; 
                }
            }
            std::cout << std::endl << std::endl; 
        }


        // Break the loop to terminate the program
        else if (command == "quit") {
            break;
        } 


        // Unknown command, show an error message
        else {
            std::cout << "WARNING: Unknown command: " << command << std::endl;
        }
    }
    return 0; 
}