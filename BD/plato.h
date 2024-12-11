#ifndef PLATO_H
#define PLATO_H

#include "superficie.h"

class Plato {
public:
    int id;
    Superficie superior;
    Superficie inferior;

    Plato(int id, size_t numPistas, size_t numSectores, size_t capacidadSector)
        : id(id),
        superior(id * 2, numPistas, numSectores, capacidadSector),
        inferior(id * 2 + 1, numPistas, numSectores, capacidadSector) {
    }
};

#endif