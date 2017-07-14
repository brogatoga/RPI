/**
 * Homework 1 - Text Manipulation
 * Ryan Lin (linr2@rpi.edu)
 * 
 * This program is used to change the alignment of text. It can switch
 * between three different modes including flush left, flush right, and
 * full justify. You can also make it format text in a half triangle instead
 * of a rectangle by passing in "triangle" as the fifth command line argument. 
 *
**/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>

#define FLUSH_LEFT    1
#define FLUSH_RIGHT   2
#define FULL_JUSTIFY  3


// Function prototypes
void draw_spaces(std::ofstream &, int);
void read_words(std::vector<std::string> &, std::ifstream &, int);
void output_words(const std::vector<std::string> &, std::ofstream &, int, int, char, char);
std::vector<std::string> get_word(std::string);


// Main function, entrypoint of the application
int main(int argc, char *argv[])
{
	std::ifstream input(argv[1]);
	std::ofstream output(argv[2]);
	std::vector<std::string> lines; 
	int width = 0; 
	int alignment = 0; 
	char border_v = '|'; 
	char border_h = '-';  


	// Are there a total of five arguments? (one for program name plus four for actual arguments)
	if (argc < 5) {
		std::cerr << "Usage: " << argv[0] << " <input_file> <output_file> <text_width> <text_alignment> [<shape>]" << std::endl;
		return 1; 
	}


	// Can the input file be opened and read from?
	if (!input.good()) {
		std::cerr << "Error: unable to open input file " << argv[1] << std::endl;
		return 1; 
	}


	// Can the output file be opened and written to?
	if (!output.good()) {
		std::cerr << "Error: unable to open output file " << argv[2] << std::endl;
		return 1; 
	}


	// Is the text width a positive non-zero integer?
	if ((width = atoi(argv[3])) <= 0) {
		std::cerr << "Error: value " << width << " is NOT a valid text width" << std::endl;
		return 1; 
	}


	// Set alignment mode, and if it is invalid, then throw an error
	std::string alignstr(argv[4]);
	if (alignstr == "flush_left")        alignment = FLUSH_LEFT;
	else if (alignstr == "flush_right")  alignment = FLUSH_RIGHT;
	else if (alignstr == "full_justify") alignment = FULL_JUSTIFY;
	else {
		std::cerr << "Error: '" << alignstr << "' is NOT a valid alignment method" << std::endl;
		return 1; 
	}


	// (Extra credit) If "starbox" is set for sixth argument, then make a starbox instead of a rectangle
	if (argc >= 6 && std::string(argv[5]) == "starbox") 
		border_v = border_h = '*'; 


	// Read words from the input file and store them into the "lines" vector 
	read_words(lines, input, width); 


	// Output the formatted lines of text to file
	output_words(lines, output, width, alignment, border_v, border_h); 


	// Finished, return zero to signal exit success
	return 0; 
}


// Draws a specified number of draw_spaces
void draw_spaces(std::ofstream &output, int count)
{
	if (count > 0)
		output << std::string(count, ' '); 
}


// Splits a string by spaces and returns a vector containing the individual words
std::vector<std::string> get_word(std::string str)
{
	std::vector<std::string> words; 
	std::string current_word; 
	int length = str.length();        // Save length to prevent multiple calls to it during the loop

	for (int c = 0; c < length; c++) {
		char ch = str[c]; 

		if ((ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r') && current_word.length() > 0) {
			words.push_back(current_word);
			current_word = ""; 
		}
		else current_word += ch; 
	}

	// Get last word if there are no trailing spaces at end of str
	if (current_word.length() > 0)
		words.push_back(current_word);

	return words; 
}


// Read words from the input file, and build a vector of strings containing the formatted lines
void read_words(std::vector<std::string> &lines, std::ifstream & input, int width)
{
	std::string current_line; 
	std::string current_word; 
	int word_length = 0; 
	int line_length = 0; 
	bool first_word = true; 
	bool long_word = false; 


	// Keep reading until end of file
	while (!input.eof()) {

		// Clear current word after it's been used (BUGFIX), but only if a long word is NOT being processed
		if (!long_word)
			current_word = "";

		// If a word > width is currently being processed, then do NOT read the next word
		if (!long_word)
			input >> current_word; 


		// If word is empty, then skip over it
		if ((word_length = current_word.length()) == 0)
			continue; 


		// If word can fit into the current line, then add it
		if ((first_word && line_length + word_length <= width) || (!first_word && line_length + word_length + 1 <= width)) {

			long_word = false;


			// If this is first word in line, then add it normally
			if (first_word) {                
				line_length += word_length; 
				current_line += current_word; 
				first_word = false; 
			}

			// For words after the first, insert a space before appending the word
			else {
				line_length += (word_length + 1); 
				current_line += (" " + current_word); 
			}
		}


		// If word is greater than width, then split it up with dashes onto multiple lines
		else if (word_length > width) {
			long_word = true;
			int cut = width - 1;    // Number of characters to "cut" out of long word (minus one for the dash)

			// If the current line is not empty, then we will have to skip to a new empty line to insert this long word
			if (current_line.length() > 0) 
				lines.push_back(current_line);

			// Fill the new line and then "cut" the part of the string added out of the current word
			current_line = (current_word.substr(0, cut) + "-");
			current_word = current_word.substr(cut);

			// Add the filled line to the vector and then create a new line again
			// Since long_word flag is set, then if word_length > width still holds on the
			// next iteration, then this else-if block will run again. 
			lines.push_back(current_line);
			current_line = "";
			line_length = 0;
			first_word = true;
		} 


		// Otherwise, add current line to vector and start a new line
		else {
			long_word = false;
			lines.push_back(current_line);
			current_line = current_word;
			line_length = word_length;
		}
	}

	// Add last line (if not empty) to the vector
	if (current_line.length() > 0) {
		lines.push_back(current_line); 
	}
}


// Output the formatted words to the output stream
void output_words(const std::vector<std::string> &lines, std::ofstream &output, int width, int alignment, char border_v, char border_h)
{
	output << std::string(width + 4, border_h) << std::endl;

	int lines_size = lines.size();                   // Save copy to prevent multiple calls to it during loop

	for (int c = 0; c < lines_size; c++) {
		output << border_v << " "; 

		if (alignment == FLUSH_LEFT || (alignment == FULL_JUSTIFY && c == lines_size - 1)) {
			output << lines[c]; 
			draw_spaces(output, width - lines[c].length()); 
		}
		else if (alignment == FLUSH_RIGHT) {
			draw_spaces(output, width - lines[c].length()); 
			output << lines[c]; 
		}
		else {    
			std::vector<std::string> words(get_word(lines[c]));
			int words_size = words.size(); 
			int count_spaces = words_size - 1; 
			int remaining_spots = width - lines[c].length() + count_spaces; 


			// If there is only one word, then left-justify it and move on
			if (count_spaces == 0) {     
				output << words[0]; 
				draw_spaces(output, width - words[0].length()); 
			}

			// Otherwise, divide remaining spaces by the number of spaces to compute space size for each space
			else {

				// If the two numbers divide evenly, then divide them to get size of space
				if (remaining_spots % count_spaces == 0) { 
					int space_size = remaining_spots / count_spaces;
					int d; 

					for (d = 0; d < words_size - 1; d++) {
						output << words[d]; 
						draw_spaces(output, space_size); 
					}
					output << words[d];
				}

				// If not, then calculate number of normal and extended spaces to write
				else {
					// There are two kinds of spaces, normal spaces and extended spaces
					// Size of normal space = floor(remaining_spots / count_spaces)
					// Size of extended space = size of normal space + 1
					// Let CN = # of normal spaces, SN = size of normal spaces
					// Let CX = # of extended spaces, SX = size of extended spaces
					// ** Solve for CN and CX such that (CN*SN)+(CX*SX) = remaining_spots and CN+CX=count_spaces **
					// The leftmost CX spaces will be extended spaces, and
					// the remaining CN spaces will be normal spaces. 

					bool found_solution = false; 
					int SN = remaining_spots / count_spaces; 
					int SX = SN + 1; 
					int CN, CX; 
					int count_extended; 
					int d, calc; 


					// Loop until a solution is found
					for (CN = 0; !found_solution; CN++) {
						for (CX = 0; !found_solution; CX++) {
								calc = (CN*SN) + (CX*SX);

								if (calc == remaining_spots && CN + CX == count_spaces) {
									count_extended = CX;
									found_solution = true;
									break;
								}
								else if (calc > remaining_spots)
									break;
						}
					}

					for (d = 0; d < words_size - 1; d++) {
						output << words[d]; 
						draw_spaces(output, d < count_extended ? SX : SN); 
					}
					output << words[d]; 
				}
			}
		}
		output << " " << border_v << std::endl;
	} 
	output << std::string(width + 4, border_h) << std::endl;
}