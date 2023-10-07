#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <cstdlib>
#include <fstream>

bool intersects(sf::Sprite& r1, sf::Sprite& r2) {
    sf::IntRect f1(sf::Vector2i(r1.getPosition()), sf::Vector2i(60, 60));
    sf::IntRect f2(sf::Vector2i(r2.getPosition()), sf::Vector2i(60, 60));
    if (f1.intersects(f2))return true;
    return false;
}

void saveBestScore(int score) {
    std::ofstream file("Best_score.txt");
    file << score;
    file.close();
}

std::string loadBestScore() {
    std::ifstream file("Best_score.txt");
    std::string text;
    getline(file, text);
    file.close();
    return text;
}

class Background {                              
    sf::RectangleShape background;
    sf::RectangleShape gameArea;

public:
    Background() {
        background.setSize(sf::Vector2f(800.f, 1000.f));
        background.setFillColor(sf::Color(180, 0, 0));
        background.setOutlineThickness(-2.f);
        background.setOutlineColor(sf::Color::Black);
        gameArea.setSize(sf::Vector2f(660.f, 960.f));
        gameArea.setFillColor(sf::Color(48, 48, 48));
        gameArea.setOutlineThickness(-2);
        gameArea.setOutlineColor(sf::Color::Black);
        gameArea.setPosition(sf::Vector2f(20.f, 20.f));
    }

    void draw(sf::RenderWindow& window) {
        window.draw(background);
        window.draw(gameArea);
    }

    sf::FloatRect getBounds()const {
        return gameArea.getGlobalBounds();
    }
};

struct Blocks {
    enum {
        Red = 0,
        Blue = 1,
        Yellow = 2,
        Magenta = 3,
        Green = 4
    };

    std::vector<sf::Sprite> blocks;
    sf::Color color;
    sf::Color initialColor;
    static short prevColor;
    sf::Texture texture;
    sf::Sprite sprite;

    Blocks() {
        texture.loadFromFile("block.png");
        chooseColore();
        sprite.setTexture(texture);
        sprite.setTextureRect(sf::IntRect(0, 0, 60, 60));
        blocks.push_back(sprite);
        blocks.push_back(sprite);
        blocks.push_back(sprite);
        blocks.push_back(sprite);
        setColor();
    }

    void chooseColore() {
        short rando = rand() % 5;
        prevColor = rando == prevColor ? (rando + 1) % 5 : rando;
        switch (prevColor) {
        case Red:
            color = sf::Color::Red;
            break;
        case Blue:
            color = sf::Color(30, 60, 255);
            break;
        case Yellow:
            color = sf::Color::Yellow;
            break;
        case Magenta:
            color = sf::Color::Magenta;
            break;
        case Green:
            color = sf::Color::Green;
            break;
        }
        initialColor = color;
    }

    void setColor() {
        blocks[0].setColor(color);
        blocks[1].setColor(color);
        blocks[2].setColor(color);
        blocks[3].setColor(color);
    }

    void setPos(short index, short x, short y) {
        blocks[index % 4].setPosition(sf::Vector2f(x, y));
    }

    void levelDown(short y) {
        for (short i = 0; i < blocks.size(); i++) {
            if (blocks[i].getPosition().y < y) {
                blocks[i].setPosition(sf::Vector2f(blocks[i].getPosition().x, blocks[i].getPosition().y + 60));
            }
        }
    }

    void remove(short y) {
        for (short i = 0; i < blocks.size(); i++) {
            if (blocks[i].getPosition().y == y) {
                blocks.erase(blocks.begin() + i);
                i--;
            }
        }
    }

    bool isIn(sf::Vector2f vec)const {
        for (short i = 0; i < blocks.size(); i++) {
            if (vec == blocks[i].getPosition()) {
                return true;
            }
        }
        return false;
    }

    sf::Vector2f getPos(short index)const {
        return blocks[index % 4].getPosition();
    }

    void draw(sf::RenderWindow& window)const {
        for (sf::Sprite r : blocks) {
            window.draw(r);
        }
    }

    void changeColor() {
        color = sf::Color((color.r >= 40 ? color.r - 20 : color.r), (color.g >= 40 ? color.g - 20 : color.g), (color.b >= 40 ? color.b - 20 : color.b));
        setColor();
    }

    void setInitialColor() {
        color = initialColor;
        setColor();
    }
};

short Blocks::prevColor = -1;



class Shape_ {
    Blocks squares;
    short angle;
public:
    Shape_() : angle(0) {};

    virtual void move(unsigned short x, unsigned short y) = 0;

    Blocks& shape() {
        return squares;
    }

    sf::Sprite& shape(short pos) {
        return squares.blocks[pos];
    }

    short getAngle()const {
        return angle;
    }

    void increaseAngle(short angle = 90) {
        this->angle += angle;
        this->angle %= 360;
    }

    void rotate() {
        increaseAngle();
    }

    void rotateBack() {
        increaseAngle(-90);
    }
};



class Z: public Shape_ {
public:
    Z(unsigned short x, unsigned short y, short angle = 0) {
        increaseAngle(angle);
        shape().setPos(0, x - 60, y);
        shape().setPos(1, x, y);
        shape().setPos(2, x, y + 60);
        shape().setPos(3, x + 60, y + 60);
    };

    void move(unsigned short x, unsigned short y)override {
        if (getAngle() % 180 == 0) {
            shape().setPos(0, x - 60, y);
            shape().setPos(1, x, y);
            shape().setPos(2, x, y + 60);
            shape().setPos(3, x + 60, y + 60);
        }
        else {
            shape().setPos(0, x , y);
            shape().setPos(1, x, y + 60);
            shape().setPos(2, x - 60, y + 60);
            shape().setPos(3, x - 60, y + 120);
        }
    }
};

class S : public Shape_ {
public:
    S(unsigned short x, unsigned short y, short angle = 0) {
        increaseAngle(angle); 
        shape().setPos(0, x - 60, y + 60);
        shape().setPos(1, x , y + 60);
        shape().setPos(2, x , y);
        shape().setPos(3, x + 60, y);
    };

    void move(unsigned short x, unsigned short y)override {
        if (getAngle() % 180 == 0) {
            shape().setPos(0, x - 60, y + 60);
            shape().setPos(1, x, y + 60);
            shape().setPos(2, x, y);
            shape().setPos(3, x + 60, y);
        }
        else {
            shape().setPos(0, x  , y);
            shape().setPos(1, x , y + 60); 
            shape().setPos(2, x + 60, y + 60);
            shape().setPos(3, x + 60, y + 120);
        }
    }
};

class O : public Shape_ {
public:
    O(unsigned short x, unsigned short y, short angle = 0) {
        increaseAngle(angle);
        shape().setPos(0, x - 60, y);
        shape().setPos(1, x, y);
        shape().setPos(2, x - 60, y + 60);
        shape().setPos(3, x, y + 60);
    };

    void move(unsigned short x, unsigned short y)override {
        shape().setPos(0, x - 60, y);
        shape().setPos(1, x, y);
        shape().setPos(2, x - 60, y + 60);
        shape().setPos(3, x, y + 60);
    }
};

class I : public Shape_ {
public:
    I(unsigned short x, unsigned short y, short angle = 0) {
        increaseAngle(angle);
        shape().setPos(0, x , y);
        shape().setPos(1, x, y + 60);
        shape().setPos(2, x, y + 120);
        shape().setPos(3, x, y + 180);
    };

    void move(unsigned short x, unsigned short y)override {
        if (getAngle() % 180 == 0) {
            shape().setPos(0, x, y);
            shape().setPos(1, x, y + 60);
            shape().setPos(2, x, y + 120);
            shape().setPos(3, x, y + 180);
        }
        else {
            shape().setPos(0, x - 60, y);
            shape().setPos(1, x, y);
            shape().setPos(2, x + 60, y);
            shape().setPos(3, x + 120, y);
        }
    }
};

class L : public Shape_ {
public:
    L(unsigned short x, unsigned short y, short angle = 0) {
        increaseAngle(angle);
        shape().setPos(0, x, y);
        shape().setPos(1, x, y + 60);
        shape().setPos(2, x, y + 120);
        shape().setPos(3, x + 60, y + 120);
    };

    void move(unsigned short x, unsigned short y)override {
        if (getAngle() == 0) {
            shape().setPos(0, x, y);
            shape().setPos(1, x, y + 60);
            shape().setPos(2, x, y + 120);
            shape().setPos(3, x + 60, y + 120);
        }
        else if(getAngle() == 90) {
            shape().setPos(0, x + 60, y);
            shape().setPos(1, x, y);
            shape().setPos(2, x - 60, y);
            shape().setPos(3, x - 60, y + 60);
        }
        else if (getAngle() == 180) {
            shape().setPos(0, x, y + 120);
            shape().setPos(1, x, y + 60);
            shape().setPos(2, x, y);
            shape().setPos(3, x - 60, y);
        }
        else {
            shape().setPos(0, x + 60, y + 60);
            shape().setPos(1, x, y + 60);
            shape().setPos(2, x - 60, y + 60);
            shape().setPos(3, x - 60, y);
        }
    }
};

class J : public Shape_ {
public:
    J(unsigned short x, unsigned short y, short angle = 0) {
        increaseAngle(angle);
        shape().setPos(0, x + 60, y);
        shape().setPos(1, x + 60, y + 60);
        shape().setPos(2, x + 60, y + 120);
        shape().setPos(3, x, y + 120);
    };

    void move(unsigned short x, unsigned short y)override {
        if (getAngle() == 0) {
            shape().setPos(0, x + 60, y);
            shape().setPos(1, x + 60, y + 60);
            shape().setPos(2, x + 60, y + 120);
            shape().setPos(3, x, y + 120);
        }
        else if (getAngle() == 90) {
            shape().setPos(0, x + 60, y + 60);
            shape().setPos(1, x, y + 60);
            shape().setPos(2, x - 60, y + 60);
            shape().setPos(3, x - 60, y);
        }
        else if (getAngle() == 180) {
            shape().setPos(0, x , y + 120);
            shape().setPos(1, x, y + 60);
            shape().setPos(2, x, y);
            shape().setPos(3, x + 60, y);
        }
        else {
            shape().setPos(0, x - 60, y);
            shape().setPos(1, x, y);
            shape().setPos(2, x + 60, y);
            shape().setPos(3, x + 60, y  + 60);
        }
    }
};

class T : public Shape_ {
public:
    T(unsigned short x, unsigned short y,short angle = 0) {
        increaseAngle(angle);
        move(x, y);
    };

    void move(unsigned short x, unsigned short y)override {
        if (getAngle() == 0) {
            shape().setPos(0, x - 60, y);
            shape().setPos(1, x, y);
            shape().setPos(2, x, y + 60);
            shape().setPos(3, x + 60, y);
        }
        else if (getAngle() == 90) {
            shape().setPos(0, x, y);
            shape().setPos(1, x, y + 60);
            shape().setPos(2, x - 60, y + 60);
            shape().setPos(3, x, y + 120);
        }
        else if (getAngle() == 180) {
            shape().setPos(0, x - 60, y + 60);
            shape().setPos(1, x, y + 60);
            shape().setPos(2, x, y);
            shape().setPos(3, x + 60, y + 60);
        }
        else {
            shape().setPos(0, x, y);
            shape().setPos(1, x, y + 60);
            shape().setPos(2, x + 60, y + 60);
            shape().setPos(3, x, y + 120);
        }
    }
};

class PlayableShape{
    sf::Vector2f pos;
    sf::Vector2f Origin;
    std::vector<Shape_*> shape;
public:

    PlayableShape(unsigned short x = 320, unsigned short y = 20) : pos(x, y) {
       Origin = pos;
       switch (rand() % 7) {
       case 0:
           shape.push_back(new Z(pos.x, pos.y, rand() % 4 * 90));
           break;
       case 1:
           shape.push_back(new S(pos.x, pos.y, rand() % 4 * 90));
           break;
       case 2:
           shape.push_back(new O(pos.x, pos.y, rand() % 4 * 90));
           break;
       case 3:
           shape.push_back(new I(pos.x, pos.y, rand() % 4 * 90));
           break;
       case 4:
           shape.push_back(new L(pos.x, pos.y, rand() % 4 * 90));
           break;
       case 5:
           shape.push_back(new J(pos.x, pos.y, rand() % 4 * 90));
           break;
       case 6:
           shape.push_back(new T(pos.x, pos.y, rand() % 4 * 90));
           break;
       }
    };

    void keyS() {
        Origin = pos;
        pos.y += 60;
        shape[0]->move(pos.x, pos.y);
    }

    void keyA() {
        Origin = pos;
        pos.x -= 60;
        shape[0]->move(pos.x, pos.y);
    }

    void keyD() {
        Origin = pos;
        pos.x += 60;
        shape[0]->move(pos.x, pos.y);
    }

    void keyR() {
        shape[0]->rotate();
        shape[0]->move(pos.x, pos.y);
    }

    void origin() {
        pos = Origin;
        shape[0]->move(pos.x, pos.y);
    }

    void rotateBack() {
        shape[0]->rotateBack();
        shape[0]->move(pos.x, pos.y);
    }

    Blocks& getShape() {
        return shape[0]->shape();
    }

    Shape_* getShape_() {
        return shape[0];
    }

    void createNew(unsigned short x = 320, unsigned short y = 20) {
        shape.erase(shape.begin());
        pos.x = x;
        pos.y = y;
        Origin = pos;
        switch (rand() % 7) {
        case 0: 
            shape.push_back(new Z(pos.x, pos.y));
            break;
        case 1:
            shape.push_back(new S(pos.x, pos.y));
            break;
        case 2:
            shape.push_back(new O(pos.x, pos.y));
            break;
        case 3:
            shape.push_back(new I(pos.x, pos.y));
            break;
        case 4:
            shape.push_back(new L(pos.x, pos.y));
            break;
        case 5:
            shape.push_back(new J(pos.x, pos.y));
            break;
        case 6:
            shape.push_back(new T(pos.x, pos.y));
            break;
        }
    }

    void setPos(sf::Vector2f& vec) {
        this->pos = vec;
    }

    sf::Vector2f& getPos() {
        return pos;
    }

    sf::Vector2f& getOrigin() {
        return Origin;
    }

    bool intersect(Shape_* sh) {
        for (sf::Sprite block : shape[0]->shape().blocks) {
            for (sf::Sprite otherBlock : sh->shape().blocks) {
                if (intersects(block, otherBlock))return true;
            }
        }
        return false;
    }
};

class Model {
    std::vector<PlayableShape> shapes;
    PlayableShape shape;
    Background background;
    int score;
public:
    Model() {
        shapes.push_back(shape);
        score = 0;
    };

    void createNewShape() {    
        shapes[shapes.size() - 1] = shape;
        shape.createNew();
        shapes.push_back(shape);
    }

    bool isIntersect() {
        sf::FloatRect bounds = background.getBounds();
        std::vector<sf::Vector2f> vec;
        if (!bounds.contains(shape.getShape().blocks[0].getPosition()) ||
            !bounds.contains(shape.getShape().blocks[1].getPosition()) ||
            !bounds.contains(shape.getShape().blocks[2].getPosition()) ||
            !bounds.contains(shape.getShape().blocks[3].getPosition())) return true;
        short i = 0;
        while (i < shapes.size() - 1) {
            if (shape.intersect(shapes[i++].getShape_())) return true;
        }
        return false;
    }

    void removeRow(short y) {
        for (PlayableShape sh : shapes) {
            sh.getShape().remove(y);
        }
        score += 450;
    }

    bool rowIsFull(short y) {
        bool isIn = false;
        for (short x = 20; x < 660; x += 60, isIn = false) {
            for (short i = 0; i < shapes.size() - 1; i++) {
                if (shapes[i].getShape().isIn(sf::Vector2f(x, y))) {
                    isIn = true;
                    break;
                }
            }
            if (!isIn) {
                return false;
            }
        }
        return true;
    }
    
    void levelDown(short y) {
        for (short i = 0; i < shapes.size() - 1; i++) {
            shapes[i].getShape().levelDown(y);
        }
    }

    Background& getBackground() {
        return background;
    }

    std::vector<PlayableShape>& getShapes() {
        return shapes;
    }

    PlayableShape& getPlayableShape() {
        return shape;
    }

    int getScore() {
        return score;
    }

    void addScore(short value) {
        this->score += value;
    }

    void originAngel() {
        shape.rotateBack();
    }

    void origin() {
        shape.origin();
    }

    sf::Vector2f& getOrigin() {
        return shape.getOrigin();
    }

    sf::Vector2f& getPos() {
        return shape.getPos();
    }

    void keyS() {
       shape.keyS();
    }

    void keyA() {
        shape.keyA();
    }

    void keyD() {
        shape.keyD();
    }

    void keyR() {
        shape.keyR();
    }
};

class View {
    sf::Text text;
    sf::Font font;
    sf::Text textCurrent;
    std::string bestScore;
public:
    View() {
        font.loadFromFile("AGENCYR.TTF");
        text.setFont(font);
        text.setCharacterSize(22);
        bestScore = loadBestScore();
        text.setString("Best score: \n" + bestScore);
        text.setPosition(690, 150);
        text.setFillColor(sf::Color::White);
        textCurrent.setFont(font);
        textCurrent.setCharacterSize(22);
        textCurrent.setPosition(690, 250);
        textCurrent.setFillColor(sf::Color::Black);
    }

    void drawBackground(Model& model, sf::RenderWindow& window) {
        model.getBackground().draw(window);
    }

    void drawGameArea(Model& model, sf::RenderWindow& window) {
        std::vector<PlayableShape> shapes = model.getShapes();
        for (short i = 0; i < shapes.size() - 1; i++) {
            model.getShapes()[i].getShape().draw(window);
        }
        if (std::stoi(bestScore) < model.getScore()) {
            text.setString("Best score: \n" + std::to_string(model.getScore()));
        }
        window.draw(text);
        textCurrent.setString("Current score:\n" + std::to_string(model.getScore()));
        window.draw(textCurrent);
    }

    void drawPlayableShape(Model& model, sf::RenderWindow& window) {
        model.getPlayableShape().getShape().draw(window);
    }

    void gameOver(Model& model, sf::RenderWindow& window) {
        sf::Text t;
        t.setFillColor(sf::Color::White);
        t.setPosition(275, 400);
        t.setFont(font);
        t.setCharacterSize(56);
        t.setString("Game over\nScore:" + std::to_string(model.getScore()));
        window.draw(t);
        window.display();
        sf::sleep(sf::Time(sf::seconds(5)));
        window.close();
    }
};

class Tetris {                                      //MVC. Tetris = Controller
    Model model;
    View view;
    bool wait;
    bool pause;
public:
    Tetris() : pause(false) {};

    void drawGame(sf::RenderWindow& window) {
        view.drawBackground(model, window);
        view.drawGameArea(model, window);
    }

    void drawPlayableShape(sf::RenderWindow& window) {
        model.getPlayableShape().getShape().draw(window);
    }

    void keyS(sf::RenderWindow& window) {
        model.keyS();
        if (model.isIntersect()) {
            if (model.getShapes().size() >= 2) {
                if (model.getShapes()[model.getShapes().size() - 2].getShape().isIn(sf::Vector2f(320, 20))) {
                    view.gameOver(model, window);
                }
            }
            model.origin();
            model.getPlayableShape().getShape_()->shape().setInitialColor();
            model.createNewShape();
            checkDeleteRow();
        }
        model.addScore(1);
        view.drawPlayableShape(model, window);
    }

    void down(sf::RenderWindow& window, sf::Clock& time) {
        model.keyS();
        if (model.isIntersect()) {
            if (model.getShapes().size() >= 2) {
                if (model.getShapes()[model.getShapes().size() - 2].getShape().isIn(sf::Vector2f(320, 20))) {
                    view.gameOver(model, window);
                }
            }
            model.origin();
            model.getPlayableShape().getShape_()->shape().changeColor();
            wait = true;
            if (time.getElapsedTime().asSeconds() >= 1) {
                model.getPlayableShape().getShape_()->shape().setInitialColor();
                model.createNewShape();
                checkDeleteRow();
                wait = false;
            }
        }
        if (!wait) time.restart();
        view.drawPlayableShape(model, window);
    }

    void checkDeleteRow() {
        for (short y = 920; y > 20; y -= 60) {
            if (model.rowIsFull(y)) {
                model.removeRow(y);
                model.levelDown(y);
                y += 60;
            }
        }
    }

    void saveRecord(){
        if (std::stoi(std::string(loadBestScore())) < model.getScore()) {
            saveBestScore(model.getScore());
        }
    }

    void keyA(sf::RenderWindow& window) {
        model.keyA();
        if (model.isIntersect()) {
            model.origin();
        }
        view.drawPlayableShape(model, window);
    }

    void keyD(sf::RenderWindow& window) {
        model.keyD();
        if (model.isIntersect()) {
            model.origin();
        }
        view.drawPlayableShape(model, window);
    }

   void keyR(sf::RenderWindow& window) {
        model.keyR();
        if (model.isIntersect()) {
            model.originAngel();
        }
        view.drawPlayableShape(model, window);
   }

   void keyEscape(sf::RenderWindow& window) {
       pause = !pause;
   }

   bool inPause() {
       return pause;
   }

   void putOnPause() {
       sf::sleep(sf::Time(sf::milliseconds(750)));
   }

   int getSpeed() {
       return (800 - (float(model.getScore()) / 13.f)) > 180 ? (800 - float((model.getScore() / 13.f))) : 180.f;
   }
};


void main()
{
    srand(time(NULL));

    sf::RenderWindow window(sf::VideoMode(800, 1000), "My window");
    window.setFramerateLimit(30);

    Tetris tetris;
    sf::Clock time;

    float speed = tetris.getSpeed();
    while (window.isOpen())
    {
        if (!tetris.inPause()) {
            window.clear(sf::Color(48, 48, 48));
            tetris.drawGame(window);
        }
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                tetris.saveRecord();
                window.close();
            }
            if (window.hasFocus()) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::S) {
                        tetris.keyS(window);
                        time.restart();
                    }
                    else if (event.key.code == sf::Keyboard::A) {
                        tetris.keyA(window);
                    }
                    else if (event.key.code == sf::Keyboard::D) {
                        tetris.keyD(window);
                    }
                    else if (event.key.code == sf::Keyboard::R) {
                        tetris.keyR(window);
                    }
                    else if (event.key.code == sf::Keyboard::Escape) {
                        tetris.keyEscape(window);
                    }
                }
            }
            else {
                tetris.putOnPause();
            }
        }
        if (tetris.inPause()) {
            tetris.putOnPause();
            continue;
        }
        speed = tetris.getSpeed();
        if (time.getElapsedTime().asMilliseconds() > speed && window.hasFocus()) {
            tetris.down(window, time);
        }
        else if (event.type != sf::Event::KeyPressed) {
            tetris.drawPlayableShape(window);
        }
        window.display();
    }
}