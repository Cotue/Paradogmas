#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "models/catalog.h"
#include "constants.h"

 
Status serialize_catalog_to_json(const Catalog *catalog, const char *filepath);

#endif 