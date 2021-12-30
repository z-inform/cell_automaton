struct CoordPair {
    char x;
    char y;
};

struct CoordArr {
    unsigned char cell_type;
    struct CoordPair* cell_coords;
};

struct CellNumber {
    unsigned char cell_type;
    unsigned char cell_number;
};

struct RuleStruct {
    unsigned char target_type;
    struct CoordArr* coord_rules;
    struct CellNumber* number_rules;
};

