SET(TARGET_SRC
    SqliteArchive.cpp
    SqliteDatabase.cpp
    Sqlite.cpp
)

SET(TARGET_H
    SqliteArchive.h
    SqliteDatabase.h
    Sqlite.h
)

SET(TARGET_ADDED_LIBRARIES OCLayer)

SET(TARGET_EXTERNAL_LIBRARIES 
    sqlite3
)

SETUP_PLUGIN(sqlite)
