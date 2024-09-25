#include "PPM.hpp"
#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

PPM::PPM(std::string fileName) {
    std::ifstream inputFile(fileName);
    if (!inputFile) { std::cerr << "Error: File " << fileName << " cannot be opened" << std::endl; return; }

    const int foundP3 = 1 << 0;
    const int foundDimensions = 1 << 1;
    const int foundRange = 1 << 2;
    int status = 0;

    std::string line;
    int pixelIndex = 0;
    
    while (std::getline(inputFile, line)) {
        if (line[0] == '#') { continue; }
        if (!(status & foundP3)) {
            if (line != "P3") { std::cerr << "Error: P3 not found, file " << fileName << " not supported" << std::endl; return; }
            status |= foundP3;
            continue;
        }
        std::stringstream ss(line);
        if (!(status & foundDimensions)) { ss >> m_width >> m_height; m_PixelData.resize(m_width * m_height * 3); status |= foundDimensions; continue; }
        if (!(status & foundRange)) { ss >> m_maxColorValue; status |= foundRange; continue; }

        int value;
        while (ss >> value) {
            if (pixelIndex < m_PixelData.size()) { m_PixelData[pixelIndex++] = static_cast<uint8_t>(value); }
        }
    }

    inputFile.close();
}


// Destructor deletes(delete or delete[]) any memory that has been allocated
// or otherwise calls any 'shutdown' or 'destroy' routines for this deletion
// to occur.
PPM::~PPM() {
    
}

void PPM::savePPM(std::string outputFileName) const {
    std::ofstream outputFile(outputFileName);
    if (!outputFile) { std::cerr << "Error: Unable to open file " << outputFileName << std::endl; return; }

    outputFile << "P3" << std::endl;
    outputFile << m_width << " " << m_height << std::endl;
    outputFile << m_maxColorValue << std::endl;

    for (int i = 0; i < m_PixelData.size(); ++i) {
        outputFile << static_cast<int>(m_PixelData[i]) << " ";
        if ((i + 1) % (m_width * 3) == 0) { outputFile << std::endl; }
    }

    outputFile.close();
}

// Darken halves (integer division by 2) each of the red, green
// and blue color components of all of the pixels
// in the PPM. Note that no values may be less than
// 0 in a ppm.
void PPM::darken() {
    for (size_t i = 0; i < m_PixelData.size(); i++) {
        int value = static_cast<int>(m_PixelData[i]) / 2;
        m_PixelData[i] = static_cast<uint8_t>(value);
    }
}


// Lighten doubles (integer multiply by 2) each of the red, green
// and blue color components of all of the pixels
// in the PPM. Note that no values may be greater than
// 255 in a ppm.
void PPM::lighten() {
    for (size_t i = 0; i < m_PixelData.size(); i++) {
        int value = static_cast<int>(m_PixelData[i]) * 2;
        m_PixelData[i] = static_cast<uint8_t>(std::min(value, 255));
    }
}

// Sets a pixel to a specific R,G,B value 
// Note: You do not *have* to use setPixel in your implementation, but
//       it may be useful to implement.
void PPM::setPixel(int x, int y, uint8_t R, uint8_t G, uint8_t B) {
    
}