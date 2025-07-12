#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
using namespace std;

void parseContentType() {
    string s;
    getline(cin, s); // Skip boundary
}

void saveUploadedFile(const string& filename) {
    string line;
    ofstream out(filename, ios::binary);

    // Skip headers (until blank line)
    int blankCount = 0;
    while (getline(cin, line)) {
        if (line == "\r" || line.empty()) {
            blankCount++;
            if (blankCount == 2) break;
        }
    }

    // Read actual file content
    while (getline(cin, line)) {
        if (line.find("------WebKitFormBoundary") != string::npos) break;
        out << line << "\n";
    }

    out.close();
}

void readPGM(const string& filename, vector<vector<int>>& image, int& width, int& height, int& maxVal) {
    ifstream file(filename);
    string magic;
    file >> magic >> width >> height >> maxVal;
    image.resize(height, vector<int>(width));
    for (int i = 0; i < height; ++i)
        for (int j = 0; j < width; ++j)
            file >> image[i][j];
    file.close();
}

void writePGM(const string& filename, const vector<vector<int>>& image, int width, int height, int maxVal) {
    ofstream file(filename);
    file << "P2\n" << width << " " << height << "\n" << maxVal << "\n";
    for (const auto& row : image) {
        for (int val : row)
            file << val << " ";
        file << "\n";
    }
}

void applyFilter(const vector<vector<int>>& input, vector<vector<int>>& output,
                 const vector<vector<int>>& kernel, int divisor) {
    int h = input.size(), w = input[0].size();
    output = input;

    for (int i = 1; i < h - 1; ++i) {
        for (int j = 1; j < w - 1; ++j) {
            int sum = 0;
            for (int ki = -1; ki <= 1; ++ki)
                for (int kj = -1; kj <= 1; ++kj)
                    sum += input[i + ki][j + kj] * kernel[ki + 1][kj + 1];
            output[i][j] = max(0, min(255, sum / divisor));
        }
    }
}

int main() {
    cout << "Content-Type: text/html\n\n";

    char* contentLengthStr = getenv("CONTENT_LENGTH");
    if (!contentLengthStr) {
        cout << "<h2>Error: No content length</h2>";
        return 1;
    }

    int contentLength = atoi(contentLengthStr);
    cin.ignore(); // Skip line

    // Save uploaded file
    saveUploadedFile("input.pgm");

    // Read filter type from stdin again
    string postData;
    getline(cin, postData); // Contains `filter=blur` or `filter=sharpen`

    string filterType = postData.find("sharpen") != string::npos ? "sharpen" : "blur";

    // Process image
    int width, height, maxVal;
    vector<vector<int>> image, output;
    readPGM("input.pgm", image, width, height, maxVal);

    vector<vector<int>> blurKernel = {
        {1, 1, 1},
        {1, 1, 1},
        {1, 1, 1}
    };

    vector<vector<int>> sharpenKernel = {
        { 0, -1,  0},
        {-1,  5, -1},
        { 0, -1,  0}
    };

    if (filterType == "blur")
        applyFilter(image, output, blurKernel, 9);
    else
        applyFilter(image, output, sharpenKernel, 1);

    writePGM("output.pgm", output, width, height, maxVal);

    // Response
    cout << "<h2>Filter applied: " << filterType << "</h2>";
    cout << "<p><a href='/output.pgm' target='_blank'>Download output.pgm</a></p>";

    return 0;
}
