#include <iostream>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <map>
#include "image.h"


// Helper function for the sorting algorithm used in SetupOffsets function
inline bool SortByMaxMappings(const std::pair<Offset, int>& a, const std::pair<Offset, int>& b)
{
    return a.second > b.second;
}


// Returns a vector containing the number of pixels mapping to each offset cell, given the pixels and offset size
std::vector<std::pair<Offset, int> > SetupOffsets(const std::vector<Offset>& pixels, int offset_size)
{
    // ** Note: have to use vector of pairs instead of map because sorting by freq DESC with maps doesn't work
    std::vector<std::pair<Offset, int> > result; 

    // Record the number of pixels that map to each cell in the offset data
    for (int c = 0; c < pixels.size(); c++) {
        Offset cell(pixels[c].dx % offset_size, pixels[c].dy % offset_size);
        bool found = false; 

        for (int d = 0; d < result.size(); d++) {
            if (result[d].first == cell) {
                result[d].second++; 
                found = true; 
                break;
            }
        }

        if (!found)
            result.push_back(std::make_pair(cell, 1)); 
    }

    std::sort(result.begin(), result.end(), SortByMaxMappings);
    return result; 
}


// Checks if the shift (SX, SY) combination works, if it doesn't cause any collisions then it returns true, otherwise false
bool validSolution(int sx, int sy, int c, int offset_size, const std::vector<std::pair<Offset, int> >& cell_mappings, 
    const Image<Color>& input, const Image<Color>& hash_data)
{
    int hash_size = hash_data.Width();
    int width = input.Width(), height = input.Height();

    for (int x = cell_mappings[c].first.dx; x < width; x += offset_size) { 
        for (int y = cell_mappings[c].first.dy; y < height; y += offset_size) {
            Color pixel_color = input.GetPixel(x, y); 
            Color hash_color = hash_data.GetPixel((x + sx) % hash_size, (y + sy) % hash_size);

            // Did we find a collision in the hash data? 
            if (!pixel_color.isWhite() && !hash_color.isWhite() && pixel_color != hash_color)
                return false; 
        }
    }
    return true;
}


// Initializes a generic Image object of any type to a given size and fills it with default values
template<class T> void Initialize(Image<T>& container, int width, int height, const T& default_value)
{
    container.Allocate(width, height); 
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++)
            container.SetPixel(x, y, default_value);
    } 
}


// Given image data, creates the compressed version of the image (three pieces of data)
void Compress(const Image<Color>& input, Image<bool>& occupancy, Image<Color>& hash_data, Image<Offset>& offset) 
{
    int width = input.Width(), height = input.Height(); 
    int count_nonwhite = 0; 
    int hash_size, offset_size; 
    std::vector<Offset> pixels; 

    // Count the number of non-white pixels in input, and record them into the pixels vector too
    for (int x = 0; x < width; x++) {
        for (int y  = 0; y < height; y++) {
            if (!input.GetPixel(x, y).isWhite()) {
                pixels.push_back(Offset(x, y)); 
                count_nonwhite++; 
            }
        }
    }

    // Calculate size of table for hash data and offset
    hash_size = ceil(sqrt(1.01 * count_nonwhite)); 
    offset_size = ceil(sqrt(count_nonwhite / 4)); 

    // Initialize internal arrays for the three outputted Image objects
    Initialize(occupancy, width, height, false);
    Initialize(hash_data, hash_size, hash_size, Color(255, 255, 255)); 
    Initialize(offset, offset_size, offset_size, Offset(0, 0)); 

    // Output the occupancy data
    for (int c = 0; c < pixels.size(); c++)
        occupancy.SetPixel(pixels[c], true); 

    // Calculate the number of pixels mapping to each cell in the offset data
    std::vector<std::pair<Offset, int> > cell_mappings = SetupOffsets(pixels, offset_size);

    // Now go through the mappings from highest to lowest and assign a proper offset (shift) for each cell 
    for (int c = 0; c < cell_mappings.size(); c++) {
        Offset shift; 
        bool solution_found = false; 

        // First cell in the offset data automatically gets (0, 0), which is basically no shift
        // For cells after that, a solution needs to be brute forced (sorted of)
        if (c > 0) {
            const int MAX = offset_size; 
            int sx, sy; 

            // Iterate through all possible shift values to possibly find a solution that results in no collisions
            for (sx = 0; sx <= MAX && !solution_found; sx++) {
                for (sy = 0; sy <= MAX; sy++) {
                    if (validSolution(sx, sy, c, offset_size, cell_mappings, input, hash_data) && !(sx == 0 && sy == 0)) {
                        shift.dx = sx;
                        shift.dy = sy;
                        solution_found = true;
                        break;
                    }
                }
            }
        }
        else if (c == 0) {
            shift.dx = shift.dy = 0; 
            solution_found = true; 
        }


        // If no solution was found, then do NOT proceed, skip writing data for this particular cell
        if (!solution_found)
            continue; 

        // Write offset data
        offset.SetPixel(cell_mappings[c].first, shift); 

        // Write hash (color) data
        for (int x = cell_mappings[c].first.dx; x < width; x += offset_size) { 
            for (int y = cell_mappings[c].first.dy; y < height; y += offset_size) {
                Color color = input.GetPixel(x, y);
                if (!color.isWhite())
                    hash_data.SetPixel((x + shift.dx) % hash_size, (y + shift.dy) % hash_size, color); 
            }
        }
    }
}


// Given the compressed input data (three pieces of data), creates the output image data
void Uncompress(const Image<bool>& occupancy, const Image<Color>& hash_data, const Image<Offset>& offset, Image<Color>& output)
{
    int width = occupancy.Width(),
        height = occupancy.Height(); 

    output.Allocate(width, height); 

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            if (!occupancy.GetPixel(x, y)) {
                output.SetPixel(x, y, Color(255, 255, 255)); 
                continue; 
            }

            Offset o = offset.GetPixel(x % offset.Width(), y % offset.Height()); 
            Color c = hash_data.GetPixel((x+o.dx) % hash_data.Width(), (y+o.dy) % hash_data.Height());
            output.SetPixel(x, y, c); 
        }
    }
}


// Takes in two 24-bit color images as input and creates a B&W output image (black = same, white = different)
void Compare(const Image<Color>& input1, const Image<Color>& input2, Image<bool>& output) 
{
    if (input1.Width() != input2.Width() || input1.Height() != input2.Height()) {
        std::cerr << "Error: can't compare images of different dimensions: " 
                 << input1.Width() << "x" << input1.Height() << " vs " 
                 << input2.Width() << "x" << input2.Height() << std::endl;
        return;
    } 

    output.Allocate(input1.Width(),input1.Height());

    int count = 0;
    for (int i = 0; i < input1.Width(); i++) {
        for (int j = 0; j < input1.Height(); j++) {
            Color c1 = input1.GetPixel(i,j);
            Color c2 = input2.GetPixel(i,j);
            if (c1.r == c2.r && c1.g == c2.g && c1.b == c2.b)
                output.SetPixel(i,j,true);
            else {
                count++;
                output.SetPixel(i,j,false);
            }
        }      
    }     

    if (count == 0) std::cout << "The images are identical." << std::endl;
    else std::cout << "The images differ at " << count << " pixel(s)." << std::endl;
}


// To allow visualization of the custom offset image format
void ConvertOffsetToColor(const Image<Offset>& input, Image<Color>& output)
{
    output.Allocate(input.Width(),input.Height());
    for (int i = 0; i < output.Width(); i++) {
        for (int j = 0; j < output.Height(); j++) {
            // grab the offset value for this pixel in the image
            Offset off = input.GetPixel(i,j);
            // set the pixel in the output image
            int dx = off.dx;
            int dy = off.dy;
            assert (dx >= 0 && dx <= 15);
            assert (dy >= 0 && dy <= 15);
            // to make a pretty image with purple, cyan, blue, & white pixels:
            int red = dx * 16;
            int green = dy *= 16;
            int blue = 255;
            assert (red >= 0 && red <= 255);
            assert (green >= 0 && green <= 255);
            output.SetPixel(i,j,Color(red,green,blue));
        }
    }
}


// Prints usage information about the program
void usage(char* executable)
{
    std::cerr << "Usage:  4 options" << std::endl;
    std::cerr << "  1)  " << executable << " compress input.ppm occupancy.pbm data.ppm offset.offset" << std::endl;
    std::cerr << "  2)  " << executable << " Uncompress occupancy.pbm data.ppm offset.offset output.ppm" << std::endl;
    std::cerr << "  3)  " << executable << " compare input1.ppm input2.ppm output.pbm" << std::endl;
    std::cerr << "  4)  " << executable << " visualize_offset input.offset output.ppm" << std::endl;
}


// Main function, entrypoint of the application
int main(int argc, char* argv[]) 
{
    if (argc < 2) { 
        usage(argv[0]); 
        exit(1); 
    }

    if (argv[1] == std::string("compress")) {
        if (argc != 6) { 
            usage(argv[0]); 
            exit(1); 
        }
        
        Image<Color> input;
        Image<bool> occupancy;
        Image<Color> hash_data;
        Image<Offset> offset;

        input.Load(argv[2]);
        Compress(input, occupancy, hash_data, offset);
        occupancy.Save(argv[3]);
        hash_data.Save(argv[4]);
        offset.Save(argv[5]);
    } 

    else if (argv[1] == std::string("uncompress")) {
        if (argc != 6) { 
            usage(argv[0]); 
            exit(1); 
        }

        Image<bool> occupancy;
        Image<Color> hash_data;
        Image<Offset> offset;
        Image<Color> output;

        occupancy.Load(argv[2]);
        hash_data.Load(argv[3]);
        offset.Load(argv[4]);
        Uncompress(occupancy, hash_data, offset, output);
        output.Save(argv[5]);
    } 

    else if (argv[1] == std::string("compare")) {
        if (argc != 5) { 
            usage(argv[0]); 
            exit(1); 
        }
       
        Image<Color> input1;
        Image<Color> input2;    
        Image<bool> output; 

        input1.Load(argv[2]);
        input2.Load(argv[3]);
        Compare(input1,input2,output);
        output.Save(argv[4]);
    } 

    else if (argv[1] == std::string("visualize_offset")) {
        if (argc != 4) { 
            usage(argv[0]); 
            exit(1); 
        }

        Image<Offset> input;
        Image<Color> output; 

        input.Load(argv[2]);
        ConvertOffsetToColor(input,output);
        output.Save(argv[3]);
    } 

    else {
        usage(argv[0]);
        exit(1);
    }

    return 0; 
}