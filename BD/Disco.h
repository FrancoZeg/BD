#ifndef DISCO_H
#define DISCO_H

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <tuple>
#include <map>
#include "plato.h"

class Disco {
public:
    std::vector<Plato> platos;
    std::map<std::string, std::vector<std::pair<std::string, std::tuple<int, int, int, int>>>> mapaTuplas;

    Disco(size_t numPlatos, size_t numPistas, size_t numSectores, size_t capacidadSector) {
        for (size_t i = 0; i < numPlatos; ++i) {
            platos.emplace_back(i, numPistas, numSectores, capacidadSector);
        }
    }
    // Función para obtener columnas como una cadena separada por comas
    std::string columnas_a_cadena(const std::vector<std::string>& columnas) const {
        std::ostringstream os;
        for (size_t i = 0; i < columnas.size(); ++i) {
            os << columnas[i];
            if (i < columnas.size() - 1) os << ", ";
        }
        return os.str();
    }
    bool insertar_tuplas(const std::string& nombreTabla, const std::vector<std::pair<std::string, std::string>>& tuplas) {
        size_t dataOffset = 0;
        std::vector<std::pair<std::string, std::tuple<int, int, int, int>>> indices;

        for (const auto& tupla : tuplas) {
            const std::string& clave = tupla.first;
            const std::string& datos = tupla.second;
            std::vector<char> data(datos.begin(), datos.end());
            dataOffset = 0;

            for (size_t platoIdx = 0; platoIdx < platos.size(); ++platoIdx) {
                for (int supIdx = 0; supIdx < 2; ++supIdx) {
                    Superficie& superficie = (supIdx == 0) ? platos[platoIdx].superior : platos[platoIdx].inferior;
                    for (auto& pista : superficie.pistas) {
                        for (size_t sectorIdx = 0; sectorIdx < pista.sectores.size(); ++sectorIdx) {
                            if (pista.sectores[sectorIdx].insertar_datos(data, dataOffset)) {
                                indices.emplace_back(clave, std::make_tuple(platoIdx, supIdx, pista.id, sectorIdx));
                                goto NEXT_TUPLA; // Salir del bucle interno
                            }
                        }
                    }
                }
            }
            if (dataOffset < data.size()) {
                std::cerr << "Error: No hay suficiente espacio para insertar todos los datos.\n";
                return false;
            }
        NEXT_TUPLA:;
        }

        // Combina los nuevos índices con los existentes
        if (mapaTuplas.find(nombreTabla) != mapaTuplas.end()) {
            mapaTuplas[nombreTabla].insert(mapaTuplas[nombreTabla].end(), indices.begin(), indices.end());
        }
        else {
            mapaTuplas[nombreTabla] = indices;
        }

        return true;
    }

    void buscar_tupla(const std::string& nombreTabla, const std::string& clave) const {
        if (mapaTuplas.find(nombreTabla) != mapaTuplas.end()) {
            const auto& tuplas = mapaTuplas.at(nombreTabla);
            for (const auto& tupla : tuplas) {
                if (tupla.first == clave) {
                    auto idx = tupla.second;
                    int platoIdx = std::get<0>(idx);
                    int supIdx = std::get<1>(idx);
                    int pistaIdx = std::get<2>(idx);
                    int sectorIdx = std::get<3>(idx);
                    std::cout << "Clave: " << clave << " encontrada en -> "
                        << "Plato: " << platoIdx << ", Superficie: " << supIdx
                        << ", Pista: " << pistaIdx << ", Sector: " << sectorIdx << "\n";
                    return;
                }
            }
            std::cout << "Clave: " << clave << " no encontrada en la tabla \"" << nombreTabla << "\".\n";
        }
        else {
            std::cout << "La tabla \"" << nombreTabla << "\" no existe en el disco.\n";
        }
    }
};

#endif