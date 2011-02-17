
#include "TargaImage.h"
#include <cassert>
#include <iostream>

using std::ifstream;
using std::istream;
using std::string;
using std::vector;

TargaImage::TargaImage() {
	m_width = 0;
	m_height = 0;
	m_bitsPerPixel = 0;
	m_bytesPerPixel = 0;
}

bool TargaImage::isImageTypeSupported(const TargaHeader& header) {
    // If this image is not one of the supported types, 
    // then return false (currently only support color images)
    if (((header.imageTypeCode != TFT_RGB) 
			&& (header.imageTypeCode != TFT_RLE_RGB))
			|| (header.colorMapType != 0)) {
        return false;
    }
    return true;
}

bool TargaImage::isCompressedTarga(const TargaHeader& header) {
    return (header.imageTypeCode == TFT_RLE_RGB
			|| header.imageTypeCode == TFT_RLE_GRAYSCALE);
}

bool TargaImage::isUncompressedTarga(const TargaHeader& header) {
    return (header.imageTypeCode == TFT_RGB 
			|| header.imageTypeCode == TFT_GRAYSCALE);
}

bool TargaImage::load(const string& filename) {
    ifstream fileIn(filename.c_str(), std::ios::binary);

    if (!fileIn.good()) {
        std::cerr << "Could not open the targa image file for reading" << std::endl;
        return false;
    }

    // Read the first 12 bytes of the file
    fileIn.read(reinterpret_cast<char*>(&m_header), sizeof(TargaHeader));

    if (!isImageTypeSupported(m_header)) {
        std::cerr << "This is not a supported image type" << std::endl;
        return false;
    }

    // Store the image width and height
    m_width = m_header.width;
    m_height = m_header.height;

	m_bitsPerPixel = m_header.bpp;
    m_bytesPerPixel = m_header.bpp / 8;

    // RGB = 3, RGBA = 4
    if (m_bytesPerPixel < 3) {
        // We don't support lower color depths
        std::cerr << "Color depth not supported: " << m_bytesPerPixel << std::endl;
        return false;
    }

    // Calculate the size of the image data
    unsigned int imageSize = m_width * m_height * m_bytesPerPixel;

    // Allocate memory for the image data
    m_imageData.resize(imageSize);

    // Skip past the id if there is one
    if (m_header.idLength > 0) {
        // This just ignores the next data
        fileIn.ignore(m_header.idLength);
    }
    bool result = false;

    // If this is an uncompressed image
    if (isUncompressedTarga(m_header)) {
        result = loadUncompressedTarga(fileIn);
    }
    else {
        result = loadCompressedTarga(fileIn);
    }

    // Use the imageDesc field to work out whether we should 
	// flip the data or not
    if ((m_header.imageDesc & TOP_LEFT) == TOP_LEFT) {
        flipImageVertically();
    }
    return result;
}

void TargaImage::unload() {
    m_imageData.clear();
}

bool TargaImage::loadCompressedTarga(istream& fileIn) {
    unsigned int pixelcount	= m_height * m_width; // Number of pixels in the image
    unsigned int currentpixel	= 0; // Current pixel being read
    unsigned int currentbyte	= 0; // Current byte

    vector<unsigned char> colorBuffer(m_bytesPerPixel);

    do {
        unsigned char chunkheader = 0;
        fileIn.read(reinterpret_cast<char*>(&chunkheader), sizeof(unsigned char));
    
        if (chunkheader < 128) {
            chunkheader++;

            for (short counter = 0; counter < chunkheader; counter++) {
                fileIn.read(reinterpret_cast<char*>(&colorBuffer[0]), m_bytesPerPixel);

                m_imageData[currentbyte] = colorBuffer[2];
                m_imageData[currentbyte + 1] = colorBuffer[1];
                m_imageData[currentbyte + 2] = colorBuffer[0];

                if (m_bytesPerPixel == 4) {
                    m_imageData[currentbyte + 3] = colorBuffer[3];
                }
                 
                currentbyte += m_bytesPerPixel;
                currentpixel++;

                if (currentpixel > pixelcount) {
                    return false;
                }
            }
        } 
        else {
            chunkheader -= 127;

            fileIn.read(reinterpret_cast<char*>(&colorBuffer[0]), m_bytesPerPixel);

            for (short counter = 0; counter < chunkheader; counter++) {
                m_imageData[currentbyte] = colorBuffer[2];
                m_imageData[currentbyte + 1] = colorBuffer[1];
                m_imageData[currentbyte + 2] = colorBuffer[0];

                if (m_bytesPerPixel == 4) {
                    m_imageData[currentbyte + 3] = colorBuffer[3];
                }

                currentbyte += m_bytesPerPixel;
                currentpixel++;

                if (currentpixel > pixelcount) {
                    return false;
                }
            }
        }
    } while(currentpixel < pixelcount);
    return true;
}

bool TargaImage::loadUncompressedTarga(istream& fileIn) {
    unsigned int imageSize = m_imageData.size();
    fileIn.read(reinterpret_cast<char*>(&m_imageData[0]), imageSize);

    // Swap the red and blue to make the data RGB instead of BGR
    for (unsigned int swap = 0; swap < imageSize; swap += m_bytesPerPixel) {
        char cswap = m_imageData[swap];
        m_imageData[swap] = m_imageData[swap + 2];
        m_imageData[swap + 2] = cswap;
    }
    return true;
}

unsigned int TargaImage::getWidth() const {
    return m_width;
}

unsigned int TargaImage::getHeight() const {
    return m_height;
}

unsigned int TargaImage::getBitsPerPixel() const {
    return m_bitsPerPixel;
}

const unsigned char* TargaImage::getImageData() const {
    return &m_imageData[0];
}

// Flips the image data vertically, some programs store the 
// image data upside-down so we flip it if it is the 
// wrong way
void TargaImage::flipImageVertically() {
    // Allocate some space to store the flipped data
    vector<unsigned char> flippedData;
    flippedData.reserve(m_imageData.size());

    // This is 3 for RGB, 4 for RGBA
    int step = m_bytesPerPixel;

    // Go backward through the rows of image data
    for (int row = m_height - 1; row >= 0; row--) {
        // Get a pointer to the start of the row
        unsigned char* rowData = &m_imageData[row * m_width * step];
        // Store this row of data one channel at a time
        for (unsigned int i = 0; i < m_width * step; ++i)  {
            flippedData.push_back(*rowData);
            rowData++;
        }
    }
    // Copy the flipped image data to the original array
    m_imageData.assign(flippedData.begin(), flippedData.end());
}
