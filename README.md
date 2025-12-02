# Remake Need for speed

## Enunciado

El enunciado, provisto por la catedra Veiga de Taller de Programación se encuentra en [need-for-speed-g25/docs/2025.02.trabajo-practico.need-for-speed.pdf](https://github.com/juli-rivero/need-for-speed-g25/blob/main/docs/2025.02.trabajo-practico.need-for-speed.pdf)

## Documentación

La documentación tecnica del proyecto se encuentra en [need-for-speed-g25/wiki](https://github.com/juli-rivero/need-for-speed-g25/wiki)

## Integrantes G25

- Elvis Quispe Limachi
- Franco Lucchesi
- Nicolas Francisco Mazzeo
- Julián Rivero Julián

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
