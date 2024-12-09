# 2024-G6-MQTT-JWT

En este proyecto se Autorizan dispositivos IoT al enviar mensajes MQTT, para ello se verifican JWT (JSON Web Tokens) antes de permitir que un cliente publique a temas específicos. El plugin del broker Mosquitto está desarrollado en C y utiliza la biblioteca `jwt` para decodificar y verificar tokens JWT. Mientras que el Firmware del dispositivo IoT esta desarrollado en Arduino y esta destinado a un ESP32 que publica datos de temperatura y humedad captados por un sensor AHT10, y a su vez estos pueden ser visualizados en una interfaz gráfica por medio de Node-RED.

<p align="center">
  <img src="assets/Dispositivo IoT.png" alt="Diagrama">
</p>


## Características

- **Autenticación JWT**: Verifica que los clientes envíen un JWT válido antes de autorizarlos.
- **Verificación de caducidad**: Comprueba el reclamo `exp` en el token para asegurarse de que el token no esté expirado.
- **Publicación de payload JWT**: Publica un mensaje en un tema específico si el dispositivo es autorizado.
- **Codificacion de JWT en dispositivo IoT**: Un ESP32 codifica los datos de temperatura en un JWT.
- **Publicacion periodica de Temperatura-Humedad**: Se captura periodicamente la temperatura y la humedad a traves de un AHT10.
- **Visualización mediante Node-RED**: Los datos publicados se procesan y visualizan de manera gráfica utilizando Node-RED.

## Estructura de Archivos

- `jwt_auth_plugin.c`: Código fuente del plugin de autorizacion.
- `auth_plugin.h`: Archivo de cabecera para las definiciones de funciones y macros necesarias.
- `firmware.ino`: Codigo fuente del ESP32.
  
## Dependencias

- **Mosquitto 2.0.11 o superior**: Para asegurarse de que la API de plugin sea compatible.
- **libjwt 1.117.2 o superior**: Para decodificar y verificar tokens JWT en C.

## Compilación

Para compilar este plugin, asegúrate de tener instaladas las dependencias requeridas e incluir el path `/snap/mosquitto/current/usr/include`

```bash
    $ make
    $ mv /PATH/TO/PROJECT/bin/mqtt_auth_plugin.so /usr/lib/mosquitto/mqtt_auth_plugin.so
```

## Configuracion

- **Mosquitto**

Cree un archivo con usuarios que esten autorizados a publicar y suscribir,
para ello en la carpeta `/etc/mosquitto/`

```bash
    $ mosquitto_passwd -c passwd user #Crea el archivo passwd con un usuario
    $ mosquitto_passwd -b passwd user password #Agrega un usuario al archivo passwd
    $ mosquitto_passwd -D passwd user #Borra un usuario del archivo
```
Luego de tener el archivo `mqtt_auth_plugin.so` y `passwd`.
Configura tu broker mosquito editando el archivo `/etc/mosquitto/mosquitto.conf`

```conf
    pid_file /run/mosquitto/mosquitto.pid

    persistence true
    persistence_location /var/lib/mosquitto/

    # Logging configuration
    log_dest file /var/log/mosquitto/mosquitto.log
    log_type all

    include_dir /etc/mosquitto/conf.d

    #Auth configuration
    allow_anonymous false
    password_file /etc/mosquitto/passwd

    # Encoded IoT listener
    listener 1122
    protocol mqtt
    plugin /usr/lib/mosquitto/mqtt_auth_plugin.so
```
Finalmente reinicie el servicio de broker para aplicar estos cambios
```bash
    $ sudo systemctl restart mosquitto
```

- **Node-RED**

En este proyecto es necesario [Node-Red v4.0.5](https://github.com/node-red/node-red/releases/tag/4.0.5).
Ademas de los nodo necesarios:
####
    node-red-dashboard
    node-red-contrib-mqtt

Estos pueden ser instalador por el gestor de paquetes en `Manage Palette`

Inicialize el servicio por terminal
```bash
    $ node-red
```
Luego de haber iniciado, se debe de entrar al navegador y acceder a [localhost:1880](http://localhost:1880).

Importe el _flow_ o workspace desde el `menú-importar`, seleccion el archivo del proyecto: `node_red/flows.json`.

En la interfaz grafica configure las credenciales para el topico mediciones/temp_hum,
estas son las credenciales que asigno al archivo `/etc/mosquitto/passwd`
Una vez configurado, se debe correr a través del botón 'Deploy' que se encuentra en el margen superior derecho.

Por último, se accede al dashboard desde [localhost/ui](http://localhost:1880/ui) donde se visualizan
los datos de humedad y temperatura publicados por el dispositivo IoT.

## Ejemplos
[![Casos de Uso](https://drive.google.com/thumbnail?id=1fClLjQQVeiKHwgglp4CNxAXF6xguolMV)](https://drive.google.com/file/d/1fClLjQQVeiKHwgglp4CNxAXF6xguolMV)
## Licencia

Este proyecto está bajo la Licencia GPL 3.0. Consulta el archivo `LICENSE` para más detalles.
