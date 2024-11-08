# 2024-G6-MQTT-JWT

En este proyecto se Autorizan dispositivos IoT al enviar mensajes MQTT, para ello se verifican JWT (JSON Web Tokens) antes de permitir que un cliente publique a temas específicos. El plugin del broker Mosquitto está desarrollado en C y utiliza la biblioteca `jwt` para decodificar y verificar tokens JWT. Mientras que el Firmware del dispositivo IoT esta desarrollado en Arduino y esta destinado a un ESP32 que publica datos de temperatura captados por un sensor AHT10.

## Características

- **Autenticación JWT**: Verifica que los clientes envíen un JWT válido antes de autorizarlos.
- **Verificación de caducidad**: Comprueba el reclamo `exp` en el token para asegurarse de que el token no esté expirado.
- **Publicación de payload JWT**: Publica un mensaje en un tema específico si el dispositivo es autorizado.
- **Codificacion de JWT en dispositivo IoT**: Un ESP32 codifica los datos de temperatura en un JWT.
- **Publicacion periodica de Temperatura**: Se captura periodicamente la temperatura a travez de un AHT10.

## Estructura de Archivos

- `jwt_auth_plugin.c`: Código fuente del plugin de autorizacion.
- `auth_plugin.h`: Archivo de cabecera para las definiciones de funciones y macros necesarias.
- `firmware.ino`: Codigo fuente del ESP32.
  
## Dependencias

- **Mosquitto 2.0.11 o superior**: Para asegurarse de que la API de plugin sea compatible.
- **Biblioteca jwt**: Para decodificar y verificar tokens JWT.

## Configuracion

Configura tu broker mosquito editando el archivo `/etc/mosquitto/mosquitto.conf`

```conf
    pid_file /run/mosquitto/mosquitto.pid

    persistence true
    persistence_location /var/lib/mosquitto/

    # Logging configuration
    log_dest file /var/log/mosquitto/mosquitto.log
    log_type all

    include_dir /etc/mosquitto/conf.d

    listener 1122
    allow_anonymous true

    # Load your plugin
    plugin /usr/lib/mosquitto/mqtt_auth_plugin.so
```

## Compilación

Para compilar este plugin, asegúrate de tener instaladas las dependencias requeridas e incluir el path `/snap/mosquitto/current/usr/include`

```bash
    $ make
    $ mv /PATH/TO/PROJECT/bin/mqtt_auth_plugin.so /usr/lib/mosquitto/mqtt_auth_plugin.so
    $ systemctl restart mosquitto
```

## Licencia

Este proyecto está bajo la Licencia GPL 3.0. Consulta el archivo `LICENSE` para más detalles.
