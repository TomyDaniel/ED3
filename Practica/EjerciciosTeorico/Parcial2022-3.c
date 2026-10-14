/*
    En la siguiente sección de código se realiza la configuración de un canal de DMA
    a - Explique detalladamente cómo queda configurado el canal, que tipo de transferencia está realizando.
    b - ¿Qué datos se transfieren, de qué posición a cuál y cuántas veces?
    c- ¿Cómo se define el tiempo de "Interrup DMA request"  o el tiempo de transferencia de c/dato?
*/

void confDMA(void){
    GPDMA_LLI_Type DMA_LLI_Struct1;
    GPDMA_LLI_Type DMA_LLI_Struct2;

    DMA_LLI_Struct1.SrcAddr  = (uint32_t)(una_tabla_global);
    DMA_LLI_Struct1.DstAddr  = (uint32_t)(otra_tabla_global);
    DMA_LLI_Struct1.NextLLI  = (uint32_t)(&DMA_LLI_Struct2);

    DMA_LLI_Struct2.SrcAddr  = (uint32_t)(otra_tabla_global);
    DMA_LLI_Struct2.DstAddr  = (uint32_t)(una_tabla_global);
    DMA_LLI_Struct2.NextLLI  = (uint32_t)(&DMA_LLI_Struct1);

    DMA_LLI_Struct.Control = TAMAÑO_DE_TABLAS    // DECLARADA POR DEFINE
        | (2<<18)
        | (2<<21)
        | (1<<27)
        | (1<<26)
        ;

    GPDMA_Init();

    GPDMACfg.ChanneNum      = 0;
    GPDMACfg.SrcMemAddr     = (uint32_t)(una_tabla_global);
    GPDMACfg.DstMemAddr     = (uint32_t)(otra_tabla_global);
    GPDMACfg.TransferSize   = TAMAÑO_DE_TABLAS;
    GPDMACfg.TransferWidth  = 0;
    GPDMACfg.TransferType   = GPDMA_TRANSFERTYPE_M2M;
    GPDMACfg.SrcConn        = 0;
    GPDMACfg.DstConn        = 0;
    GPDMACfg.DMALLI         = (uint32_t)(&DMA_LLI_Struct2);

    GPDMA_Setup(&GPDMACfg);

    return;
}

/*
    a- Es transferencia de memoria a memoria. Se usan 2 listas enlazadas. Canal 0 de DMA. Maxima prioridad. Ancho de palabra un byte
    b- Se transfiere datos de memoria de una tabla a otra. Quedan haciendo buffers alternados
    c- Se transfiere a la velocidad del CCLK pq es memoria a memoria

    Hay una inconsistencia en el codigo
*/