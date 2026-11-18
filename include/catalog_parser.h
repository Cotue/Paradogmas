#ifndef CATALOG_PARSER_H
#define CATALOG_PARSER_H

#include "models/catalog.h"
#include "constants.h"

Status parse_catalog(
    const char *filepath,
    Catalog *catalog,
    const char *plan_code
);

#endif