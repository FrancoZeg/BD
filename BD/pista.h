#ifndef PISTA_H
#define PISTA_H

#include <vector>
#include "sector.h"

class Pista {
public:
    int id;
    std::vector<Sector> sectores;

    Pista(int id, size_t numSectores, size_t capacidadSector) : id(id) {
        for (size_t i = 0; i < numSectores; ++i) {
            sectores.emplace_back(capacidadSector);
        }
    }
};

#endif