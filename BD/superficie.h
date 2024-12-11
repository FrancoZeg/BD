#ifndef SUPERFICIE_H
#define SUPERFICIE_H

#include <vector>
#include "pista.h"

class Superficie {
public:
    int id;
    std::vector<Pista> pistas;

    Superficie(int id, size_t numPistas, size_t numSectores, size_t capacidadSector) : id(id) {
        for (size_t i = 0; i < numPistas; ++i) {
            pistas.emplace_back(i, numSectores, capacidadSector);
        }
    }
};

#endif