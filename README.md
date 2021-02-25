# TP2 Via Unica

# Sistemas operativos

## Descripción

En este trabajo se simula el paso de trenés, que van en vias paralelas y opuestas, por una via única. Al ser una sola vía no pueden ingresar trenes de sentido puesto porque estos chocarían asi que para eso se tiene un programa que permite el ingreso en un solo sentido a la vez.


## Instalación

1) Descargar el ultimo release de la pagina de GitHub (Source code.zip)

2) Descomprimir


## Modo de uso

1) Ingresar a la carpeta "Via_unica"

2) Abrir una terminal dentro del directorio

3) Ejecutar el siguiente comando
```$ make all```

Se crearan varios ejecutables

4) En la terminal ejecutar el siguiente comando
```./inicializador```

En un principio el paso de trenes estara habilitado de Sur a Norte, identificado con las letras SN.

Desde este momento el usuario utilizara 3 programas para poder interactuar con la simulación

```./trenNS```  coloca un tren en la entrada norte

```./trenSN``` coloca un tren en la entrada sur

```./coordinador <NS o SN>:``` cambia el sentido de circulación de los trenes según el parámetro dado por entrada estandar NS para habilitar la entrada NORTE, SN para habilitar la SUR.


Para finalizar con la simulación debe ejecutar el siguiente comando

```./terminador```




