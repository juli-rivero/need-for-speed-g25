# 🏎️ Need for Speed - Remake (G25)

![C++](https://img.shields.io/badge/C++-17-00599C?logo=c%2B%2B&logoColor=white)
![Qt](https://img.shields.io/badge/Qt-6-41cd52?logo=qt&logoColor=white)
![SDL2](https://img.shields.io/badge/SDL2-2.32-FF3D00?logo=sdl&logoColor=white)
![Box2D](https://img.shields.io/badge/Physics-Box2D-orange)

> Trabajo Práctico Final - Taller de Programación I (Cátedra Veiga) - FIUBA

**Need for Speed G25** es una implementación multijugador del clásico juego de carreras. Desarrollado desde cero utilizando **C++**, implementando un modelo **Cliente-Servidor**, y **Qt** para la renderización gráfica y el editor.

---
## 🌐 Sitio Web Oficial

¡Visita nuestra Landing Page para ver el trailer y descargar el juego con estilo retro!

**[juli-rivero.github.io/need-for-speed-g25](https://juli-rivero.github.io/need-for-speed-g25/)**

---

## 🚀 Sobre el Proyecto

Este proyecto busca replicar la experiencia arcade de carreras con las siguientes características técnicas:
- **Multijugador:** Arquitectura Cliente-Servidor concurrente.
- **Físicas:** Simulación de colisiones y movimiento en el servidor.
- **Editor de Mapas:** Herramienta gráfica para crear pistas personalizadas.

El enunciado original se encuentra disponible [aquí](https://github.com/juli-rivero/need-for-speed-g25/blob/main/docs/2025.02.trabajo-practico.need-for-speed.pdf).

---

## 🛠️ Instalación

El proyecto ha sido probado en **Ubuntu/Linux Mint**.

El repositorio incluye un script que instala **todas las dependencias necesarias** (Qt, CMake, herramientas de compilación) y compila el proyecto.

1.
   **Opciòn A - Clonar el repositorio:**
   ```bash
   git clone [https://github.com/juli-rivero/need-for-speed-g25.git](https://github.com/juli-rivero/need-for-speed-g25.git)
   cd need-for-speed-g25
   ````
   **Opción B — Descargar ZIP desde Releases**
    Descomprimir:
    ```bash
    unzip <nombre del zip>
    cd <nombre del zip>
    ```

3.  **Ejecutar el instalador:**
    ```bash
    chmod +x installer.sh
    sudo ./installer.sh
    ```
    *\> Nota: Se requieren permisos de `sudo` para instalar las librerías de Qt y compiladores.*

## 🎮 Ejecución

Una vez compilado, puedes utilizar los siguientes comandos definidos en el `Makefile`:

### 1\. Servidor

Inicia el servidor para aceptar conexiones.

```bash
make exec-server
```

### 2\. Cliente

Inicia una instancia del juego para conectarse al servidor.

```bash
make exec-client
```

### 3\. Editor de Niveles

Abre el editor gráfico para crear o modificar pistas.

```bash
make exec-editor
```

-----

## 📚 Documentación

Para detalles profundos sobre la arquitectura, el protocolo de comunicación y los diagramas de clase, visita nuestra **Wiki**:
👉 [Documentación Técnica (Wiki)](https://github.com/juli-rivero/need-for-speed-g25/wiki)

-----

## 👥 Integrantes (Grupo 25)

| Nombre | GitHub |
|:--- |:--- |
| **Elvis Quispe Limachi** | [@Elvis-Quispe](https://github.com/ElvisQ) |
| **Franco Lucchesi** | [@Franco-Lucchesi](https://github.com/lucchesifd) |
| **Nicolas Francisco Mazzeo** | [@Nicolas-Mazzeo](https://github.com/Nicolas-Mazzeo) |
| **Julián Rivero** | [@juli-rivero](https://github.com/juli-rivero) |

Agradecemos por la ayuda de nuestro corrector, quién siempre estuvo a disposición en todo momento, Iván Erlich.
