# Ejemplo de CMAKE para el TP Final

**Importante:** el primer commit de este repositorio tiene el setup
básico para el TP Final que incluye la instalación de la lib de Google
Tests, `SDL2` y `SDL2pp` (el wrapper de C++).

El resto de los commits son a modo de ejemplo de como se pueden
agregar mas código fuente al proyecto.

Este ejemplo **no** incluye instalarse otras libs que son dependencias
tanto de `SDL2` como de `SDL2pp` (pero si estan documentadas en el
`CMakeFile` para Xubuntu 24.04).

Este repo **no** incluye tampoco un instalador.

**Se deben agregar las librerias necesarias y el instalador.**

También el ejemplo usa una estructura de carpetas muy simple:

```
client/
server/
editor/
common/
```

Bien se puede mejorar (cambiando el cmakefile) agregando mas
sub-carpetas.

Asi tambien **deben** ser cambiados los *targets* del cmake (`taller_client`,
`taller_server`, ...) por nombres mas acordes al TP que se este
haciendo.

Tambien, por default solo se compila una version *debug* sin
optimizar. Si se quiere compilar binarios optimizados
(lo que cmake llama *release*) se puede, solo hay modificar
cmake.

Aprender del ejemplo para saber como extenderlo!

**Importante:** este repositorio **no** incluye pre-commits hooks,
ni scripts adicionales (como correr valgrind).

**Eso esta a cargo de los estudiantes,** tal como fue mostrado
en los tps individuales, recaps y hands-on.

---
#  Instalación:
El proyecto incluye un instalador automático que configura **todas las dependencias**, compila el juego y deja todo listo para ejecutar en Ubuntu.
##  1. Descargar el juego
Podés:

### **Opción A — Descargar ZIP desde Releases**
Descomprimir:

```bash
unzip need-for-speed-g25-v1.0.zip
cd need-for-speed-g25-installer
```

### **Opción B — Clonar el repositorio**
```bash
git clone https://github.com/<usuario>/<repo>.git
cd <repo>
```

##  2. Ejecutar el instalador

```bash
chmod +x installer.sh
sudo ./installer.sh
```

## 3. Ejecutar el juego

### Lanzar el servidor:
```bash
make exec-server
```
### Lanzar un cliente:
```bash
make exec-client
```
### Lanzar el editor:
```bash
make exec-editor
```
