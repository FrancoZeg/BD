#ifndef SECTOR_H
#define SECTOR_H

#include <vector>

class Sector {
public:
    std::vector<char> datos;
    size_t capacidad;
    size_t espacioLibre;

    Sector(size_t size) : capacidad(size), espacioLibre(size) {}

    bool insertar_datos(const std::vector<char>& data, size_t& dataOffset) {
        size_t dataSize = data.size() - dataOffset;

        while (dataSize > 0 && espacioLibre > 0) {
            size_t fragmentSize = std::min(dataSize, espacioLibre);

            datos.insert(datos.end(), data.begin() + dataOffset, data.begin() + dataOffset + fragmentSize);
            espacioLibre -= fragmentSize;
            dataOffset += fragmentSize;
            dataSize -= fragmentSize;

            if (dataSize == 0) return true; // Dato completamente insertado
        }
        return false; // Aún queda dato por insertar
    }
};

#endif