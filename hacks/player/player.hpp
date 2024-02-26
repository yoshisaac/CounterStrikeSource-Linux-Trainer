#include <string>

class player {
private:
  int index;
  
  int health;
  std::string name;
  float pitch;
  float yaw;
  float x, y, z;

public:
  player(int index, int health, std::string name, float pitch, float yaw, float x, float y, float z) {
    this->index = index;
    this->health = health;
    this->name = name;
    this->pitch = pitch;
    this->yaw = yaw;
    this->x = x;
    this->y = y;
    this->z = z;
  }
};
