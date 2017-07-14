#ifndef IMAGE_H
#define IMAGE_H

#include <cassert>
#include <string>
#include <iostream>


// 24-bit (RGB) color pixel
class Color 
{
public:
  Color(unsigned char red=255, unsigned char green=255, unsigned char blue=255) : r(red),g(green),b(blue) {}
  bool isWhite() const { return r==255 && g==255 && b==255; }
  bool operator==(const Color& c) { return r==c.r && g==c.g && b==c.b; }
  bool operator!=(const Color& c) { return !(*this == c); }

  unsigned char r, g, b; 
};


// 8-bit offset (dx and dy)
class Offset 	
{
public:
  Offset(unsigned char dx_=0, unsigned char dy_=0) : dx(dx_),dy(dy_) {}
  bool operator==(const Offset& o) { return dx == o.dx && dy == o.dy; }

  unsigned char dx, dy;  
};


// Templated image class, can have several formats
// .ppm (Color), .pbm (bool), .offset (Offset)
template <class T> class Image
{
public:
    Image() : width(0), height(0), data(NULL) {}
    Image(const Image &image) : data(NULL) { this->copy_helper(image); }
    ~Image() { delete [] data; }

    const Image& operator=(const Image &image) 
    { 
	    if (this != &image)
	    	copy_helper(image);
	    return *this; 
    }

	void Allocate(int w, int h) 
	{
		width = w;
		height = h;
		if (data != NULL)
			delete[] data;
		data = (width == 0 && height == 0) ? NULL : new T[width*height]; 
	}
	
	int Width() const { return width; }
  	int Height() const { return height; }
  	const T& GetPixel(const Offset& o) const { return GetPixel(o.dx, o.dy); }
  	const T& GetPixel(int x, int y) const { return data[y*width+x]; }

  	void SetPixel(const Offset& o, const T& value) { SetPixel(o.dx, o.dy, value); }
  	void SetPixel(int x, int y, const T& value) { data[y*width+x] = value; }
	void SetAllPixels(const T& value)
	{
		for (int i = 0; i < width*height; i++) 
		  data[i] = value; 
	}

  	bool Load(const std::string &filename);
  	bool Save(const std::string &filename) const; 
  

private:
	void copy_helper(const Image &image)
	{
		Allocate (image.Width(), image.Height());
		for (int i = 0; i < image.Width(); i++) {
			for (int j = 0; j < image.Height(); j++)
		    	this->SetPixel(i,j,image.GetPixel(i,j));
		}
	}

	int width;
	int height;
	T *data;
};


#endif