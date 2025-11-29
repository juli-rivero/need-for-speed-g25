#pragma once
#include <string>

#include "common/structs.h"

// TODO(nico): esto lo hice temporalmente para no repetir codigo en el
// selectingWindow y en el waitingWindow, para mi esto deberia ser un widget y
// mostrar imagenes de los coches, en vez de strings
class CarSprite {
   public:
    static std::string getSprite(CarType car);
};
