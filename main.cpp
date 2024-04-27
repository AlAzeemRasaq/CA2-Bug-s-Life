#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include "Board.h"
#include "Bug.h"
#include "CrawlerBug.h"
#include "HopperBug.h"
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;

// functions
void boardOperation(const std::vector<Bug*>& bugs);
void parseLine(const string& strLine);
void inFileStream(vector<Bug*> &bugs); void outFileStream();

// board
struct tile {
    CircleShape shape;
    bool isSelected = false;
    tile() {
        shape.setRadius(20);
    }

    bool contains(int x, int y) {
        int shapeX = ((int)shape.getPosition().x/50) * 50,
                shapeY = ((int)shape.getPosition().y/50) * 50;
        cout << shapeX << ", " << shapeY << endl;
        return x == shapeX && y == shapeY;
    }

    Vector2f getPosition() {
        int shapeX = ((int)shape.getPosition().x/50) * 50,
                shapeY = ((int)shape.getPosition().y/50) * 50;
        return Vector2f (shapeX, shapeY);
    }
};
bool tileEmpty(int x, int y);
void mouseReleased(int x, int y);
void createTiles();
vector<tile> tiles;
tile *selectedTile = nullptr;

int main() {
    vector<Bug*> bugs;
   // outFileStream();

   // initialises the bugs
    inFileStream(bugs);
    cout << "Initialising: " << bugs.size() << " bugs in file:" << endl;

    std::ifstream file("bugs.txt");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::cout << line << std::endl;

            HopperBug* bug = new HopperBug();
            std::stringstream ss(line);
            ss >> bug->type;
            bugs.push_back(bug);
        }
        file.close();
    } else {
        std::cerr << "Unable to open file\n";
        return 1;
    }

    boardOperation(bugs);
    createTiles();

    int menu;
    do {
        std::cout
                << "Menu:\n"
                   "1. Initialise the bug board\n"
                   "2. Display all Bugs\n"
                   "3. Find a Bug\n"
                   "4. Display Life History of all Bugs\n"
                   "5. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> menu;

        switch(menu) {
            case 1:

                break;
            case 2:
                boardOperation(bugs);
                createTiles();

                break;
            case 3:
                int searchID;
                std::cout << "Enter Bug ID to find: ";
                std::cin >> searchID;
                for (const auto& bug : bugs) {
                    if (bug->getId() == searchID) {
                        cout << "Bug found!" << endl;
                        break;
                    } else {
                        cout << "Bug not found" << endl;
                        break;
                    }
                }
                break;
            case 4:


                break;
            case 5:
                std::cout << "Exiting Bug's Life...\n";
                
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    } while (menu > 0 && menu <= 8);

    return 0;
}

void outFileStream() { // outputs bugs from a file named 'bugs.txt'
    ofstream outFile("bugs.txt");

    if ( outFile.good() ) {
        outFile << "Testing for bug history";

        cout << "File has been written." << endl;
    }
}

void parseLine(const string& strLine, vector<Bug*> &bugs) {
    stringstream strStream( strLine );
    const char DELIMITER = ';';
    string type, temp;
    int id, x, y, direction, size;
    // extract the first token (the name) into the name variable
    // - a comma delimits the strings  -
    getline(strStream, type, DELIMITER);
    getline(strStream, temp, DELIMITER);
    id = stoi(temp);
    getline(strStream, temp, DELIMITER);
    x = stoi(temp);
    getline(strStream, temp, DELIMITER);
    y = stoi(temp);
    getline(strStream, temp, DELIMITER);
    direction = stoi(temp);
    getline(strStream, temp, DELIMITER);
    size = stoi(temp);

    if(type == "C") // gets a crawler bug
    {
        Bug* c = new CrawlerBug(type, id, make_pair(x, y), Direction::North, size);
        bugs.push_back(c);
    }
    if(type == "H") // gets a hopper bug
    {
        getline(strStream, temp, DELIMITER);
        int len = stoi(temp);
        Bug* c = new HopperBug(type, id, make_pair(x, y), Direction::North, size, len);
        bugs.push_back(c);
    }
}
void inFileStream(vector<Bug* > &bugs) { // reads bugs from a file named 'bugs.txt'
    std::string filename = "bugs.txt";

    ifstream inFileStream(filename); // open file as input file stream (from working directory)

    if ( inFileStream.good() )  // if file opened successfully, and not empty
    {
        string line;            // create a string object to store a line of text

        while ( getline(inFileStream, line) )   // read a line until false returned , getline() from <string> library
        {
            parseLine( line, bugs );  // pass the line of text to have it parsed
        }
        inFileStream.close();
    }
    else
        cout << "Unable to open file, or file is empty.";
}

void boardOperation(const std::vector<Bug*>& bugs) { // SFML board
    sf::RenderWindow window(sf::VideoMode(500, 500), "Now displaying bug board.");

    vector<sf::RectangleShape> bg;
    for (int r = 0; r < 10; r++) {
        for (int c = 0; c < 10; c++) {
            sf::RectangleShape shape;
            shape.setPosition(r*50, c*50);
            shape.setSize(Vector2f(50, 50));
            shape.setFillColor((r + c) % 2 == 0 ? Color::Green : Color::Yellow);
            bg.push_back(shape);
        }
    }

    ifstream file("bugs.txt");
    if (file.is_open()) {
        string line;
        vector<Bug*> bugs;

        while (getline(file, line)) {
            stringstream ss(line);
            string type;
            int id, x, y, size;
            pair<int, int> position;
            Direction direction;

            ss >> type >> id >> x >> y >> size;
            position = make_pair(x, y);

            Bug* bug = new Bug(type, id, position, direction, size);
            bugs.push_back(bug);
        }

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    cout << "Goodbye." << endl;
                    window.close();
                }
            }
            window.clear();
            for (RectangleShape &s: bg) {
                window.draw(s);
            }

            for (const auto &bug: bugs) {
                sf::CircleShape circle(25);
                if (bug->type[0] == 'C') {
                    circle.setFillColor(sf::Color::Red);
                } else if (bug->type[0] == 'H') {
                    circle.setFillColor(sf::Color::Blue);
                } else {
                    circle.setFillColor(sf::Color::White);
                }
                window.draw(circle);
            }

            window.display();
        }
    }
}

void createTiles() {
    // display bugs on the board
    for (int r = 0; r < 10; r++) {
        for (int c = 0; c < 10; c++) {
            tile t;
            sf::CircleShape bug(25);
            bug.setFillColor(sf::Color::Cyan);
            bug.setPosition(c*50+5, (r*50)+5);
        }
    }
}
