// =============== COMPARATIVO_PRODUTO ===============
int comparativo_produto(const void* a, const void* b) {
    const Produto* pa = (const Produto*)a;
    const Produto* pb = (const Produto*)b;

    if (pa->id < pb->id) {
        return -1;
    }

    if (pa->id > pb->id) {
        return 1;
    }

    return 0;
}


// =============== COMPARATIVO_COMPRA ===============
int comparativo_compra(const void *a, const void *b) {
    CompraRegistro *ca = (CompraRegistro *) a;
    CompraRegistro *cb = (CompraRegistro *) b;

    if(ca->usuario_id < cb->usuario_id) {
        return -1;
    }
    if(ca->usuario_id > cb->usuario_id) {
        return 1;
    }

    return 0;
}


// =============== COMPARATIVO_USUARIO_AGREGADO ===============
int comparativo_usuario_agreg(const void *a, const void *b) {
    AgregadoUsuario *ca = (AgregadoUsuario *) a;
    AgregadoUsuario *cb = (AgregadoUsuario *) b;

    if(ca->total > cb->total) {
        return -1;
    }
    if(ca->total < cb->total) {
        return 1;
    }

    return 0;
}


// =============== COMPARATIVO_CATEGORIA_AGREGADO ===============
int comparativo_cat_agreg(const void *a, const void *b) {
    AgregadoCategoria *ca = (AgregadoCategoria *) a;
    AgregadoCategoria *cb = (AgregadoCategoria *) b;

    if(ca->total > cb->total) {
        return -1;
    }
    if(ca->total < cb->total) {
        return 1;
    }

    return 0;
}
