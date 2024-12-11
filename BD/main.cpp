#include <vector>
#include <string>
#include <tuple>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include "Disco.h"
#include "plato.h"
#include "superficie.h"
#include "pista.h"
#include "sector.h"

// Función para procesar el archivo SQL
std::vector<std::string> procesar_archivo_sql(const std::string& ruta) {
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) {
        throw std::runtime_error("No se pudo abrir el archivo SQL: " + ruta);
    }

    std::vector<std::string> columnas;
    std::string linea;

    // Leer las columnas definidas en el CREATE TABLE
    while (std::getline(archivo, linea)) {
        std::transform(linea.begin(), linea.end(), linea.begin(), ::tolower);
        if (linea.find("create table") != std::string::npos || linea.find(");") != std::string::npos) {
            continue; // Ignorar líneas que no definen columnas
        }
        size_t pos = linea.find(' ');
        if (pos != std::string::npos) {
            columnas.push_back(linea.substr(0, pos)); // Extraer el nombre de la columna
        }
    }
    return columnas;
}
// Función para procesar archivo CSV
std::vector<std::pair<std::string, std::string>> procesar_archivo_csv(const std::string& ruta) {
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) {
        throw std::runtime_error("No se pudo abrir el archivo CSV: " + ruta);
    }

    std::vector<std::pair<std::string, std::string>> tuplas;
    std::string linea;

    while (std::getline(archivo, linea)) {
        std::istringstream iss(linea);
        std::string clave, resto;

        // Leer la clave (primera columna)
        std::getline(iss, clave, ',');
        clave.erase(std::remove(clave.begin(), clave.end(), ' '), clave.end()); // Eliminar espacios

        // Leer el resto de la fila como datos
        std::getline(iss, resto);
        resto.erase(std::remove(resto.begin(), resto.end(), '"'), resto.end()); // Eliminar comillas
        resto.erase(0, resto.find_first_not_of(' ')); // Quitar espacios al inicio

        tuplas.emplace_back(clave, resto);
    }
    return tuplas;
}

// Manejador para la búsqueda
void buscar_y_mostrar(const Disco& disco, const std::string& tabla, const std::string& criterio, const std::string& valor, std::string& resultado) {
    std::ostringstream os;
    if (criterio == "Clave") {
        if (disco.mapaTuplas.find(tabla) != disco.mapaTuplas.end()) {
            const auto& tuplas = disco.mapaTuplas.at(tabla);
            for (const auto& tupla : tuplas) {
                if (tupla.first == valor) {
                    auto idx = tupla.second;
                    os << "Clave: " << valor << " encontrada en -> "
                        << "Plato: " << std::get<0>(idx) << ", Superficie: " << std::get<1>(idx)
                        << ", Pista: " << std::get<2>(idx) << ", Sector: " << std::get<3>(idx);
                    resultado = os.str();
                    return;
                }
            }
        }
        resultado = "Clave no encontrada.";
    }
    else if (criterio == "Índice") {
        try {
            int index = std::stoi(valor);
            if (disco.mapaTuplas.find(tabla) != disco.mapaTuplas.end()) {
                const auto& tuplas = disco.mapaTuplas.at(tabla);
                if (index >= 0 && index < tuplas.size()) {
                    const auto& tupla = tuplas[index];
                    auto idx = tupla.second;
                    os << "Índice: " << index << " encontrado en -> "
                        << "Plato: " << std::get<0>(idx) << ", Superficie: " << std::get<1>(idx)
                        << ", Pista: " << std::get<2>(idx) << ", Sector: " << std::get<3>(idx);
                    resultado = os.str();
                    return;
                }
            }
            resultado = "Índice fuera de rango.";
        }
        catch (...) {
            resultado = "Índice inválido.";
        }
    }
    else {
        resultado = "Criterio no soportado.";
    }
}
// Función para aplicar estilos personalizados
void ApplyCustomStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Fondo y elementos básicos
    colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f); // Fondo de las ventanas
    colors[ImGuiCol_TitleBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);

    // Botones y elementos interactivos
    colors[ImGuiCol_Button] = ImVec4(0.2f, 0.3f, 0.4f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.4f, 0.5f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.4f, 0.5f, 0.6f, 1.0f);

    // Bordes y texto
    colors[ImGuiCol_Border] = ImVec4(0.6f, 0.6f, 0.6f, 0.5f);
    colors[ImGuiCol_Text] = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);

    // Ajustes de estilo
    style.FrameRounding = 5.0f; // Redondeo de esquinas
    style.WindowRounding = 5.0f;
    style.GrabRounding = 5.0f;
}

// Función para cargar fuentes personalizadas
void LoadCustomFonts() {
    ImGuiIO& io = ImGui::GetIO();

    // Cambia estas rutas por rutas válidas en tu sistema
    const char* font1_path = "C:/Users/renat/Downloads/Roboto-Medium.ttf";
    const char* font2_path = "C:/Users/renat/Downloads/Roboto-Bold.ttf";

    if (std::ifstream(font1_path)) {
        io.Fonts->AddFontFromFileTTF(font1_path, 16.0f);
    }
    else {
        std::cerr << "No se encontró la fuente: " << font1_path << "\n";
    }

    if (std::ifstream(font2_path)) {
        io.Fonts->AddFontFromFileTTF(font2_path, 18.0f);
    }
    else {
        std::cerr << "No se encontró la fuente: " << font2_path << "\n";
    }
}


void SetupImGui(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ApplyCustomStyle(); // Aplica el estilo personalizado
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    LoadCustomFonts(); // Carga fuentes personalizadas
}

// Simulación de la función para búsqueda y mostrar resultados

int main() {
    // Configuración de GLFW
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(1200, 800, "Disco DB GUI", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Inicializar ImGui
    SetupImGui(window);

    // Configuración del disco
    size_t numPlatos = 2, numPistas = 4, numSectores = 8, capacidadSector = 64;
    Disco disco(numPlatos, numPistas, numSectores, capacidadSector);

    // Procesar archivos (ruta ajustable)
    std::string ruta_sql = "C:/Users/renat/Downloads/struct_table.txt";
    auto columnas = procesar_archivo_sql(ruta_sql);
    std::string ruta_csv = "C:/Users/renat/Downloads/taxables.csv";
    auto tuplas_csv = procesar_archivo_csv(ruta_csv);
    disco.insertar_tuplas("PRODUCTO", tuplas_csv);

    // Variables para la GUI
    char clave[64] = "";
    char datos[256] = "";
    char valorBusqueda[64] = "";
    std::string resultadoBusqueda;
    std::string resultadoEstado = "Listo para operar.";

    // Opciones para el Combo
    const char* opcionesCriterio[] = { "Clave", "Índice" };
    int indiceCriterio = 0; // Índice seleccionado inicialmente

    // Loop de renderizado
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        // Ventana de búsqueda
        ImGui::Begin("Buscar Datos");
        ImGui::Text("Búsqueda de datos:");
        if (ImGui::Combo("Criterio", &indiceCriterio, opcionesCriterio, IM_ARRAYSIZE(opcionesCriterio))) {}
        ImGui::InputText("Valor", valorBusqueda, sizeof(valorBusqueda));
        if (ImGui::Button("Buscar")) {
            buscar_y_mostrar(disco, "PRODUCTO", opcionesCriterio[indiceCriterio], valorBusqueda, resultadoBusqueda);
        }
        ImGui::End();

        // Ventana para resultados
        ImGui::Begin("Resultados de Búsqueda");
        ImGui::TextWrapped("Resultados:");
        ImGui::TextWrapped("%s", resultadoBusqueda.empty() ? "Sin resultados aún." : resultadoBusqueda.c_str());
        ImGui::End();

        // Ventana para agregar datos
        ImGui::Begin("Agregar Datos");
        ImGui::Text("Agregar nueva tupla:");
        ImGui::InputText("Clave", clave, sizeof(clave));
        ImGui::InputTextMultiline("Datos", datos, sizeof(datos));
        if (ImGui::Button("Agregar")) {
            std::vector<std::pair<std::string, std::string>> nuevaTupla = { { clave, datos } };
            disco.insertar_tuplas("PRODUCTO", nuevaTupla);
            resultadoEstado = "Tupla agregada con éxito.";
        }
        ImGui::End();

        // Ventana de estado del sistema
        ImGui::Begin("Estado del Sistema");
        ImGui::TextWrapped("Estado:");
        ImGui::TextWrapped("%s", resultadoEstado.c_str());
        ImGui::End();

        // Renderizado
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Limpiar recursos de ImGui y GLFW
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}