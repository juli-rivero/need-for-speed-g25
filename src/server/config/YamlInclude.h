#pragma once
#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <string>

// Combina nodos YAML recursivamente
inline void mergeYaml(YAML::Node base, const YAML::Node& other) {
    for (auto it : other) {
        const std::string key = it.first.as<std::string>();
        const YAML::Node& val = it.second;

        // Si ambos son mapas → merge recursivo
        if (base[key] && base[key].IsMap() && val.IsMap()) {
            mergeYaml(base[key], val);
        } else {
            // Caso general: override
            base[key] = val;
        }
    }
}

// Carga YAML con soporte para include
inline YAML::Node loadYamlWithInclude(const std::string& path) {
    // Convertimos a path absoluto
    std::filesystem::path mainPath = std::filesystem::absolute(path);
    YAML::Node root = YAML::LoadFile(mainPath.string());

    // Si no hay include → devolvemos root
    if (!root["include"]) return root;

    // Ruta escrita en el include
    std::string incFile = root["include"].as<std::string>();

    // Construimos ruta absoluta relativa al YAML actual
    std::filesystem::path incPath = mainPath.parent_path() / incFile;

    // Cargar archivo base
    YAML::Node base = YAML::LoadFile(incPath.string());

    // Combinar base + override
    mergeYaml(base, root);

    return base;
}
