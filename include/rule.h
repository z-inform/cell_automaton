#pragma once

typedef struct CoordPair {
    char x;
    char y;
} CoordPairType;

typedef struct CoordArr {
    unsigned char cell_type;
    struct CoordPair* cell_coords;
} CoordArrType;

typedef struct CellNumber {
    unsigned char cell_type;
    unsigned char cell_number;
} CellNumberType;

typedef struct RuleStruct {
    unsigned char target_type;
    struct CoordArr* coord_rules;
    struct CellNumber* number_rules;
} RuleType;

