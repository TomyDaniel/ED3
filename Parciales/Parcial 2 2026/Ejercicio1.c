/*
EJERCICIO N° 1 (30%): 25%
En la siguiente sección de código se realiza la configuración de un canal de DMA:
a) Explique detalladamente cómo queda configurado el canal y qué tipo de transferencia está realizando.
b) ¿Qué datos se transfieren, de qué posición a qué posición y cuántas veces?
c) Realizar las modificaciones necesarias en el código para que la próxima región de memoria a 
transferir sea la que está a continuación de la región definida por "una_tabla_global" y su destino sea la posición de memoria "0x10002000".
*/

void confDMA(void) {

    GPDMA_LLI_T LLI_Struct1;
    LLI_Struct1.srcAddr = (uint32_t)(una_tabla_global);
    LLI_Struct1.dstAddr = (uint32_t)(otra_tabla_global);
    LLI_Struct1.nextLLI = (uint32_t)&LLI_Struct2;

    static GPDMA_LLI_T DMA_LLI_Struct2;
    LLI_Struct2.srcAddr = (uint32_t)(otra_tabla_global);
    LLI_Struct2.dstAddr = (uint32_t)(una_tabla_global);
    LLI_Struct2.nextLLI = (uint32_t)&LLI_Struct1;

    //Buffer circular

    LLI_Struct.control = TAMAÑO_DE_TABLAS | (3 << 18) | (1 << 21) | (1 << 26) | (1 << 27); // Bien excepto el 3 << 18. Deberia ir halfword
    GPDMA_Init();

    DMACfg.channelNum = GPDMA_CH_0;
    DMACfg.transferSize = TAMAÑO_DE_TABLAS; // Cantidad de muestras
    DMACfg.type = GPDMA_M2M; // ok
    DMACfg.srcMemAddr = (uint32_t)(una_tabla_global);
    DMACfg.dstMemAddr = (uint32_t)(otra_tabla_global);
    DMACfg.src.width = GPDMA_HALFWORD;
    DMACfg.src.increment = ENABLE; // Bien. Faltaria la de destino
    DMACfg.dst.width = GPDMA_HALFWORD;
    DMACfg.linkedList = (uint32_t)&LLI_Struct2; // Llamo a a struct2 

    GPDMA_SetupChannel(&DMACfg);
    return;
}

/*
a-  Se configura el canal 0 para transmision memoria a memoria. Se transmite desde "una tabla global" a "otra tabla global" y en viceversa teniendo asi un buffer circular.
    Se transmite media palabra y una cantidad de muestras igual al tamanio de la tabla.

b-  Se transfiere tamanio de tabla por media palabra, osea 2 bytes de una posicion que es la de una tabla global a otra tabla global y viceversa. Se transfieren infinitas veces. Buffer circular

c-  Lo que esta a continuacion una_tabla_global + 2*tamanio de tablas cambiar en src y en dst poner 0x10002000.
*/
